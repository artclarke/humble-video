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
 * EncoderTest.cpp
 *
 *  Created on: Sep 6, 2013
 *      Author: aclarke
 */
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/FilterPictureSink.h>
#include <io/humble/video/FilterAudioSink.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/MediaAudio.h>
#include "EncoderTest.h"

using namespace io::humble::ferry;
VS_LOG_SETUP(VS_CPP_PACKAGE);

EncoderTest::EncoderTest() {
}

EncoderTest::~EncoderTest() {
}

void
EncoderTest::testCreation() {
  RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_H264);
  RefPointer<Encoder> encoder = Encoder::make(codec.value());
  TS_ASSERT(encoder);
}

void
EncoderTest::testEncodeVideo() {
  const bool isMemCheck = getenv("VS_TEST_MEMCHECK") ? true : false;
  const int32_t maxPics = isMemCheck ? 10 : 500;
  int32_t width=176;
  int32_t height=144;

  RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_H264);
  RefPointer<Encoder> encoder = Encoder::make(codec.value());
  RefPointer<MediaPicture> picture = MediaPicture::make(width*2,height*2,
      PixelFormat::PIX_FMT_YUV420P);

  // set the encoder properties we need
  encoder->setWidth(picture->getWidth());
  encoder->setHeight(picture->getHeight());
  encoder->setPixelFormat(picture->getFormat());
  encoder->setProperty("b", (int64_t)400000); // bitrate
  encoder->setProperty("g", (int64_t) 10); // gop
  encoder->setProperty("bf", (int64_t)1); // max b frames

  RefPointer<Rational> tb = Rational::make(1,25);
  encoder->setTimeBase(tb.value());

  // mandlebrot, that is then negated, horizontally flipped, and edge detected, before
  // final outputting to a new picture with each version in one of 4 quadrants.
  char graphCommand[1024];
  snprintf(graphCommand,sizeof(graphCommand),"mandelbrot=s=%dx%d[mb];"
      "[mb]split=4[0][1][2][3];"
      "[0]pad=iw*2:ih*2[a];"
      "[1]negate[b];"
      "[2]hflip[c];"
      "[3]edgedetect[d];"
      "[a][b]overlay=w[x];"
      "[x][c]overlay=0:h[y];"
      "[y][d]overlay=w:h[out]", width, height);

  RefPointer<FilterGraph> graph = FilterGraph::make();
  RefPointer<FilterPictureSink> fsink = graph->addPictureSink("out", picture->getFormat());
  graph->open(graphCommand);

  // let's set a frame time base of 1/30
  RefPointer<Rational> pictureTb = Rational::make(1,30);

  // create an output muxer
  RefPointer<Muxer> muxer = Muxer::make("EncoderTest_encodeVideo.mov", 0, 0);
  RefPointer<MuxerFormat> format = muxer->getFormat();

  // if the container will require a global header, then the encoder needs to set this.
  if (format->getFlag(MuxerFormat::GLOBAL_HEADER))
    encoder->setFlag(Encoder::FLAG_GLOBAL_HEADER, true);

  // open the encoder
  encoder->open(0, 0);

  // add a stream for the encoded packets
  {
    RefPointer<MuxerStream> stream = muxer->addNewStream(encoder.value());
  }

  // and open the muxer
  muxer->open(0, 0);

  // now we're (in theory) ready to start writing data.
  int32_t numPics = 0;
  RefPointer<MediaPacket> packet;

  while(fsink->getPicture(picture.value()) >= 0 && numPics < maxPics) {
    picture->setTimeBase(pictureTb.value());
    picture->setTimeStamp(numPics);

    // let's encode
    packet = MediaPacket::make();
    encoder->encodeVideo(packet.value(), picture.value());
    if (packet->isComplete()) {
      muxer->write(packet.value(), false);
    }
    ++numPics;
  }
  // now flush the encoder
  do {
    packet = MediaPacket::make();
    encoder->encodeVideo(packet.value(), 0);
    if (packet->isComplete()) {
      muxer->write(packet.value(), false);
    }
  } while (packet->isComplete());

  muxer->close();

}

