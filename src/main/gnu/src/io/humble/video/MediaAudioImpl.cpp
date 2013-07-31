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
#include <io/humble/ferry/HumbleException.h>
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
MediaAudioImpl::make(int32_t numSamples, int32_t sampleRate, int32_t channels,
    AudioChannel::Layout layout, AudioFormat::Type format) {
  if (numSamples <= 0) {
    VS_THROW(HumbleInvalidArgument("No samples specified"));
  }
  if (channels <= 0) {
    VS_THROW(HumbleInvalidArgument("No channels specified"));
  }
  if (format == AudioFormat::SAMPLE_FMT_NONE) {
    VS_THROW(HumbleInvalidArgument("No audio format specified"));
  }

  int32_t bufSize = av_samples_get_buffer_size(0, channels, numSamples,
      (enum AVSampleFormat) format, 0);
  RefPointer<Buffer> buffer = Buffer::make(0, bufSize);
  MediaAudioImpl* retval = make(buffer.value(), numSamples, sampleRate, channels, layout,
      format);
  if (retval) buffer->setJavaAllocator(retval->getJavaAllocator());
  return retval;
}

MediaAudioImpl*
MediaAudioImpl::make(io::humble::ferry::Buffer* buffer, int32_t numSamples,
    int32_t sampleRate,
    int32_t channels, AudioChannel::Layout layout, AudioFormat::Type format) {
  if (numSamples <= 0) {
    VS_THROW(HumbleInvalidArgument("No samples specified"));
  }
  if (channels <= 0) {
    VS_THROW(HumbleInvalidArgument("No channels specified"));
  }
  if (sampleRate <= 0) {
    VS_THROW(HumbleInvalidArgument("No sample rate specified"));
  }
  if (format == AudioFormat::SAMPLE_FMT_NONE) {
    VS_THROW(HumbleInvalidArgument("No audio format specified"));
  }
  if (!buffer) {
    VS_THROW(HumbleInvalidArgument("No audio buffer specified"));
  }
  if (layout != AudioChannel::CH_LAYOUT_UNKNOWN) {
    // let's do a sanity check
    if (channels != AudioChannel::getNumChannelsInLayout(layout)) {
      VS_LOG_ERROR("Passed in channel layout does not match number of channels. Layout: %s. Expected Channels: %d. Actual Channels: %d",
          AudioChannel::getLayoutName(layout),
          AudioChannel::getNumChannelsInLayout(layout),
          channels
          );
      VS_THROW(HumbleInvalidArgument("Channel layout does not match number of channels"));
    }
  }
  // get the required buf size
  int32_t linesize = 0;
  int32_t bufSize = av_samples_get_buffer_size(&linesize, channels, numSamples,
      (enum AVSampleFormat) format, 0);
  if (bufSize < buffer->getBufferSize()) {
    VS_THROW(HumbleInvalidArgument("passed in buffer too small to fit requested num samples"));
  }

  // By default we are always going to try and manage audio
  // through Buffers, but we cannot guarantee that FFmpeg won't
  // free them and replace them with their own objects, so we
  // must let mFrame->buf[] and mFrame->extended_buf[] win.
  RefPointer<MediaAudioImpl> retval = make();
  AVFrame* frame = retval->mFrame;
  av_frame_set_sample_rate(frame, sampleRate);
  av_frame_set_channels(frame, channels);
  av_frame_set_channel_layout(frame, layout);
  frame->nb_samples = numSamples;
  frame->format = format;
  // we're going to tell the world this buffer now contains the right kind of data
  setBufferType((AudioFormat::Type)frame->format, buffer);

  // now we have to layout the audio FFmpeg makes this hard if you pass
  // in your own buffers.
  bool planar = retval->isPlanar();
  int planes = planar ? channels : 1;
  int ret;

  if (!frame->linesize[0]) {
    frame->linesize[0] = linesize;
  }
  // now, let's fill all of those extended_data objects.
  if (planes > AV_NUM_DATA_POINTERS) {
    frame->extended_data = (uint8_t**) av_mallocz(
        planes * sizeof(*frame->extended_data));
    frame->extended_buf = (AVBufferRef**) av_mallocz(
        (planes - AV_NUM_DATA_POINTERS) * sizeof(*frame->extended_buf));
    if (!frame->extended_data || !frame->extended_buf) {
      av_freep(&frame->extended_data);
      av_freep(&frame->extended_buf);
      VS_THROW(HumbleBadAlloc());
    }
    frame->nb_extended_buf = planes - AV_NUM_DATA_POINTERS;
  } else frame->extended_data = frame->data;

  // fill in the extended_data planes
  uint8_t* buf = (uint8_t*) buffer->getBytes(0, bufSize);
  ret = av_samples_fill_arrays(frame->extended_data, &frame->linesize[0], buf,
      retval->getChannels(), numSamples, (enum AVSampleFormat) frame->format,
      0);
  if (ret < 0) {
    VS_THROW(HumbleRuntimeError("Could not layout all the audio data; fatal error"));
  }

  // now create references to our one mega buffer in all other pointers
  for (int32_t i = 0; i < FFMIN(planes, AV_NUM_DATA_POINTERS); i++) {
    frame->data[i] = frame->extended_data[i];
    frame->buf[i] = AVBufferSupport::wrapBuffer(buffer, frame->extended_data[i], frame->linesize[0]);
  }
  // and add refs for the final buffers
  for (int32_t i = 0; i < planes - AV_NUM_DATA_POINTERS; i++) {
    frame->extended_buf[i] = AVBufferSupport::wrapBuffer(buffer, frame->extended_data[i+AV_NUM_DATA_POINTERS], frame->linesize[0]);
  }
  return retval.get();
}


