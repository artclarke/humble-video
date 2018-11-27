/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
/*
 * Audio.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: aclarke
 */

#include "MediaAudio.h"
#include "AVBufferSupport.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/RefPointer.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.MediaAudio);

namespace io {
namespace humble {
namespace video {

MediaAudio::MediaAudio() {
  mFrame = av_frame_alloc();
  if (!mFrame) throw std::bad_alloc();
  mFrame->opaque = this;
  mComplete = false;
}

MediaAudio::~MediaAudio() {
  av_frame_free(&mFrame);
}

MediaAudio*
MediaAudio::make(int32_t numSamples, int32_t sampleRate, int32_t channels,
    AudioChannel::Layout layout, AudioFormat::Type format) {
  Global::init();
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
  MediaAudio* retval = make(buffer.value(), numSamples, sampleRate, channels, layout,
      format);
  buffer->setJavaAllocator(retval->getJavaAllocator());
  return retval;
}

MediaAudio*
MediaAudio::make(io::humble::ferry::Buffer* buffer, int32_t numSamples,
    int32_t sampleRate,
    int32_t channels, AudioChannel::Layout layout, AudioFormat::Type format) {
  Global::init();

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
  RefPointer<MediaAudio> retval = make();
  RefPointer<Rational> tb = Rational::make(1,sampleRate); // a sensible default.
  retval->setTimeBase(tb.value());
  AVFrame* frame = retval->mFrame;
  frame->sample_rate = sampleRate;
  frame->channels = channels;
  frame->channel_layout = layout;
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
MediaAudio::copy(AVFrame* src, bool complete) {
  if (!src)
    VS_THROW(HumbleInvalidArgument("no src"));
  // release any memory we have
  RefPointer<Rational> timeBase = Rational::make(1, src->sample_rate); // a default
  setTimeBase(timeBase.value());
  av_frame_unref(mFrame);
  // and copy any data in.
  av_frame_ref(mFrame, src);
  mComplete=complete;
}

MediaAudio*
MediaAudio::make(MediaAudio* src, bool copy) {
  Global::init();

  RefPointer<MediaAudio> retval;

  if (!src) VS_THROW(HumbleInvalidArgument("no src object to copy from"));

  if (copy) {
    // first create a new mediaaudio object to copy into
    retval = make(src->getMaxNumSamples(),
        src->getSampleRate(),
        src->getChannels(),
        src->getChannelLayout(),
        src->getFormat());

    retval->setNumSamples(src->getNumSamples());
    retval->setTimeStamp(src->getTimeStamp());
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
  }
  // copy the items not embedded in the frame
  retval->setComplete(src->isComplete());
  RefPointer<Rational> timeBase = src->getTimeBase();
  retval->setTimeBase(timeBase.value());

  return retval.get();
}

io::humble::ferry::Buffer*
MediaAudio::getData(int32_t plane) {
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
MediaAudio::getDataPlaneSize(int32_t plane) {
  int32_t n = getNumDataPlanes();
  if (plane < 0 || plane >= n)
    VS_THROW(HumbleInvalidArgument("plane is out of range"));
  if (isComplete())
    return getNumSamples()*AudioFormat::getBytesPerSample(getFormat())*(isPlanar()? 1 : getChannels());
  else
    // oddly for audio, each plane in multiplane audio must
    // be the same as linesize[0] and sometimes ffmpeg
    // doesn't copy all that data in.
    return mFrame->linesize[0];
}

int32_t
MediaAudio::getNumDataPlanes() {
  if (isPlanar()) return mFrame->channels;
  else return 1;
}

int32_t
MediaAudio::getMaxNumSamples() {
  int32_t bytesPerSample = AudioFormat::getBytesPerSample(getFormat());
  int32_t size = bytesPerSample ? mFrame->linesize[0] / bytesPerSample : 0;
  return size;
}

int32_t
MediaAudio::getBytesPerSample() {
  return AudioFormat::getBytesPerSample((AudioFormat::Type) mFrame->format);
}

void
MediaAudio::setComplete(bool complete) {
  mComplete = complete;
}

int32_t
MediaAudio::getSampleRate() {
  return mFrame->sample_rate;
}

int32_t
MediaAudio::getChannels() {
  return mFrame->channels;
}

AudioFormat::Type
MediaAudio::getFormat() {
  return (AudioFormat::Type) mFrame->format;
}

int32_t
MediaAudio::getNumSamples() {
  return mFrame->nb_samples;
}
void
MediaAudio::setNumSamples(int32_t numSamples) {
  if (numSamples <= 0 || numSamples > getMaxNumSamples()) {
    VS_THROW(HumbleInvalidArgument("invalid number of samples to put in this MediaAudio object"));
  }
  mFrame->nb_samples = numSamples;
}


bool
MediaAudio::isComplete() {
  return mComplete && mFrame->nb_samples > 0;
}

bool
MediaAudio::isKey() {
  return mFrame->key_frame;
}

bool
MediaAudio::isPlanar() {
  return AudioFormat::isPlanar((AudioFormat::Type) mFrame->format);
}

AudioChannel::Layout
MediaAudio::getChannelLayout() {
  return (AudioChannel::Layout) mFrame->channel_layout;
}

AVFrame*
MediaAudio::getCtx() {
  return mFrame;
}

void
MediaAudio::setBufferType(AudioFormat::Type format,
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

int64_t
MediaAudio::logMetadata(char* buffer, size_t len)
{
  RefPointer<Rational> tb = getTimeBase();
  char pts[48];
  if (getPts() == Global::NO_PTS) {
    snprintf(pts, sizeof(pts), "NONE");
  } else
    snprintf(pts, sizeof(pts), "%" PRId64, getPts());

  return snprintf(buffer, len,
                  "MediaAudio@%p:[pts:%s;tb:%" PRId64 "/%" PRId64 ";sr:%" PRId64 ";ch:%" PRId64 ";fo:%" PRId64 ";co:%s;sam:%" PRId64 "]",
                  this,
                  pts,
                  (int64_t)(tb?tb->getNumerator():0),
                  (int64_t)(tb?tb->getDenominator():0),
                  (int64_t)getSampleRate(),
                  (int64_t)getChannels(),
                  (int64_t)getFormat(),
                  isComplete()?"true":"false",
                  (int64_t)getNumSamples()
                  );
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
