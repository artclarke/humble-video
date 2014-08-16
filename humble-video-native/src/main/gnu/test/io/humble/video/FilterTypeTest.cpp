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
 * FilterTypeTest.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#include "FilterTypeTest.h"

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/video/FilterType.h>
#include <io/humble/video/VideoExceptions.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

FilterTypeTest::FilterTypeTest() {
}

FilterTypeTest::~FilterTypeTest() {
}

void
FilterTypeTest::testEnumeration() {
  int n = FilterType::getNumFilterTypes();
  TS_ASSERT(n > 0);

  for(int i = 0; i < n; i++) {
    RefPointer<FilterType> ft = FilterType::getFilterType(i);
    TS_ASSERT(ft);

    const char* name = ft->getName();
    TS_ASSERT(name && *name);

    RefPointer<FilterType> ft2 = FilterType::findFilterType(name);
    TS_ASSERT(name);

    {
      LoggerStack stack;
      stack.setGlobalLevel(Logger::LEVEL_DEBUG, false);

      VS_LOG_DEBUG("Name: %s", ft->getName());
      VS_LOG_DEBUG("Description: %s", ft->getDescription());
      VS_LOG_DEBUG("Num Inputs: %d", ft->getNumInputs());
      for(int j = 0; j < ft->getNumInputs(); j++) {
        VS_LOG_DEBUG("   Input %d: %s (%d)", j, ft->getInputName(j), ft->getInputType(j));
      }
      VS_LOG_DEBUG("Num Outputs: %d", ft->getNumOutputs());
      for(int j = 0; j < ft->getNumOutputs(); j++) {
        VS_LOG_DEBUG("   Output %d: %s (%d)", j, ft->getOutputName(j), ft->getOutputType(j));
      }

    }

  }

  // now test some edge conditions
  {
    LoggerStack stack;
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);
    TS_ASSERT_THROWS(FilterType::findFilterType(0), HumbleInvalidArgument);
    TS_ASSERT_THROWS(FilterType::findFilterType("notafiltertypeatall"), PropertyNotFoundException);
  }
}