void
MediaAudioImpl::copy(AVFrame* src, bool complete) {
  if (!src)
    VS_THROW(HumbleInvalidArgument("no src"));
  // release any memory we have
  av_frame_unref(mFrame);
  // and copy any data in.
  av_frame_ref(mFrame, src);
  mComplete=complete;
}

MediaAudioImpl*
MediaAudioImpl::make(MediaAudioImpl* src, bool copy) {
  RefPointer<MediaAudioImpl> retval;

  if (!src) VS_THROW(HumbleInvalidArgument("no src object to copy from"));

  if (copy) {
    // first create a new mediaaudio object to copy into
    retval = make(src->getMaxNumSamples(),
        src->getSampleRate(),
        src->getChannels(),
        src->getChannelLayout(),
        src->getFormat());

    retval->setComplete(src->isComplete());
    retval->setNumSamples(src->getNumSamples());

    // then copy the data into retval
    int32_t n = src->getNumDataPlanes();
    for(int32_t i = 0; i < n; i++ )
    {
      AVBufferRef* dstBuf = av_frame_get_plane_buffer(retval->mFrame, i);
      AVBufferRef* srcBuf = av_frame_get_plane_buffer(src->mFrame, i);
      VS_ASSERT(dstBuf, "should always have buffer");
      VS_ASSERT(srcBuf, "should always have buffer");
      memcpy(dstBuf->data, srcBuf->data, srcBuf->size);
    }
  } else {
    // first create a new media audio object to reference into
    retval = make();

    // then do the reference
    av_frame_ref(retval->mFrame, src->mFrame);
    retval->setComplete(src->isComplete());
  }
  return retval.get();
}

io::humble::ferry::Buffer*
MediaAudioImpl::getData(int32_t plane) {
  // we get the buffer for the given plane if it exists, and wrap
  // it in an Buffer
  if (plane < 0) {
    VS_THROW(HumbleInvalidArgument("plane must be >= 0"));
  }

  if (plane >= getNumDataPlanes()) {
    VS_THROW(HumbleInvalidArgument("plane must be < getNumDataPlane()"));
  }

  // now we're guaranteed that we should have a plane.
  RefPointer<Buffer> buffer;
  if (plane < AV_NUM_DATA_POINTERS) buffer = mFrame->buf[plane] ? AVBufferSupport::wrapAVBuffer(this,
      mFrame->buf[plane], mFrame->extended_data[plane], mFrame->linesize[0]) : 0;
  else buffer = mFrame->extended_buf[plane-AV_NUM_DATA_POINTERS] ? AVBufferSupport::wrapAVBuffer(this,
      mFrame->extended_buf[plane - AV_NUM_DATA_POINTERS], mFrame->extended_data[plane], mFrame->linesize[0]) : 0;
  if (buffer)
    setBufferType((AudioFormat::Type)mFrame->format, buffer.value());
  return buffer.get();
}

