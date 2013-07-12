/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
/*
 * MediaAudioImpl.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: aclarke
 */

#include "MediaAudioImpl.h"
#include "AVBufferSupport.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

MediaAudioImpl::MediaAudioImpl() {
  mFrame = av_frame_alloc();
  if (!mFrame) throw std::bad_alloc();
  mFrame->opaque = this;
  mComplete = false;
}

MediaAudioImpl::~MediaAudioImpl() {
  av_frame_free(&mFrame);
}

MediaAudioImpl*
MediaAudioImpl::make(int32_t numSamples, int32_t channels,
    AudioChannel::Layout layout,
    AudioFormat::Type format) {
  if (numSamples <= 0) {
    VS_LOG_ERROR("No samples specified");
    return 0;
  }
  if (channels <= 0) {
    VS_LOG_ERROR("No channels specified");
    return 0;
  }
  if (format == AudioFormat::SAMPLE_FMT_NONE) {
    VS_LOG_ERROR("No audio format specified");
    return 0;
  }

  int32_t bufSize = av_samples_get_buffer_size(0, channels, numSamples, (enum AVSampleFormat)format, 0);
  RefPointer<IBuffer> buffer = IBuffer::make(0, bufSize);
  MediaAudioImpl* retval = make(buffer.value(), numSamples, channels, layout, format);
  if (retval)
    buffer->setJavaAllocator(retval->getJavaAllocator());
  return retval;
}

MediaAudioImpl*
MediaAudioImpl::make(io::humble::ferry::IBuffer* buffer,
    int32_t numSamples,
    int32_t channels,
    AudioChannel::Layout layout,
    AudioFormat::Type format) {
  if (channels <= 0) {
    VS_LOG_ERROR("No channels specified");
    return 0;
  }
  if (format == AudioFormat::SAMPLE_FMT_NONE) {
    VS_LOG_ERROR("No audio format specified");
    return 0;
  }
  if (!buffer) {
    VS_LOG_ERROR("No audio buffer specified");
    return 0;
  }
  // get the required buf size
  int32_t linesize = 0;
  int32_t bufSize = av_samples_get_buffer_size(&linesize, channels, numSamples, (enum AVSampleFormat)format, 0);
  if (bufSize < buffer->getBufferSize()) {
    VS_LOG_ERROR("passed in buffer too small to fit requested num samples: %d", numSamples);
    return 0;
  }

  // By default we are always going to try and manage audio
  // through IBuffers, but we cannot guarantee that FFmpeg won't
  // free them and replace them with their own objects, so we
  // must let mFrame->buf[] and mFrame->extended_buf[] win.
  RefPointer<MediaAudioImpl> retval = make();
  AVFrame* frame = retval->mFrame;
  frame->nb_samples = numSamples;
  frame->channels = channels;
  frame->format = format;
  frame->channel_layout = layout;

  // now we have to layout the audio FFmpeg makes this hard if you pass
  // in your own buffers.
  int planar   = av_sample_fmt_is_planar((enum AVSampleFormat)frame->format);
  int planes   = planar ? channels : 1;
  int ret;

  if (!frame->linesize[0]) {
    frame->linesize[0] = linesize;
  }
  // now, let's fill all of those extended_data objects.
  if (planes > AV_NUM_DATA_POINTERS) {
      frame->extended_data = (uint8_t**)av_mallocz(planes *
                                        sizeof(*frame->extended_data));
      frame->extended_buf  = (AVBufferRef**)av_mallocz((planes - AV_NUM_DATA_POINTERS) *
                                        sizeof(*frame->extended_buf));
      if (!frame->extended_data || !frame->extended_buf) {
          av_freep(&frame->extended_data);
          av_freep(&frame->extended_buf);
          return 0;
      }
      frame->nb_extended_buf = planes - AV_NUM_DATA_POINTERS;
  } else
      frame->extended_data = frame->data;

  // now, let's fill in those buffers.
  frame->buf[0] = AVBufferSupport::wrapIBuffer(buffer);
  // all the rest should be zero.

  // fill in the extended_data planes
  uint8_t* buf = (uint8_t*)buffer->getBytes(0, bufSize);
  ret = av_samples_fill_arrays(frame->extended_data, &frame->linesize[0],
      buf,
      frame->channels,
      frame->nb_samples,
      (enum AVSampleFormat)frame->format,
      0);
  if (ret <= 0)
    return 0;

  // patch up the first line.
  frame->data[0] = frame->extended_data[0];

  // now create references to our one mega buffer in all other pointers
  for (int32_t i = 1; i < FFMIN(planes, AV_NUM_DATA_POINTERS); i++) {
    frame->data[i] = frame->extended_data[i];
    frame->buf[i] = av_buffer_ref(frame->buf[0]);
  }
  // and add refs for the final buffers
  for (int32_t i = 0; i < planes - AV_NUM_DATA_POINTERS; i++) {
    frame->extended_buf[i] = av_buffer_ref(frame->buf[0]);
  }
  return retval.get();
}

io::humble::ferry::IBuffer*
MediaAudioImpl::getData(int32_t plane) {
  (void) plane;
  return 0;
}

int32_t
MediaAudioImpl::getDataPlaneSize() {
  return mFrame->linesize[0];
}

int32_t
MediaAudioImpl::getNumDataPlanes() {
  if (isPlanar()) return mFrame->channels;
  else return 1;
}

int32_t
MediaAudioImpl::getMaxNumSamples() {
  return mFrame->nb_samples;
}

int32_t
MediaAudioImpl::getBytesPerSample() {
  return AudioFormat::getBytesPerSample((AudioFormat::Type) mFrame->format);
}

void
MediaAudioImpl::setComplete(bool complete, uint32_t numSamples,
    int32_t sampleRate, int32_t channels, AudioFormat::Type format,
    int64_t pts) {
  (void) complete;
  (void) numSamples;
  (void) sampleRate;
  (void) channels;
  (void) format;
  (void) pts;
}

int32_t
MediaAudioImpl::getSampleRate() {
  return mFrame->sample_rate;
}

int32_t
MediaAudioImpl::getChannels() {
  return mFrame->channels;
}

AudioFormat::Type
MediaAudioImpl::getFormat() {
  return (AudioFormat::Type) mFrame->format;
}

bool
MediaAudioImpl::isComplete() {
  return mComplete;
}

bool
MediaAudioImpl::isKey() {
  return mFrame->key_frame;
}

bool
MediaAudioImpl::isPlanar() {
  return AudioFormat::isPlanar((AudioFormat::Type) mFrame->format);
}

AudioChannel::Layout
MediaAudioImpl::getChannelLayout() {
  return (AudioChannel::Layout) mFrame->channel_layout;
}

AVFrame*
MediaAudioImpl::getCtx() {
  return mFrame;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
