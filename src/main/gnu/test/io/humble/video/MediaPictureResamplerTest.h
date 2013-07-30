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
 * MediaPictureResamplerTest.h
 *
 *  Created on: Jul 30, 2013
 *      Author: aclarke
 */

#ifndef MEDIAPICTURERESAMPLERTEST_H_
#define MEDIAPICTURERESAMPLERTEST_H_

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/MediaPictureResampler.h>
#include "TestData.h"

class MediaPictureResamplerTest : public CxxTest::TestSuite
{
public:
  MediaPictureResamplerTest();
  virtual
  ~MediaPictureResamplerTest();
  void testRescale();
private:
  void writePicture(const char* prefix, int32_t* frameNo,
      io::humble::video::MediaPicture* picture,
      io::humble::video::MediaPictureResampler*,
      io::humble::video::MediaPicture*);
  TestData mFixtures;
\
};

#endif /* MEDIAPICTURERESAMPLERTEST_H_ */
