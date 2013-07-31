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
 * MediaAudioResamplerTest.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: aclarke
 */

#include "MediaAudioResamplerTest.h"
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/MediaAudioResampler.h>
#include <io/humble/video/MediaAudio.h>


using namespace io::humble::ferry;
using namespace io::humble::video;

MediaAudioResamplerTest::MediaAudioResamplerTest() {
}

MediaAudioResamplerTest::~MediaAudioResamplerTest() {
}

void
MediaAudioResamplerTest::testCreation()
{
  RefPointer<MediaAudioResampler> resampler;
  AudioChannel::Layout outLayout = AudioChannel::CH_LAYOUT_STEREO;
  AudioChannel::Layout inLayout = AudioChannel::CH_LAYOUT_5POINT1;
  int32_t outSampleRate = 22050;
  int32_t inSampleRate = 44100;
  AudioFormat::Type outFormat = AudioFormat::SAMPLE_FMT_S16;
  AudioFormat::Type inFormat = AudioFormat::SAMPLE_FMT_DBLP;

  {
    LoggerStack stack;
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

    TS_ASSERT_THROWS(
        MediaAudioResampler::make(AudioChannel::CH_LAYOUT_UNKNOWN, outSampleRate, outFormat,
            inLayout, inSampleRate, inFormat),
            HumbleInvalidArgument);
    TS_ASSERT_THROWS(
        MediaAudioResampler::make(outLayout, 0, outFormat,
            inLayout, inSampleRate, inFormat),
            HumbleInvalidArgument);
    TS_ASSERT_THROWS(
        MediaAudioResampler::make(outLayout, outSampleRate, AudioFormat::SAMPLE_FMT_NONE,
            inLayout, inSampleRate, inFormat),
            HumbleInvalidArgument);
    TS_ASSERT_THROWS(
        MediaAudioResampler::make(outLayout, outSampleRate, outFormat,
            AudioChannel::CH_LAYOUT_UNKNOWN, inSampleRate, inFormat),
            HumbleInvalidArgument);
    TS_ASSERT_THROWS(
        MediaAudioResampler::make(outLayout, outSampleRate, outFormat,
            inLayout, 0, inFormat),
            HumbleInvalidArgument);
    TS_ASSERT_THROWS(
        MediaAudioResampler::make(outLayout, outSampleRate, outFormat,
            inLayout, inSampleRate, AudioFormat::SAMPLE_FMT_NONE),
            HumbleInvalidArgument);
  }
  resampler = MediaAudioResampler::make(outLayout, outSampleRate, outFormat,
      inLayout, inSampleRate, inFormat);
  TS_ASSERT(resampler);
}

void
MediaAudioResamplerTest::testResampleErrors()
{
  RefPointer<MediaAudioResampler> resampler;
  AudioChannel::Layout outLayout = AudioChannel::CH_LAYOUT_STEREO;
  AudioChannel::Layout inLayout = AudioChannel::CH_LAYOUT_5POINT1;
  int32_t outSampleRate = 22050;
  int32_t inSampleRate = 44100;
  AudioFormat::Type outFormat = AudioFormat::SAMPLE_FMT_S16;
  AudioFormat::Type inFormat = AudioFormat::SAMPLE_FMT_DBLP;

  resampler = MediaAudioResampler::make(outLayout, outSampleRate, outFormat,
      inLayout, inSampleRate, inFormat);
  TS_ASSERT(resampler);
  resampler->open();

  // now, test that we get the right errors when samples do not match.
  RefPointer<MediaAudio> in;
  RefPointer<MediaAudio> out;
  {
    LoggerStack stack;
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

    in = MediaAudio::make(1000, inSampleRate*2, AudioChannel::getNumChannelsInLayout(inLayout), inLayout, inFormat);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout), outLayout, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout)*2, AudioChannel::CH_LAYOUT_UNKNOWN, inFormat);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout), outLayout, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout), AudioChannel::CH_LAYOUT_UNKNOWN, inFormat);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout), outLayout, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout), inLayout, AudioFormat::SAMPLE_FMT_S32);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout), outLayout, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout), inLayout, inFormat);
    out = MediaAudio::make(1000, outSampleRate*2, AudioChannel::getNumChannelsInLayout(outLayout), outLayout, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout), inLayout, inFormat);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout)*2, AudioChannel::CH_LAYOUT_UNKNOWN, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout), inLayout, inFormat);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout), AudioChannel::CH_LAYOUT_UNKNOWN, outFormat);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);

    in = MediaAudio::make(1000, inSampleRate, AudioChannel::getNumChannelsInLayout(inLayout), inLayout, inFormat);
    out = MediaAudio::make(1000, outSampleRate, AudioChannel::getNumChannelsInLayout(outLayout), outLayout, AudioFormat::SAMPLE_FMT_S32);
    TS_ASSERT_THROWS(resampler->resample(out.value(), in.value()), HumbleInvalidArgument);
  }

}
