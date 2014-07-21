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
#include <io/humble/video/VideoExceptions.h>
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

  // find the LCM of the input and output sample rates
  int64_t gcd = av_gcd(inSampleRate, outSampleRate);
  int64_t lcm = inSampleRate / gcd * outSampleRate;
  if (lcm > LONG_MAX) {
    VS_LOG_INFO("LCM of input and output sample rates is greater than can be fit in a 32-bit value");
  }
  retval->mTimeBase = Rational::make(1, (int32_t)lcm);

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
    FfmpegException::check(retval, "Could not open audio resampler: ");
  }
  mState = STATE_OPENED;
}

void
MediaAudioResampler::setTimeBase(Rational* tb) {
  if (!tb) {
    VS_THROW(HumbleInvalidArgument("time base must be non null"));
  }
  mTimeBase.reset(tb, true);
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


  // first convert PTS to sample number
  int64_t inputTs = Global::NO_PTS;
  if (in) {
    inputTs = in->getTimeStamp();
    RefPointer<Rational> inBase = in->getTimeBase();
    /// convert to 1/samplerate
    inputTs = Rational::rescale(inputTs, 1,
        getInputSampleRate()*getOutputSampleRate(),
        inBase ? inBase->getNumerator() : 1,
        inBase ? inBase->getDenominator() : getInputSampleRate(),
        Rational::ROUND_DOWN);
  }
  // now convert the new PTS back to the right timebase
  outFrame->pts = Rational::rescale(
      getNextPts(inputTs == Global::NO_PTS ? LLONG_MIN : inputTs),
      mTimeBase->getNumerator(), mTimeBase->getDenominator(),
      1, getInputSampleRate() * getOutputSampleRate(),
      Rational::ROUND_DOWN);
  // and set our time base.
  out->setTimeBase(mTimeBase.value());

  // now, we know the audio is hidden in extended_data
  int retval = swr_convert(mCtx,
      outFrame->extended_data,
      out->getMaxNumSamples(),
      inFrame? (const uint8_t**)inFrame->extended_data : 0,
      inFrame ? inFrame->nb_samples : 0);
  if (retval < 0) {
    FfmpegException::check(retval, "Could not convert audio ");
  }
  outFrame->nb_samples = retval;
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

int32_t
MediaAudioResampler::getNumResampledSamples(int32_t numSamples) {
  return av_rescale_rnd(getOutputSampleRate(), numSamples, getInputSampleRate(), AV_ROUND_UP);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
