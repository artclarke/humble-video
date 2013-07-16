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
 * PixelFormatTest.cpp
 *
 *  Created on: Jul 16, 2013
 *      Author: aclarke
 */

#include "PixelFormatTest.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/PixelFormat.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;
using namespace io::humble::video;

PixelFormatTest::PixelFormatTest() {

}

PixelFormatTest::~PixelFormatTest() {
}

void
PixelFormatTest::testGetInstalledFormats() {
  int32_t n = PixelFormat::getNumInstalledFormats();

  TS_ASSERT_LESS_THAN(30, n);

  // now get all of them
  for(int32_t i = 0; i < n; i++) {
    RefPointer<PixelFormatDescriptor> d = PixelFormat::getInstalledFormatDescriptor(i);

    TS_ASSERT(d);

    {
      LoggerStack stack;
      stack.setGlobalLevel(Logger::LEVEL_DEBUG, false);

      VS_LOG_DEBUG("Got Descriptor: %s", d->getName());
    }


  }
}
