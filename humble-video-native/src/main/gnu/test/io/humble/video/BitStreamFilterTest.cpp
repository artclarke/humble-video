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

#include "BitStreamFilterTest.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Buffer.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

BitStreamFilterTypeTest::BitStreamFilterTypeTest ()
{
}

BitStreamFilterTypeTest::~BitStreamFilterTypeTest ()
{
}

void
BitStreamFilterTypeTest::testGetNumBitStreamFilterTypes() {
  int32_t n = BitStreamFilterType::getNumBitStreamFilterTypes();
  // NOTE: This will break each time a new bit filter type is added to Ffmpeg,
  // but I want that so we re-look at it.
  TS_ASSERT_EQUALS(30, n);
}

void
BitStreamFilterTypeTest::testGetBitStreamFilterType() {
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_INFO, false);

  // This test makes sure that the getBitStreamFilterType method calls Global::init
  RefPointer<BitStreamFilterType> t = BitStreamFilterType::getBitStreamFilterType(0);
  TS_ASSERT(t.value());

  int32_t n = BitStreamFilterType::getNumBitStreamFilterTypes();
  for(int32_t i = 0; i < n; i++) {
    RefPointer<BitStreamFilterType> t1 = BitStreamFilterType::getBitStreamFilterType(i);
    TS_ASSERT(t1->getName() != 0);
    RefPointer<BitStreamFilterType> t2 = BitStreamFilterType::getBitStreamFilterType(t1->getName());
    TS_ASSERT(strcmp(t1->getName(), t2->getName()) == 0);
    VS_LOG_DEBUG("(%03d) Filter: %s", i, t1->getName());
  }
}

BitStreamFilterTest::BitStreamFilterTest ()
{
}

BitStreamFilterTest::~BitStreamFilterTest ()
{
}

void
BitStreamFilterTest::testMakeByName () {
  const char* name = "noise";
  RefPointer<BitStreamFilter> f = BitStreamFilter::make(name);
  TS_ASSERT(strcmp(name, f->getName())==0);
  RefPointer<BitStreamFilterType> t = f->getType();
  TS_ASSERT(strcmp(name, t->getName())==0);
}
void
BitStreamFilterTest::testMakeByType () {
  const char* name = "noise";
  RefPointer<BitStreamFilterType> t = BitStreamFilterType::getBitStreamFilterType(name);
  TS_ASSERT(strcmp(name, t->getName())==0);
  RefPointer<BitStreamFilter> f = BitStreamFilter::make(t.value());
  TS_ASSERT(strcmp(name, f->getName())==0);
}

void
BitStreamFilterTest::testNoiseFilter() {
  const char* name = "noise";
  RefPointer<BitStreamFilter> f = BitStreamFilter::make(name);


  f->open(0, 0);

  // let's generate a buffer to add noise to.
  const int32_t inSize = 1024;
  // Annoyingly I have to add AVPROBE_PADDING_SIZE because the noise
  // filter incorrectly memcopies an additional 16 bytes, and I want Valgrind to
  // not worry about that.
  // 2018 note: removing this until it shows up again under Valgrind
  RefPointer<Buffer> inBuf = Buffer::make(0, inSize);
  uint8_t* in = static_cast<uint8_t*>(inBuf->getBytes(0, inSize));
  for(int32_t i = 0; i < inSize; i++) {
    in[i] = static_cast<uint8_t>(i);
  }

  // now, let's create a packet out of that buffer
  RefPointer<MediaPacket> p = MediaPacket::make(inBuf.value());

  // send to the filter
  ProcessorResult r;
  r = f->sendPacket(p.value());
  TS_ASSERT_EQUALS(RESULT_SUCCESS, r);

  // fetch the packet
  r = f->receivePacket(p.value());
  TS_ASSERT_EQUALS(RESULT_SUCCESS, r);

  RefPointer<Buffer> outBuf = p->getData();
  uint8_t *out = static_cast<uint8_t*>(outBuf->getBytes(0, p->getSize()));
  // should be a different buffer not
  TS_ASSERT_DIFFERS(in, out);
  TS_ASSERT_EQUALS(inSize, p->getSize());

  int matches = 0;
  for(int32_t i = 0; i < inSize; i++) {
    if (in[i] == out[i])
      ++ matches;
  }
  // noise should make sure that not all elements match.
  TS_ASSERT(matches < inSize);
}

void
BitStreamFilterTest::testChompFilter() {
  const char* name = "chomp";
  RefPointer<BitStreamFilter> f = BitStreamFilter::make(name);

  f->open(0, 0);

  // let's generate a buffer to chomp nulls off the end of.
  const int32_t inSize = 1024;
  RefPointer<Buffer> inBuf = Buffer::make(0, inSize);
  uint8_t* in = static_cast<uint8_t*>(inBuf->getBytes(0, inSize));

  // set the entire input buffer to null for now.
  memset(in, 0, inSize);
  // then set the first half of the buffer to be non-null.
  for(int32_t i = 0; i < inSize/2; i++) {
    in[i] = 0x01;
  }

  // now, let's create a packet out of that buffer
  RefPointer<MediaPacket> p = MediaPacket::make(inBuf.value());

  // send to the filter
  ProcessorResult r;
  r = f->sendPacket(p.value());
  TS_ASSERT_EQUALS(RESULT_SUCCESS, r);

  // fetch the packet
  r = f->receivePacket(p.value());
  TS_ASSERT_EQUALS(RESULT_SUCCESS, r);

  TS_ASSERT_EQUALS(p->getSize(), inSize/2);
  RefPointer<Buffer> outBuf = p->getData();
  uint8_t *out = static_cast<uint8_t*>(outBuf->getBytes(0, p->getSize()));

  for(int32_t i = 0; i < inSize/2; i++) {
    TS_ASSERT_EQUALS(in[i], out[i]);
  }
}
