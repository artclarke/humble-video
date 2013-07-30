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
 * DecoderTest.cpp
 *
 *  Created on: Jul 28, 2013
 *      Author: aclarke
 */

#include "DecoderTest.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Source.h>
#include <io/humble/video/SourceStream.h>
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
  TS_ASSERT_EQUALS(-1, (int32_t)decoder->getPropertyAsLong("dual_mono_mode"));

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

  RefPointer<Source> source = Source::make();

  int32_t retval=-1;
  retval = source->open(filepath, 0, false, true, 0, 0);
  TS_ASSERT(retval >= 0);

  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  int32_t streamToDecode = 0;
  RefPointer<SourceStream> stream = source->getSourceStream(streamToDecode);
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

  while(source->read(packet.value()) >= 0) {
    // got a packet; now we try to decode it.
    if (packet->getStreamIndex() == streamToDecode &&
        packet->isComplete()) {
      int32_t bytesRead = 0;
      int32_t byteOffset=0;
      do {
        bytesRead = decoder->decodeAudio(audio.value(), packet.value(), byteOffset);
        if (audio->isComplete()) {
          writeAudio(output, audio.value());
        }
        byteOffset += bytesRead;
      } while(byteOffset < packet->getSize());
      // now, handle the case where bytesRead is 0; we need to flush any
      // cached packets
      do {
        decoder->decodeAudio(audio.value(), 0, 0);
        if (audio->isComplete()) {
          writeAudio(output, audio.value());
        }
      } while (audio->isComplete());
    }

  }

  fclose(output);
  source->close();
}


void
DecoderTest::writePicture(const char* prefix, int32_t* frameNo, MediaPicture* picture)
{
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

//  TS_SKIP("Not yet implemented");

  TestData::Fixture* fixture=mFixtures.getFixture("testfile.flv");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Source> source = Source::make();

  int32_t retval=-1;
  retval = source->open(filepath, 0, false, true, 0, 0);
  TS_ASSERT(retval >= 0);

  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  int32_t streamToDecode = 0;
  RefPointer<SourceStream> stream = source->getSourceStream(streamToDecode);
  TS_ASSERT(stream);
  RefPointer<Decoder> decoder = stream->getDecoder();
  TS_ASSERT(decoder);
  RefPointer<Codec> codec = decoder->getCodec();
  TS_ASSERT(codec);
  TS_ASSERT_EQUALS(Codec::CODEC_ID_FLV1, codec->getID());

  decoder->open(0, 0);

  // now, let's start a decoding loop.
  RefPointer<MediaPacket> packet = MediaPacket::make();

  RefPointer<MediaPicture> picture = MediaPicture::make(
      decoder->getWidth(),
      decoder->getHeight(),
      decoder->getPixelFormat());

  int32_t frameNo = 0;
  while(source->read(packet.value()) >= 0) {
    // got a packet; now we try to decode it.
    if (packet->getStreamIndex() == streamToDecode &&
        packet->isComplete()) {
      int32_t bytesRead = 0;
      int32_t byteOffset=0;
      do {
        bytesRead = decoder->decodeVideo(picture.value(), packet.value(), byteOffset);
        if (picture->isComplete()) {
          writePicture("DecoderTest_testDecodeVideo", &frameNo, picture.value());
        }
        byteOffset += bytesRead;
      } while(byteOffset < packet->getSize());
      // now, handle the case where bytesRead is 0; we need to flush any
      // cached packets
      do {
        decoder->decodeVideo(picture.value(), 0, 0);
        if (picture->isComplete()) {
          writePicture("DecoderTest_testDecodeVideo", &frameNo, picture.value());
        }
      } while (picture->isComplete());
    }

  }
  source->close();
}
