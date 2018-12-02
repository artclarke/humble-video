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
 * DecoderTest.cpp
 *
 *  Created on: Jul 28, 2013
 *      Author: aclarke
 */
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>

#include "DecoderTest.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Demuxer.h>
#include <io/humble/video/DemuxerStream.h>
#include <io/humble/video/MediaAudio.h>

DecoderTest::DecoderTest() {
}

DecoderTest::~DecoderTest() {
}

void
DecoderTest::testCreation()
{
  RefPointer<Codec> codec = Codec::findDecodingCodec(Codec::CODEC_ID_H264);
  RefPointer<Decoder> decoder = Decoder::make(codec.value());
  TS_ASSERT(decoder);

  RefPointer<Decoder> copy = Decoder::make(decoder.value());
  TS_ASSERT(copy);

  RefPointer<Codec> copyCodec = copy->getCodec();
  TS_ASSERT_EQUALS(codec->getID(), copyCodec->getID());
}

void
DecoderTest::testCreationWithErrors()
{
  TS_ASSERT_THROWS(Decoder::make((Codec*)0),
      HumbleInvalidArgument);
  TS_ASSERT_THROWS(Decoder::make((Decoder*)0),
      HumbleInvalidArgument);
}
void
DecoderTest::testOpen() {
  RefPointer<Codec> codec = Codec::findDecodingCodec(Codec::CODEC_ID_H264);
  RefPointer<Decoder> decoder = Decoder::make(codec.value());
  TS_ASSERT(decoder);

  TS_ASSERT_EQUALS(Coder::STATE_INITED, decoder->getState());

  RefPointer<Decoder> copy;
  copy = Decoder::make(decoder.value());
  TS_ASSERT_EQUALS(Coder::STATE_INITED, copy->getState());

  // try opening both codecs
  decoder->open(0, 0);

  TS_ASSERT_EQUALS(Coder::STATE_OPENED, decoder->getState());

  copy = Decoder::make(decoder.value());
  TS_ASSERT_EQUALS(Coder::STATE_INITED, copy->getState());

  copy->open(0, 0);
  TS_ASSERT_EQUALS(Coder::STATE_OPENED, copy->getState());
}

void
DecoderTest::testOpenWithOptions() {
  RefPointer<KeyValueBag> inOpts = KeyValueBag::make();
  RefPointer<KeyValueBag> outOpts = KeyValueBag::make();

  RefPointer<Codec> codec = Codec::findDecodingCodec(Codec::CODEC_ID_AAC);
  RefPointer<Decoder> decoder = Decoder::make(codec.value());

  TS_ASSERT_EQUALS(0, (int32_t)decoder->getPropertyAsLong("maxrate"));
//  TS_ASSERT_EQUALS(-1, (int32_t)decoder->getPropertyAsLong("dual_mono_mode"));

  // a valid option, all decoders
  inOpts->setValue("maxrate", "500000");
  // a valid option, custom to the give codec
  inOpts->setValue("dual_mono_mode", "both");

  // an invalid option
  inOpts->setValue("not_an_option", "farfegnugen");

  TS_ASSERT_EQUALS(0, outOpts->getNumKeys());

  decoder->open(inOpts.value(), outOpts.value());

  TS_ASSERT_EQUALS(1, outOpts->getNumKeys());
  TS_ASSERT(strcmp("farfegnugen", outOpts->getValue("not_an_option", KeyValueBag::KVB_NONE))==0);
  TS_ASSERT_EQUALS(500000, (int32_t)decoder->getPropertyAsLong("maxrate"));
  // for some reason this does not work; need to figure out why (later).
  // TS_ASSERT_EQUALS(2, (int32_t)decoder->getPropertyAsLong("dual_mono_mode"));
}

void
DecoderTest::writeAudio(FILE* output, MediaAudio* audio)
{
  TS_ASSERT(audio->isComplete());
  RefPointer<Rational> timeBase = audio->getTimeBase();
  TS_ASSERT(timeBase);
  TS_ASSERT_DIFFERS(Global::NO_PTS, audio->getPacketDts());
  TS_ASSERT_DIFFERS(Global::NO_PTS, audio->getTimeStamp());

  RefPointer<Buffer> buf;

  // we'll just write out the first channel.
  buf = audio->getData(0);
  size_t size = buf->getBufferSize();
  const void* data = buf->getBytes(0, size);
  fwrite(data, 1, size, output);
}