void
EncoderTest::testEncodeAudio() {
  const bool isMemCheck = getenv("VS_TEST_MEMCHECK") ? true : false;
  const int32_t sampleRate = 44100;
  const int32_t maxSamples = isMemCheck ? sampleRate*0.5 : sampleRate*10;
  const int32_t numSamples = 1024;
  const AudioChannel::Layout channelLayout = AudioChannel::CH_LAYOUT_STEREO;
  const int32_t channels = AudioChannel::getNumChannelsInLayout(channelLayout);
  const AudioFormat::Type audioFormat = AudioFormat::SAMPLE_FMT_S16P;
  RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_MP3);
  RefPointer<Encoder> encoder = Encoder::make(codec.value());

  RefPointer<FilterGraph> graph = FilterGraph::make();

  RefPointer<MediaAudio> audio = MediaAudio::make(numSamples, sampleRate, channels, channelLayout,
      audioFormat);

  // set the encoder properties we need
  encoder->setSampleRate(audio->getSampleRate());
  encoder->setSampleFormat(audio->getFormat());
  encoder->setChannelLayout(audio->getChannelLayout());
  encoder->setChannels(audio->getChannels());
  encoder->setProperty("b", (int64_t)64000); // bitrate
  RefPointer<Rational> tb = Rational::make(1,25);
  encoder->setTimeBase(tb.value());

  // open the encoder
  encoder->open(0, 0);

  RefPointer<FilterAudioSink> fsink = graph->addAudioSink("out", audio->getSampleRate(), audio->getChannelLayout(), audio->getFormat());

  // Generate a 220 Hz sine wave with a 880 Hz beep each second, for 60 seconds.
  //  graph->open("sine=frequency=220:beep_factor=4:duration=60[out]");
  // Generate an amplitude modulated signal
  graph->open("aevalsrc=sin(10*2*PI*t)*sin(880*2*PI*t)[out]");

  // create an output muxer
  RefPointer<Muxer> muxer = Muxer::make("EncoderTest_encodeAudio.mp3", 0, 0);

  // add a stream for the encoded packets
  {
    RefPointer<MuxerStream> stream = muxer->addNewStream(encoder.value());
  }

  // and open the muxer
  muxer->open(0, 0);

  // now we're (in theory) ready to start writing data.
  int32_t numFrames = 0;
  RefPointer<MediaPacket> packet;

  while(fsink->getAudio(audio.value()) >= 0 && numFrames*audio->getNumSamples() < maxSamples) {
    audio->setTimeStamp(numFrames*audio->getNumSamples());

    // let's encode
    packet = MediaPacket::make();
    encoder->encodeAudio(packet.value(), audio.value());
    if (packet->isComplete()) {
      muxer->write(packet.value(), false);
    }
    ++numFrames;
  }
  // now flush the encoder
  do {
    packet = MediaPacket::make();
    encoder->encodeAudio(packet.value(), 0);
    if (packet->isComplete()) {
      muxer->write(packet.value(), false);
    }
  } while (packet->isComplete());

  muxer->close();
}

/**
 * Test that memory is not leaked/corrupted during error paths.
 */