int32_t
MediaAudioImpl::getDataPlaneSize(int32_t plane) {
  int32_t n = getNumDataPlanes();
  if (plane < 0 || plane >= n)
    VS_THROW(HumbleInvalidArgument("plane is out of range"));
  if (isComplete())
    return AudioFormat::getDataPlaneSizeNeeded(getNumSamples(), getChannels(), getFormat());
  else
    // oddly for audio, each plane in multiplane audio must
    // be the same as linesize[0] and sometimes ffmpeg
    // doesn't copy all that data in.
    return mFrame->linesize[0];
}

int32_t
MediaAudioImpl::getNumDataPlanes() {
  if (isPlanar()) return av_frame_get_channels(mFrame);
  else return 1;
}

int32_t
MediaAudioImpl::getMaxNumSamples() {
  int32_t bytesPerSample = AudioFormat::getBytesPerSample(getFormat());
  int32_t size = bytesPerSample ? mFrame->linesize[0] / bytesPerSample : 0;
  return size;
}

int32_t
MediaAudioImpl::getBytesPerSample() {
  return AudioFormat::getBytesPerSample((AudioFormat::Type) mFrame->format);
}

void
MediaAudioImpl::setComplete(bool complete) {
  mComplete = complete;
}

int32_t
MediaAudioImpl::getSampleRate() {
  return av_frame_get_sample_rate(mFrame);
}

int32_t
MediaAudioImpl::getChannels() {
  return av_frame_get_channels(mFrame);
}

AudioFormat::Type
MediaAudioImpl::getFormat() {
  return (AudioFormat::Type) mFrame->format;
}

int32_t
MediaAudioImpl::getNumSamples() {
  return mFrame->nb_samples;
}
void
MediaAudioImpl::setNumSamples(int32_t numSamples) {
  if (numSamples <= 0 || numSamples > getMaxNumSamples()) {
    VS_THROW(HumbleInvalidArgument("invalid number of samples to put in this MediaAudio object"));
  }
  mFrame->nb_samples = numSamples;
}


bool
MediaAudioImpl::isComplete() {
  return mComplete && mFrame->nb_samples > 0;
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
  return (AudioChannel::Layout) av_frame_get_channel_layout(mFrame);
}

AVFrame*
MediaAudioImpl::getCtx() {
  return mFrame;
}

void
MediaAudioImpl::setBufferType(AudioFormat::Type format,
    Buffer* buffer)
{
  if (!buffer)
    VS_THROW(HumbleInvalidArgument("no buffer passed in"));
  switch(format)
  {
    case AudioFormat::SAMPLE_FMT_DBL:
    case AudioFormat::SAMPLE_FMT_DBLP:
      buffer->setType(Buffer::BUFFER_DBL64);
      break;
    case AudioFormat::SAMPLE_FMT_FLT:
    case AudioFormat::SAMPLE_FMT_FLTP:
      buffer->setType(Buffer::BUFFER_FLT32);
      break;
    case AudioFormat::SAMPLE_FMT_S16:
    case AudioFormat::SAMPLE_FMT_S16P:
      buffer->setType(Buffer::BUFFER_SINT16);
      break;
    case AudioFormat::SAMPLE_FMT_S32:
    case AudioFormat::SAMPLE_FMT_S32P:
      buffer->setType(Buffer::BUFFER_SINT32);
      break;
    case AudioFormat::SAMPLE_FMT_U8:
    case AudioFormat::SAMPLE_FMT_U8P:
      buffer->setType(Buffer::BUFFER_UINT8);
      break;
    default:
      break;
  }
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
