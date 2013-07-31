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
 * MediaAudioResampler.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/Error.h>
#include <io/humble/video/MediaAudioImpl.h>
#include "MediaAudioResampler.h"

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

VS_LOG_SETUP(VS_CPP_PACKAGE);

MediaAudioResampler::MediaAudioResampler() {
  mCtx = swr_alloc();
  mState = STATE_INITED;
  if (!mCtx) {
    VS_THROW(HumbleBadAlloc());
  }
}
MediaAudioResampler::~MediaAudioResampler() {
  swr_free(&mCtx);
}

MediaAudioResampler*
MediaAudioResampler::make(AudioChannel::Layout outLayout, int32_t outSampleRate,
    AudioFormat::Type outFormat, AudioChannel::Layout inLayout,
    int32_t inSampleRate, AudioFormat::Type inFormat) {
  RefPointer<MediaAudioResampler> retval;

  if (outLayout == AudioChannel::CH_LAYOUT_UNKNOWN)
    VS_THROW(HumbleInvalidArgument("outLayout must not be CH_LAYOUT_UNKNOWN"));
  if (outSampleRate <= 0)
    VS_THROW(HumbleInvalidArgument("outSampleRate must be > 0"));
  if (outFormat == AudioFormat::SAMPLE_FMT_NONE)
    VS_THROW(HumbleInvalidArgument("outFormat must not be SAMPLE_FMT_NONE"));
  if (inLayout == AudioChannel::CH_LAYOUT_UNKNOWN)
    VS_THROW(HumbleInvalidArgument("inLayout must not be CH_LAYOUT_UNKNOWN"));
  if (inSampleRate <= 0)
    VS_THROW(HumbleInvalidArgument("inSampleRate must be > 0"));
  if (inFormat == AudioFormat::SAMPLE_FMT_NONE)
    VS_THROW(HumbleInvalidArgument("inFormat must not be SAMPLE_FMT_NONE"));

  retval.reset(new MediaAudioResampler(), true);

  av_opt_set_int(retval->mCtx, "ocl", outLayout,   0);
  av_opt_set_int(retval->mCtx, "osf", outFormat,  0);
  av_opt_set_int(retval->mCtx, "osr", outSampleRate, 0);
  av_opt_set_int(retval->mCtx, "icl", inLayout,    0);
  av_opt_set_int(retval->mCtx, "isf", inFormat,   0);
  av_opt_set_int(retval->mCtx, "isr", inSampleRate,  0);
  av_opt_set_int(retval->mCtx, "tsf", AV_SAMPLE_FMT_NONE,   0);
  av_opt_set_int(retval->mCtx, "ich", av_get_channel_layout_nb_channels(inLayout), 0);
  av_opt_set_int(retval->mCtx, "och", av_get_channel_layout_nb_channels(outLayout), 0);
  av_opt_set_int(retval->mCtx, "uch", 0, 0);


  // now we set all the values
  return retval.get();
}

AudioChannel::Layout
MediaAudioResampler::getOutputLayout() {
  int64_t val;
  if(av_opt_get_int(mCtx, "ocl", 0, &val) < 0)
    return AudioChannel::CH_LAYOUT_UNKNOWN;
  else
    return (AudioChannel::Layout)val;
}

AudioChannel::Layout
MediaAudioResampler::getInputLayout() {
  int64_t val;
  if(av_opt_get_int(mCtx, "icl", 0, &val) < 0)
    return AudioChannel::CH_LAYOUT_UNKNOWN;
  else
    return (AudioChannel::Layout)val;
}

int32_t
MediaAudioResampler::getOutputSampleRate() {
  int64_t val;
  if(av_opt_get_int(mCtx, "osr", 0, &val) < 0)
    return -1;
  else
    return (int32_t)val;
}

int32_t
MediaAudioResampler::getInputSampleRate() {
  int64_t val;
  if(av_opt_get_int(mCtx, "isr", 0, &val) < 0)
    return -1;
  else
    return (int32_t)val;
}

AudioFormat::Type
MediaAudioResampler::getOutputFormat() {
  int64_t val;
  if(av_opt_get_int(mCtx, "osf", 0, &val) < 0)
    return AudioFormat::SAMPLE_FMT_NONE;
  else
    return (AudioFormat::Type)val;
}

