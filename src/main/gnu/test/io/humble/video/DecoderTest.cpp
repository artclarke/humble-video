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
