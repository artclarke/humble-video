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
 * EncoderTest.h
 *
 *  Created on: Sep 6, 2013
 *      Author: aclarke
 */

#ifndef ENCODERTEST_H_
#define ENCODERTEST_H_
#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/Encoder.h>
#include "TestData.h"

using namespace io::humble::video;
using namespace io::humble::ferry;

class EncoderTest : public CxxTest::TestSuite
{
public:
  EncoderTest();
  virtual
  ~EncoderTest();

  void testCreation();
};

#endif /* ENCODERTEST_H_ */
