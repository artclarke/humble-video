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
  TS_ASSERT_EQUALS(12, n);
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
BitStreamFilterTest::testTrue () {

}
