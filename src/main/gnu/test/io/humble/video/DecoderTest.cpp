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
  RefPointer<IBuffer> buf;

  // we'll just write out the first channel.
  buf = audio->getData(0);
  size_t size = buf->getBufferSize();
  const void* data = buf->getBytes(0, size);
  fwrite(data, 1, size, output);
}
void
DecoderTest::testDecodeAudio() {

  TS_SKIP("Not yet implemented");

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

  RefPointer<SourceStream> stream = source->getSourceStream(0);
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
    if (packet->isComplete()) {
      int32_t bytesRead = 0;
      int32_t byteOffset=0;
      do {
        bytesRead = decoder->decodeAudio(audio.value(), packet.value(), byteOffset);
        if (audio->isComplete()) {
          writeAudio(output, audio.value());
        }
        // now, handle the case where bytesRead is 0; we need to flush any
        // cached packets
        do {
          decoder->decodeAudio(audio.value(), 0, 0);
          if (audio->isComplete()) {
            writeAudio(output, audio.value());
          }
        } while (audio->isComplete());
        byteOffset += bytesRead;
      } while(bytesRead > 0);
    }

  }

  fclose(output);
  source->close();
}
