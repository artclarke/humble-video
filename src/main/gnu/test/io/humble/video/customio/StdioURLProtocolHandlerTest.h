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
#ifndef STDIOURLHANDLERTEST_H_
#define STDIOURLHANDLERTEST_H_

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/customio/StdioURLProtocolManager.h>

using namespace io::humble::video::customio;

class StdioURLProtocolHandlerTest: public CxxTest::TestSuite
{
public:
  StdioURLProtocolHandlerTest();
  virtual
  ~StdioURLProtocolHandlerTest();
  void setUp();
  void tearDown();
  void testCreation();
  void testOpenClose();
  void testRead();
  void testReadWrite();
  void testSeek();
  void testSeekableFlags();
private:
  const char * FIXTURE_DIRECTORY;
  const char * SAMPLE_FILE;
  char mFixtureDir[4098];
  char mSampleFile[4098];
};

#endif /* STDIOURLHANDLERTEST_H_ */
