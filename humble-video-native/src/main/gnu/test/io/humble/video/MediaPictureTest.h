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
 * MediaPictureTester.h
 *
 *  Created on: Jul 13, 2013
 *      Author: aclarke
 */

#ifndef MEDIAPICTURETEST_H_
#define MEDIAPICTURETEST_H_

#include <io/humble/testutils/TestUtils.h>

class MediaPictureTest : public CxxTest::TestSuite
{
public:

  MediaPictureTest();

  virtual
  ~MediaPictureTest();

  void testCreation();
  void testCreationInvalidParameters();
  void testCreationFromBuffer();

};

#endif /* MEDIAPICTURETEST_H_ */