AudioFormat::Type
MediaAudioResampler::getInputFormat() {
  int64_t val;
  if(av_opt_get_int(mCtx, "isf", 0, &val) < 0)
    return AudioFormat::SAMPLE_FMT_NONE;
  else
    return (AudioFormat::Type)val;
}


int32_t
MediaAudioResampler::getInputChannels() {
  int64_t val;
  if(av_opt_get_int(mCtx, "ich", 0, &val) < 0)
    return -1;
  else
    return (int32_t)val;
}

int32_t
MediaAudioResampler::getOutputChannels() {
  int64_t val;
  if(av_opt_get_int(mCtx, "och", 0, &val) < 0)
    return -1;
  else
    return (int32_t)val;
}


void
MediaAudioResampler::open() {
  int retval = swr_init(mCtx);
  if (retval < 0) {
    mState = STATE_ERROR;
    RefPointer<Error> error = Error::make(retval);
    VS_THROW(HumbleRuntimeError::make("Could not open audio resampler: %s", error ? error->getDescription() : ""));
  }
  mState = STATE_OPENED;
}

int32_t
MediaAudioResampler::resample(MediaAudio* aOut, MediaAudio* aIn) {
  MediaAudioImpl* out = dynamic_cast<MediaAudioImpl*>(aOut);
  MediaAudioImpl* in = dynamic_cast<MediaAudioImpl*>(aIn);

  if (mState != STATE_OPENED)
    VS_THROW(HumbleRuntimeError("Must call open() on resampler before using"));
  if (in) {
    if (in->getFormat() != getInputFormat())
      VS_THROW(HumbleInvalidArgument("input audio format does not match what resampler expected"));
    if (in->getSampleRate() != getInputSampleRate())
      VS_THROW(HumbleInvalidArgument("input audio sample rate does not match what resampler expected"));
    if (in->getChannelLayout() != getInputLayout())
      VS_THROW(HumbleInvalidArgument("input audio channel layout does not match what resampler expected"));
  }
  if (!out)
    VS_THROW(HumbleInvalidArgument("output must be specified"));

  if (out->getFormat() != getOutputFormat())
    VS_THROW(HumbleInvalidArgument("output audio format does not match what resampler expected"));
  if (out->getSampleRate() != getOutputSampleRate())
    VS_THROW(HumbleInvalidArgument("output audio sample rate does not match what resampler expected"));
  if (out->getChannelLayout() != getOutputLayout())
    VS_THROW(HumbleInvalidArgument("output audio channel layout does not match what resampler expected"));

  out->setComplete(false);

  AVFrame* outFrame = out->getCtx();
  AVFrame* inFrame = in ? in->getCtx() : 0;

  // now, we know the audio is hidden in extended_data
  int retval = swr_convert(mCtx,
      outFrame ? outFrame->extended_data : 0,
      out ? out->getMaxNumSamples() : 0 ,
      inFrame? (const uint8_t**)inFrame->extended_data : 0,
      inFrame ? inFrame->nb_samples : 0);
  if (retval < 0) {
    RefPointer<Error> error = Error::make(retval);
    VS_THROW(HumbleRuntimeError::make("Could not convert audio: %s", error ? error->getDescription() : ""));
  }
  outFrame->nb_samples = retval;
  // wrong; need to figure out PTS stuff.
  outFrame->pts = getNextPts(inFrame ? inFrame->pts : Global::NO_PTS);
  out->setComplete(retval > 0);
  return retval;

}

int64_t
MediaAudioResampler::getNextPts(int64_t pts) {
  return swr_next_pts(mCtx, pts);
}

void
MediaAudioResampler::setCompensation(int32_t sample_delta,
    int32_t compensation_distance) {
  swr_set_compensation(mCtx, sample_delta, compensation_distance);
}

int32_t
MediaAudioResampler::dropOutput(int32_t count) {
  return swr_drop_output(mCtx, count);
}

int32_t
MediaAudioResampler::injectSilence(int32_t count) {
  return swr_inject_silence(mCtx, count);
}

int64_t
MediaAudioResampler::getDelay(int64_t base) {
  return swr_get_delay(mCtx, base);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