void
DecoderTest::testDecodeAudio() {

//  TS_SKIP("Not yet implemented");

  TestData::Fixture* fixture=mFixtures.getFixture("testfile.mp3");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Demuxer> source = Demuxer::make();

  source->open(filepath, 0, false, true, 0, 0);

  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  int32_t streamToDecode = 0;
  RefPointer<DemuxerStream> stream = source->getStream(streamToDecode);
  TS_ASSERT(stream);
  RefPointer<Decoder> decoder = stream->getDecoder();
  TS_ASSERT(decoder);
  RefPointer<Codec> codec = decoder->getCodec();
  TS_ASSERT(codec);
  TS_ASSERT_EQUALS(Codec::CODEC_ID_MP3, codec->getID());

  FILE* output = fopen("DecoderTest_testDecodeAudio.au", "wb");
  TS_ASSERT(output);

  decoder->open(0, 0);

  // now, let's start a decoding loop.
  RefPointer<MediaPacket> packet = MediaPacket::make();

  // make audio to read into
  RefPointer<MediaAudio> audio = MediaAudio::make(
      decoder->getFrameSize(),
      decoder->getSampleRate(),
      decoder->getChannels(),
      decoder->getChannelLayout(),
      decoder->getSampleFormat()
  );

  int64_t oldTimeStamp = Global::NO_PTS;
  ProcessorResult r = RESULT_SUCCESS;

  int retval = 0;
  do {
    retval = source->read(packet.value());
    if (retval >= 0 &&
        (!packet->isComplete() || packet->getStreamIndex() != streamToDecode)
    )
      // we read some data, but not the right packet or not enough yet
      continue;

    MediaPacket* p = packet.value();
    if (retval <0)
      p = 0; // force flushing if end of file.

    r = decoder->send(p); // tell the decoder about the packet
    if (p) {
      TS_ASSERT_EQUALS(r, RESULT_SUCCESS);
    } else {
      // when flushing, a second call to the decoder should then be END_OF_STREAM
      r = decoder->send(p); // send again.
      TS_ASSERT_EQUALS(r, RESULT_END_OF_STREAM);
    }

    while((r = decoder->receive(audio.value())) == RESULT_SUCCESS) {
      writeAudio(output, audio.value());
      if (oldTimeStamp != Global::NO_PTS) {
        // check for monotonically increasing
        TS_ASSERT(oldTimeStamp < audio->getTimeStamp());
      }
      oldTimeStamp = audio->getTimeStamp();
    }
    if (p)
      TS_ASSERT_EQUALS(r, RESULT_AWAITING_DATA);

  } while (retval >= 0);
  // and when exiting the decoder receive loop should have gotten EOS.
  TS_ASSERT_EQUALS(r, RESULT_END_OF_STREAM);
  fclose(output);
  source->close();
}


void
DecoderTest::writePicture(const char* prefix, int32_t* frameNo, MediaPicture* picture)
{
  TS_ASSERT(picture->isComplete());
  TS_ASSERT_DIFFERS(Global::NO_PTS, picture->getPts());
  TS_ASSERT_DIFFERS(Global::NO_PTS, picture->getTimeStamp());

  char filename[2048];
  // write data as PGM file.
  snprintf(filename, sizeof(filename), "%s-%06d.pgm", prefix, *frameNo);
  // only write every n'th frame to save disk space
  RefPointer<Buffer> buf = picture->getData(0);
  uint8_t* data = (uint8_t*)buf->getBytes(0, buf->getBufferSize());
  if (!((*frameNo) % 30)) {
    FILE* output = fopen(filename, "wb");
    TS_ASSERT(output);
    fprintf(output,"P5\n%d %d\n%d\n",picture->getWidth(),picture->getHeight(),255);
    for(int32_t i=0;i<picture->getHeight();i++)
        fwrite(data + i * picture->getLineSize(0),1,picture->getWidth(),output);
    fclose(output);
  }
#ifdef DECODER_TEST_ASCII_ART
  // now for fun, ascii art
  puts("\033c");
  uint8_t* p0=data;
  for (int32_t y = 0; y < picture->getHeight(); y++) {
    uint8_t* p = p0;
    for (int32_t x = 0; x < picture->getWidth(); x++)
      putchar((int)(" .-+#"[*(p++) / 52]));
    putchar('\n');
    p0 += picture->getLineSize(0);
    fflush(stdout);
  }
#endif

  (*frameNo)++;
}

