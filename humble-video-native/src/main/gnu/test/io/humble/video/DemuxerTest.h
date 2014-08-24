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
 * DemuxerTest.h
 *
 *  Created on: Jul 3, 2013
 *      Author: aclarke
 */

#ifndef SOURCETEST_H_
#define SOURCETEST_H_

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/Demuxer.h>

#include "TestData.h"

class DemuxerTest : public CxxTest::TestSuite
{
public:
  DemuxerTest();
  virtual
  ~DemuxerTest();
  void testMake();
  void testOpen();
  void testOpenDemuxerPrivatePropertySetting();
  void testOpenResetInputFormat();
  void testOpenCustomIO();
  void testOpenWithoutCloseAutoCloses();
  void testOpenInvalidArguments();
  void testRead();
private:
  void openTestHelper(const char* url);
  char mSampleFile[2048];
  TestData mFixtures;
  TestData::Fixture* mFixture;
};
#endif /* SOURCETEST_H_ */
