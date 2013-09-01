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
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/MediaAudioResampler.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/Demuxer.h>
#include <io/humble/video/Decoder.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

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
  RefPointer<Rational> tb = resampler->getTimeBase();
  TS_ASSERT_EQUALS(1, tb->getNumerator());
  TS_ASSERT_EQUALS(inSampleRate, tb->getDenominator());

  // now make sure LCM works
  resampler = MediaAudioResampler::make(outLayout, 48000, outFormat,
      inLayout, 22050, inFormat);
  TS_ASSERT(resampler);
  tb = resampler->getTimeBase();
  TS_ASSERT_EQUALS(1, tb->getNumerator());
  TS_ASSERT_EQUALS(7056000, tb->getDenominator());

}

static void writeAudioHelper(FILE* output, MediaAudio* audio) {
  RefPointer<Buffer> buf;

  // we'll just write out the first channel.
  buf = audio->getData(0);
  // let's figure out # of bytes to make valgrind happy.
  size_t size = audio->getDataPlaneSize(0);

  const void* data = buf->getBytes(0, size);
  fwrite(data, 1, size, output);
}
void
MediaAudioResamplerTest::writeAudio(FILE* output, MediaAudio* audio,
    MediaAudioResampler* resampler,
    MediaAudio* rAudio)
{
  if (audio->isComplete()) {
    // resampling correctly is tricky; need to make sure that we flush everything.
    int resampled = 0;
    do {
      RefPointer<Rational> inputTb = audio ? audio->getTimeBase() : Rational::make(1, 1);
      resampled = resampler->resample(rAudio, audio);
      RefPointer<Rational> outputTb = rAudio->getTimeBase();
      VS_LOG_TRACE("Input ts: %lld (%d/%d); Output ts: %lld (%d/%d); Rebased: %lld (%d/%d)",
          audio ? audio->getTimeStamp() : Global::NO_PTS,
              inputTb->getNumerator(), inputTb->getDenominator(),
              rAudio->getTimeStamp(),
              outputTb->getNumerator(), outputTb->getDenominator(),
              Rational::rescale(rAudio->getTimeStamp(), 1, resampler->getOutputSampleRate(),
                  outputTb->getNumerator(), outputTb->getDenominator(), Rational::ROUND_DOWN),
              1, resampler->getOutputSampleRate());
      if (rAudio->isComplete()) {
        // we successfully resampled some audio; write it.
        writeAudioHelper(output, rAudio);
      }
      if (resampled > 0)
        // now let's null out audio so we try to flush the resampler.
        audio = 0;
    } while (resampled > 0);
  }
}

void
MediaAudioResamplerTest::testResample() {
  TestData::Fixture* fixture=mFixtures.getFixture("bigbuckbunny_h264_aac_5.1.mp4");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Demuxer> source = Demuxer::make();

  source->open(filepath, 0, false, true, 0, 0);

  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  int32_t streamToDecode = 1;
  RefPointer<DemuxerStream> stream = source->getStream(streamToDecode);
  TS_ASSERT(stream);
  RefPointer<Decoder> decoder = stream->getDecoder();
  TS_ASSERT(decoder);
  RefPointer<Codec> codec = decoder->getCodec();
  TS_ASSERT(codec);
  TS_ASSERT_EQUALS(Codec::CODEC_ID_AAC, codec->getID());

  decoder->open(0, 0);

  TS_ASSERT_EQUALS(AudioFormat::SAMPLE_FMT_FLTP, decoder->getSampleFormat());

  VS_LOG_TRACE("Channel Layout: %s; Value: %ld", AudioChannel::getLayoutName(decoder->getChannelLayout()),
      decoder->getChannelLayout());
  TS_ASSERT_EQUALS((int32_t)AudioChannel::CH_LAYOUT_5POINT1_BACK, (int32_t)decoder->getChannelLayout());

  // now, let's start a decoding loop.
  RefPointer<MediaPacket> packet = MediaPacket::make();

  RefPointer<MediaAudio> audio = MediaAudio::make(
      decoder->getFrameSize(),
      decoder->getSampleRate(),
      decoder->getChannels(),
      decoder->getChannelLayout(),
      decoder->getSampleFormat());

  int32_t rSampleRate = 22050; // this will result in an uneven conversion, which tests flushing well.
  AudioChannel::Layout rLayout = AudioChannel::CH_LAYOUT_STEREO;
  int32_t rChannels = AudioChannel::getNumChannelsInLayout(rLayout);
  AudioFormat::Type rFormat = AudioFormat::SAMPLE_FMT_S16;
  RefPointer<MediaAudio> rAudio = MediaAudio::make(
      decoder->getFrameSize(),
      rSampleRate,
      rChannels,
      rLayout,
      rFormat
  );

  // now let's make a resampler
  RefPointer<MediaAudioResampler> resampler =
      MediaAudioResampler::make(
          rAudio->getChannelLayout(),
          rAudio->getSampleRate(),
          rAudio->getFormat(),
          audio->getChannelLayout(),
          audio->getSampleRate(),
          audio->getFormat()
      );
  resampler->open();

  FILE* output = fopen("MediaAudioResamplerTest_testResample.au", "wb");
  TS_ASSERT(output);

  int32_t numSamples = 0;
  while(source->read(packet.value()) >= 0) {
    // got a packet; now we try to decode it.
    if (packet->getStreamIndex() == streamToDecode &&
        packet->isComplete()) {
      int32_t bytesRead = 0;
      int32_t byteOffset=0;
      do {
        bytesRead = decoder->decodeAudio(audio.value(), packet.value(), byteOffset);
        if (audio->isComplete()) {
          writeAudio(output, audio.value(), resampler.value(), rAudio.value());
          numSamples += audio->getNumSamples();
        }
        byteOffset += bytesRead;
      } while(byteOffset < packet->getSize());
    }
    if (getenv("VS_TEST_MEMCHECK") && numSamples > 10240)
      // short circuit if running under valgrind.
      break;
  }
  do {
    decoder->decodeAudio(audio.value(), 0, 0);
    if (audio->isComplete()) {
      writeAudio(output, audio.value(), resampler.value(), rAudio.value());
      numSamples += audio->getNumSamples();
    }
  } while (audio->isComplete());

  source->close();
  fclose(output);
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

void
MediaAudioResamplerTest::testFlushResampler() {
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

  RefPointer<MediaAudio> out;
  out = MediaAudio::make(1000, outSampleRate,
      AudioChannel::getNumChannelsInLayout(outLayout),
      outLayout, outFormat);

  // make sure we can pass a null input.
  resampler->resample(out.value(), 0);
  TS_ASSERT(!out->isComplete());
}