void
DecoderTest::testDecodeVideo() {

//  TestData::Fixture* fixture=mFixtures.getFixture("testfile_h264_mp4a_tmcd.mov");
  TestData::Fixture* fixture=mFixtures.getFixture("ucl_h264_aac.mp4");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Demuxer> source = Demuxer::make();

  source->open(filepath, 0, false, true, 0, 0);

  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  int32_t streamToDecode = -1;
  RefPointer<Decoder> decoder;
  for(int i =0 ; i < numStreams; i++) {
    RefPointer<DemuxerStream> stream = source->getStream(i);
    TS_ASSERT(stream);
    decoder = stream->getDecoder();
    TS_ASSERT(decoder);
    if (decoder->getCodecType() == MediaDescriptor::MEDIA_VIDEO) {
      streamToDecode = 1;
    }
  }
  TS_ASSERT(streamToDecode >= 0);

  decoder->open(0, 0);

  // now, let's start a decoding loop.
  RefPointer<MediaPacket> packet = MediaPacket::make();

  RefPointer<MediaPicture> picture = MediaPicture::make(
      decoder->getWidth(),
      decoder->getHeight(),
      decoder->getPixelFormat());

  int32_t frameNo = 0;

  ProcessorResult r = RESULT_SUCCESS;

  int retval = 0;
  do {
    retval = source->read(packet.value());
    if (retval >= 0 &&
        (!packet->isComplete() || packet->getStreamIndex() != streamToDecode)
    )
      // we read some data, but not the right packet or not enough yet
      continue;

    MediaPacket* p = packet.value();
    if (retval <0)
      p = 0; // force flushing if end of file.

    r = decoder->send(p); // tell the decoder about the packet
    if (p) {
      TS_ASSERT_EQUALS(r, RESULT_SUCCESS);
    } else {
      // when flushing, a second call to the decoder should then be END_OF_STREAM
      r = decoder->send(p); // send again.
      TS_ASSERT_EQUALS(r, RESULT_END_OF_STREAM);
    }


    while((r = decoder->receive(picture.value())) == RESULT_SUCCESS) {
      TS_ASSERT_DIFFERS(Global::NO_PTS, picture->getPacketDts());
      writePicture("DecoderTest_testDecodeVideo", &frameNo, picture.value());
    }
    if (p)
      TS_ASSERT_EQUALS(r, RESULT_AWAITING_DATA);

  } while (retval >= 0);
  // and when exiting the decoder receive loop should have gotten EOS.
  TS_ASSERT_EQUALS(r, RESULT_END_OF_STREAM);
  source->close();
}

void
DecoderTest::testOpenCloseMP4() {
  TestData::Fixture* fixture=mFixtures.getFixture("ucl_h264_aac.mp4");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Demuxer> source = Demuxer::make();

  source->open(filepath, 0, false, true, 0, 0);
  int32_t n = source->getNumStreams();
  for(int i = 0; i < n; i++) {
    RefPointer<DemuxerStream> s = source->getStream(i);
    RefPointer<Decoder> d = s->getDecoder();
    if (d->getCodecType() == MediaDescriptor::MEDIA_AUDIO ||
        d->getCodecType() == MediaDescriptor::MEDIA_VIDEO) {
      // open the decoders
      d->open(0, 0);
    }
  }
  source->close();
}
void
DecoderTest::testIssue27()
{
  const char* testURL="https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8";
  RefPointer<Demuxer> demuxer = Demuxer::make();

  RefPointer<Decoder> decoder;
  int32_t decoderIndex = -1;
  try {
    demuxer->open(testURL, 0, true, false, 0, 0);
    demuxer->queryStreamMetaData();
    int32_t n = demuxer->getNumStreams();
    // loop through until we find our first video stream.
    for(int32_t i = 0; i < n; ++i) {
      RefPointer<DemuxerStream> stream = demuxer->getStream(i);
      decoder = stream->getDecoder();
      if (decoder->getCodecType() == MediaDescriptor::MEDIA_VIDEO) {
        decoderIndex = i;
        break;
      }
      decoder = 0;
    }
  } catch (std::exception & e) {
    // if we catch an error, just return. that's not the focus of this test, and it will fail if the URL or network is down.
    return;
  }
  if (!decoder.value())
    return;

  // now let's decode some number of frames.
  int32_t frames = 150;
  int32_t frameNo = 0;
  RefPointer<MediaPacket> packet = MediaPacket::make();
  RefPointer<MediaPicture> picture = MediaPicture::make(
      decoder->getWidth(),
      decoder->getHeight(),
      decoder->getPixelFormat());

  decoder->open(0, 0);
  ProcessorResult r = RESULT_SUCCESS;

  int retval = 0;
  do {
    retval = demuxer->read(packet.value());
    if (retval >= 0 &&
        (!packet->isComplete() || packet->getStreamIndex() != decoderIndex)
    )
      // we read some data, but not the right packet or not enough yet
      continue;

    MediaPacket* p = packet.value();
    if (retval <0 || frameNo >= frames)
      p = 0; // force flushing if end of file.

    r = decoder->send(p); // tell the decoder about the packet
    if (p) {
      TS_ASSERT_EQUALS(r, RESULT_SUCCESS);
    } else {
      // when flushing, r should then be END_OF_STREAM if we flush again
      r = decoder->send(0);
      TS_ASSERT_EQUALS(r, RESULT_END_OF_STREAM);
    }

    while((r = decoder->receive(picture.value())) == RESULT_SUCCESS) {
      writePicture("DecoderTest_testIssue27", &frameNo, picture.value());
    }

    if (p)
      // if we are not yet flushing, the encoder should always be awaiting data.
      TS_ASSERT_EQUALS(r, RESULT_AWAITING_DATA);

  } while (retval >= 0 && frameNo < frames);

  demuxer->close();
}
