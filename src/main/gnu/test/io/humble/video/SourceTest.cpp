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
 * SourceTest.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: aclarke
 */

// for getenv
#include <cstdlib>

#include "SourceTest.h"
#include <io/humble/video/SourceImpl.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

SourceTest::SourceTest() {
}

SourceTest::~SourceTest() {
}

void
SourceTest::testMake() {
  RefPointer<Source> source = Source::make();
  // for debugging, we'll keep another value around to look at
  SourceImpl* obj = (SourceImpl*) source.value();
  (void) obj;

  VS_TUT_ENSURE("", source);
  VS_TUT_ENSURE("", source->getState() == Container::STATE_INITED);
}

void
SourceTest::testOpen() {
  RefPointer<Source> source = Source::make();
  VS_TUT_ENSURE("", source);
  char file[2048];
  const char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
  const char *sample = "testfile_h264_mp4a_tmcd.mov";
  if (fixtureDirectory && *fixtureDirectory)
    snprintf(file, sizeof(file), "%s/%s", fixtureDirectory, sample);
  else
    snprintf(file, sizeof(file), "./%s", sample);

  int32_t retval = source->open(file, 0, false, false, 0, 0);
  VS_TUT_ENSURE("", retval >= 0);
  VS_TUT_ENSURE("", source->getState() == Container::STATE_OPENED);

  retval = source->close();
  VS_TUT_ENSURE("", retval >= 0);
}

