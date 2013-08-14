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
 * MuxerTest.h
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#ifndef MUXERTEST_H_
#define MUXERTEST_H_

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/Muxer.h>
#include "TestData.h"

using namespace io::humble::video;
using namespace io::humble::ferry;

class MuxerTest : public CxxTest::TestSuite
{
public:
  MuxerTest();
  virtual
  ~MuxerTest();
  void testCreation();
};

#endif /* MUXERTEST_H_ */