void
EncoderTest::testEncodeInvalidParameters()
{
  // Sub-test 1
  {
    int32_t width=176;
    int32_t height=360; // invalid dimensions for H263 codec
    RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_H263);
    RefPointer<Encoder> encoder = Encoder::make(codec.value());

    RefPointer<MediaPicture> picture = MediaPicture::make(width*2,height*2,
        PixelFormat::PIX_FMT_YUV420P);

    // set the encoder properties we need
    encoder->setWidth(picture->getWidth());
    encoder->setHeight(picture->getHeight());
    encoder->setPixelFormat(picture->getFormat());
    encoder->setProperty("b", (int64_t)400000); // bitrate
    encoder->setProperty("g", (int64_t) 10); // gop

    RefPointer<Rational> tb = Rational::make(1,25);
    encoder->setTimeBase(tb.value());

    // open the encoder
    try {
      // Temporarily turn down logging
      LoggerStack stack;
      stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

      encoder->open(0, 0);
      TS_FAIL("should never get here");
    } catch (std::exception & e) {
      // ignore exception
    }
  }

  // Sub-test 2
  {
    int32_t width=176;
    int32_t height=144;

    RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_H264);
    RefPointer<Encoder> encoder = Encoder::make(codec.value());
    RefPointer<MediaPicture> picture = MediaPicture::make(width*2,height*2,
        PixelFormat::PIX_FMT_YUV420P);

    // set the encoder properties we need
    encoder->setWidth(picture->getWidth());
    encoder->setHeight(picture->getHeight());
    encoder->setPixelFormat(picture->getFormat());
    encoder->setProperty("b", (int64_t)400000); // bitrate
    encoder->setProperty("g", (int64_t) 10); // gop
    encoder->setProperty("bf", (int64_t)1); // max b frames

    RefPointer<Rational> tb = Rational::make(1,25);
    encoder->setTimeBase(tb.value());

    // Do not open the encoder

    // create an output muxer
    RefPointer<Muxer> muxer = Muxer::make("EncoderTest_encodeVideo.mov", 0, 0);

    // add a stream for the encoded packets
    try {
      // Temporarily turn down logging
      LoggerStack stack;
      stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

      RefPointer<MuxerStream> stream = muxer->addNewStream(encoder.value());
      TS_FAIL("should never get here");
    } catch (std::exception & e) {
      // success
    }
  }
}

void
EncoderTest::encodeAndMux(
    Media* media,
    Muxer* muxer,
    Encoder* encoder)
{
  RefPointer<MediaPacket> packet = MediaPacket::make();
  do {
    encoder->encode(packet.value(), media);
    if (packet->isComplete()) {
      VS_LOG_DEBUG("Encode: %p; TS: %lld", encoder, packet->getDts());
      muxer->write(packet.value(), true);
    }
  } while (!media && packet->isComplete()); // this forces flushing if media is null
}

void
EncoderTest::decodeAndEncode(
    MediaPacket* packet,
    Decoder* decoder,
    Media* media,
    Muxer* muxer,
    Encoder* encoder
)
{
  int32_t offset = 0;
  int32_t bytesRead = 0;
  do {
    VS_LOG_DEBUG("Decode: %p; TS: %lld; offset: %lld",
        decoder, packet ? packet->getDts() : Global::NO_PTS,
            offset);
    bytesRead += decoder->decode(media, packet, offset);
    if (media->isComplete()) {
      // we encode and write it
      encodeAndMux(media, muxer, encoder);
    }
    offset += bytesRead;
  } while ((!packet && media->isComplete()) || (packet && offset < packet->getSize()));
  if (!packet) {
    // we should also flush the encoders
    encodeAndMux(0, muxer, encoder);
  }
}

/**
 * This test will read ironman (FLV, H263 video and mp3 audio) and transcode
 * to MP4 (H264 Video and aac audio).
 */
