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
 * MediaAudioResamplerTest.h
 *
 *  Created on: Jul 30, 2013
 *      Author: aclarke
 */

#ifndef MEDIAAUDIORESAMPLERTEST_H_
#define MEDIAAUDIORESAMPLERTEST_H_

#include <io/humble/testutils/TestUtils.h>
#include "TestData.h"

#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaAudioResampler.h>

class MediaAudioResamplerTest : public CxxTest::TestSuite
{
public:
  MediaAudioResamplerTest();
  virtual
  ~MediaAudioResamplerTest();
  void testCreation();
  void testResampleErrors();
  void testResample();
private:
  void writeAudio(FILE* output, io::humble::video::MediaAudio* audio,
      io::humble::video::MediaAudioResampler*,
      io::humble::video::MediaAudio*);

  TestData mFixtures;
};

#endif /* MEDIAAUDIORESAMPLERTEST_H_ */
