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
  TS_ASSERT_EQUALS(14, n);
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

  // let's generate a buffer to add noise to.
  const int32_t inSize = 1024;
  // Annoyingly I have to add FF_INPUT_BUFFER_PADDING_SIZE because the noise
  // filter incorrectly memcopies an additional 16 bytes, and I want Valgrind to
  // not worry about that.
  RefPointer<Buffer> inBuf = Buffer::make(0, inSize+FF_INPUT_BUFFER_PADDING_SIZE);
  uint8_t* in = static_cast<uint8_t*>(inBuf->getBytes(0, inSize));
  int32_t outSize = inSize;
  RefPointer<Buffer> outBuf = Buffer::make(0, outSize+FF_INPUT_BUFFER_PADDING_SIZE);
  uint8_t* out = static_cast<uint8_t*>(outBuf->getBytes(0, outSize));
  for(int32_t i = 0; i < inSize+FF_INPUT_BUFFER_PADDING_SIZE; i++) {
    in[i] = static_cast<uint8_t>(i);
    out[i] = in[i];
  }

  outSize = f->filter(outBuf.value(), 0, inBuf.value(), 0, inSize, 0, 0, false);
  TS_ASSERT_EQUALS(outSize, inSize);
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

  // let's generate a buffer to chomp nulls off the end of.
  const int32_t inSize = 1024;
  RefPointer<Buffer> inBuf = Buffer::make(0, inSize);
  uint8_t* in = static_cast<uint8_t*>(inBuf->getBytes(0, inSize));
  int32_t outSize = inSize;
  RefPointer<Buffer> outBuf = Buffer::make(0, outSize);
  uint8_t* out = static_cast<uint8_t*>(outBuf->getBytes(0, outSize));

  // set the entire input buffer to null for now.
  memset(in, 0, inSize);
  // then set the first half of the buffer to be non-null.
  for(int32_t i = 0; i < inSize/2; i++) {
    in[i] = 0x01;
    out[i] = in[i];
  }

  // should filter out all the null bytes at the end.
  outSize = f->filter(outBuf.value(), 0, inBuf.value(), 0, inSize, 0, 0, false);
  TS_ASSERT_EQUALS(outSize, inSize/2);
  for(int32_t i = 0; i < inSize/2; i++) {
    TS_ASSERT_EQUALS(in[i], out[i]);
  }
}
