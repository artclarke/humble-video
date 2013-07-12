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
MediaAudioImpl::make(int32_t maxSamples, int32_t channels,
    AudioFormat::Type format) {
  if (maxSamples <= 0) {
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

  int32_t bufSize = av_samples_get_buffer_size(0, channels, maxSamples, (enum AVSampleFormat)format, 0);
  RefPointer<IBuffer> buffer = IBuffer::make(0, bufSize);
  MediaAudioImpl* retval = make(buffer.value(), channels, format);
  if (retval)
    buffer->setJavaAllocator(retval->getJavaAllocator());
  return retval;
}

MediaAudioImpl*
MediaAudioImpl::make(io::humble::ferry::IBuffer* buffer, int32_t channels,
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
  // By default we are always going to try and manage audio
  // through IBuffers, but we cannot guarantee that FFmpeg won't
  // free them and replace them with their own objects, so we
  // must let mFrame->buf[] and mFrame->extended_buf[] win.
  int32_t bytesPerSample = AudioFormat::getBytesPerSample(format);
  int32_t numSamples = buffer->getBufferSize() / bytesPerSample; // flooring is correct
  MediaAudioImpl* retval = make();
  retval->mFrame->nb_samples = numSamples;
  retval->mFrame->channels = channels;
  retval->mFrame->format = format;

  // now we have to layout the audio FFmpeg makes this hard if you pass
  // in your own buffers.
  return retval;
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
