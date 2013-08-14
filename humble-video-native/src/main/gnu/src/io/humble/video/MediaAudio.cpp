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
 * Audio.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: aclarke
 */

#include "MediaAudio.h"
#include "MediaAudioImpl.h"

namespace io {
namespace humble {
namespace video {

MediaAudio::MediaAudio() {

}

MediaAudio::~MediaAudio() {

}

MediaAudio*
MediaAudio::make(int32_t numSamples, int32_t sampleRate, int32_t channels,
    AudioChannel::Layout channelLayout, AudioFormat::Type format) {
  Global::init();
  return MediaAudioImpl::make(numSamples, sampleRate, channels, channelLayout,
      format);
}

MediaAudio*
MediaAudio::make(io::humble::ferry::Buffer *buffer, int32_t numSamples,
    int32_t sampleRate, int32_t channels, AudioChannel::Layout channelLayout,
    AudioFormat::Type format) {
  Global::init();
  return MediaAudioImpl::make(buffer, numSamples, sampleRate, channels,
      channelLayout, format);
}

MediaAudio*
MediaAudio::make(MediaAudio* src, bool copy) {
  Global::init();
  return MediaAudioImpl::make(dynamic_cast<MediaAudioImpl*>(src), copy);
}

void
AudioChannel::freeString(char* s) {
  av_free(s);
}

int64_t
AudioChannel::getChannelBitmask(const char* name) {
  if (!name || !*name) return 0;
  return av_get_channel_layout(name);
}

char*
AudioChannel::getChannelLayoutString(int32_t numChannels, int64_t layout) {
  const int bufSize = 512;
  char* retval = (char*) av_malloc(bufSize);
  if (retval) av_get_channel_layout_string(retval, bufSize, numChannels,
      (uint64_t) layout);
  return retval;
}

int32_t
AudioChannel::getNumChannelsInLayout(Layout layout) {
  return av_get_channel_layout_nb_channels((uint64_t) layout);
}

AudioChannel::Layout
AudioChannel::getDefaultLayout(int numChannels) {
  return (Layout) av_get_default_channel_layout(numChannels);
}

int32_t
AudioChannel::getIndexOfChannelInLayout(Layout layout, Type channel) {
  return av_get_channel_layout_channel_index((uint64_t) layout,
      (uint64_t) channel);
}

AudioChannel::Type
AudioChannel::getChannelFromLayoutAtIndex(Layout layout, int32_t index) {
  return (Type) av_channel_layout_extract_channel((uint64_t) layout, index);
}

const char *
AudioChannel::getChannelName(Type channel) {
  return av_get_channel_name(channel);
}

const char *
AudioChannel::getChannelDescription(Type channel) {
  return av_get_channel_description(channel);
}

const char*
AudioChannel::getLayoutName(Layout layout) {
  const char* name = 0;
  int i = 0;
  int retval = 0;
  uint64_t l = 0;
  do {
    retval = av_get_standard_channel_layout(i, &l, &name);
    if (retval < 0) {
      name = 0;
      break;
    }
    if (l == (uint64_t) layout) break;
    // and increment the index.
    ++i;
  } while (1);
  return name;
}

const char*
AudioFormat::getName(Type format) {
  return av_get_sample_fmt_name((enum AVSampleFormat) format);
}

AudioFormat::Type
AudioFormat::getFormat(const char* name) {
  return (Type) av_get_sample_fmt(name);
}

AudioFormat::Type
AudioFormat::getAlternateSampleFormat(Type sample_fmt, bool planar) {
  return (Type) av_get_alt_sample_fmt((enum AVSampleFormat) sample_fmt,
      (int) planar);
}

AudioFormat::Type
AudioFormat::getPackedSampleFormat(Type sample_fmt) {
  return (Type) av_get_packed_sample_fmt((enum AVSampleFormat) sample_fmt);
}

AudioFormat::Type
AudioFormat::getPlanarSampleFormat(Type sample_fmt) {
  return (Type) av_get_planar_sample_fmt((enum AVSampleFormat) sample_fmt);
}

int32_t
AudioFormat::getBytesPerSample(Type sample_fmt) {
  return av_get_bytes_per_sample((enum AVSampleFormat) sample_fmt);
}

bool
AudioFormat::isPlanar(Type sample_fmt) {
  return av_sample_fmt_is_planar((enum AVSampleFormat) sample_fmt);
}

int32_t
AudioFormat::getBufferSizeNeeded(int32_t numSamples, int32_t numChannels,
    Type format) {
  return av_samples_get_buffer_size(0, numChannels, numSamples,
      (enum AVSampleFormat) format, 0);
}

int32_t
AudioFormat::getDataPlaneSizeNeeded(int32_t numSamples, int32_t numChannels,
    Type format) {
  int32_t linesize = 0;
  (void) av_samples_get_buffer_size(&linesize, numChannels, numSamples,
      (enum AVSampleFormat) format, 0);
  return linesize;
}
} /* namespace video */
} /* namespace humble */
} /* namespace io */