void
EncoderTest::testTranscode()
{
  TS_SKIP("not yet working");
  const bool isMemCheck = getenv("VS_TEST_MEMCHECK") ? true : false;
  if (isMemCheck)
    return; // don't test this under Valgrind yet

  TestData::Fixture* fixture=mFixtures.getFixture("testfile.flv");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Demuxer> source = Demuxer::make();
  source->open(filepath, 0, false, true, 0, 0);
  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  // Let's create a helper object to help us with decoding
  typedef struct {
    MediaDescriptor::Type type;
    RefPointer<DemuxerStream> stream;
    RefPointer<Decoder> decoder;
    RefPointer<Media> media;
  } DemuxerStreamHelper;

  // I know there are only 2 in the input file.
  DemuxerStreamHelper inputHelpers[10];
  for(int32_t i = 0; i < numStreams; i++) {
    DemuxerStreamHelper* helper = &inputHelpers[i];
    helper->stream = source->getStream(i);
    helper->decoder = helper->stream->getDecoder();
    helper->decoder->open(0, 0);
    helper->type = helper->decoder->getCodecType();
    if (helper->type == MediaDescriptor::MEDIA_AUDIO)
      helper->media = MediaAudio::make(
          helper->decoder->getFrameSize(),
          helper->decoder->getSampleRate(),
          helper->decoder->getChannels(),
          helper->decoder->getChannelLayout(),
          helper->decoder->getSampleFormat());
    else if (helper->type  == MediaDescriptor::MEDIA_VIDEO)
      helper->media = MediaPicture::make(
          helper->decoder->getWidth(),
          helper->decoder->getHeight(),
          helper->decoder->getPixelFormat()
      );
  }

  // now, let's set up our output file.
  RefPointer<Muxer> muxer = Muxer::make("EncoderTest_testTranscode.mp4", 0, 0);
  RefPointer<MuxerFormat> format = muxer->getFormat();

  // Let's create a helper object to help us with decoding
  typedef struct {
    MediaDescriptor::Type type;
    RefPointer<MuxerStream> stream;
    RefPointer<MediaAudioResampler> audioResampler;
    RefPointer<Encoder> encoder;
  } MuxerStreamHelper;
  MuxerStreamHelper outputHelpers[10];
  for(int32_t i = 0; i < numStreams; i++) {
    DemuxerStreamHelper *input = &inputHelpers[i];
    MuxerStreamHelper *output = &outputHelpers[i];
    output->type = input->type;
    RefPointer<Encoder> encoder;
    if (output->type == MediaDescriptor::MEDIA_VIDEO) {
      RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_H264);
      encoder = Encoder::make(codec.value());

      // set the encoder properties we need
      encoder->setWidth(input->decoder->getWidth());
      encoder->setHeight(input->decoder->getHeight());
      encoder->setPixelFormat(input->decoder->getPixelFormat());
      encoder->setProperty("b", (int64_t)400000); // bitrate
      encoder->setProperty("g", (int64_t) 10); // gop
      encoder->setProperty("bf", (int64_t)1); // max b frames

    } else if (output->type == MediaDescriptor::MEDIA_AUDIO) {
      RefPointer<Codec> codec = Codec::findEncodingCodec(Codec::CODEC_ID_AAC);
      encoder = Encoder::make(codec.value());

      // set the encoder properties we need
      encoder->setSampleRate(input->decoder->getSampleRate());
      encoder->setSampleFormat(input->decoder->getSampleFormat());
      encoder->setChannelLayout(input->decoder->getChannelLayout());
      encoder->setChannels(input->decoder->getChannels());
      encoder->setProperty("b", (int64_t)64000); // bitrate
    }
    output->encoder = encoder;
    if (output->encoder) {
      // if the container will require a global header, then the encoder needs to set this.
      RefPointer<Rational> tb = input->decoder->getTimeBase();
      output->encoder->setTimeBase(tb.value());

      if (format->getFlag(MuxerFormat::GLOBAL_HEADER))
        output->encoder->setFlag(Encoder::FLAG_GLOBAL_HEADER, true);

      output->encoder->open(0,0);
      output->stream = muxer->addNewStream(output->encoder.value());
    }
  }
  // now we should be ready to open the muxer
  muxer->open(0, 0);

  // now, let's start a decoding loop.
  RefPointer<MediaPacket> packet = MediaPacket::make();

  while(source->read(packet.value()) >= 0) {
    // got a packet; now we try to decode it.
    if (packet->isComplete()) {
      int32_t streamNo = packet->getStreamIndex();
      DemuxerStreamHelper *input = &inputHelpers[streamNo];
      MuxerStreamHelper* output = &outputHelpers[streamNo];
      decodeAndEncode(
          packet.value(),
          input->decoder.value(),
          input->media.value(),
          muxer.value(),
          output->encoder.value());
    }
  }

  // now, flush any cached packets
  for(int i = 0; i < numStreams; i++) {
    DemuxerStreamHelper *input = &inputHelpers[i];
    MuxerStreamHelper* output = &outputHelpers[i];
    decodeAndEncode(
        0,
        input->decoder.value(),
        input->media.value(),
        muxer.value(),
        output->encoder.value());

  }
  source->close();
  muxer->close();
}
