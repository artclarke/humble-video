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
 * FilterGraphTest.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERGRAPHTEST_H_
#define FILTERGRAPHTEST_H_

#include <io/humble/testutils/TestUtils.h>
#include "TestData.h"
#include <io/humble/video/MediaPicture.h>

class FilterGraphTest : public CxxTest::TestSuite
{
public:
  FilterGraphTest();
  virtual ~FilterGraphTest();
  void testCreation();
  void testAddIO();
  void testFilterVideo();
private:

  void writePicture(const char* prefix, int32_t* frameNo, io::humble::video::MediaPicture* picture);

  TestData mFixtures;

};
#endif /* FILTERGRAPHTEST_H_ */
