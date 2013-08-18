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
 * MuxerTest.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#include "MuxerTest.h"

MuxerTest::MuxerTest() {

}

MuxerTest::~MuxerTest() {
}

void
MuxerTest::testCreation() {
  RefPointer<Muxer> muxer;

  muxer = Muxer::make(0, "MuxerTest_testCreation.flv", 0);

  RefPointer<MuxerStream> stream;

  RefPointer<Codec> c = Codec::findEncodingCodec(Codec::CODEC_ID_FLV1);
  RefPointer<Encoder> e = Encoder::make(c.value());
  RefPointer<Rational> t = Rational::make(1, 1000);

  e->setWidth(480);
  e->setHeight(320);
  e->setPixelType(PixelFormat::PIX_FMT_YUV420P);
  e->setTimeBase(t.value());

  e->open(0, 0);

  stream = muxer->addNewStream(e.value());

  muxer->open(0, 0);

  muxer->close();


}
