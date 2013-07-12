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

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include "SourceTest.h"
#include <io/humble/video/SourceImpl.h>
#include <io/humble/video/customio/StdioURLProtocolManager.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;
using namespace io::humble::video;
using namespace io::humble::video::customio;

SourceTest::SourceTest() {
  mFixture = 0;
  mSampleFile[0] = 0;
  mFixture = mFixtures.getFixture(0);
  TSM_ASSERT("Missing fixture", mFixture);
  mFixtures.fillPath(mFixture, mSampleFile, sizeof(mSampleFile));
  VS_LOG_TRACE("Sample File: %s", mSampleFile);
}

SourceTest::~SourceTest() {
  URLProtocolManager::unregisterAllProtocols();
}

void
SourceTest::testMake() {
  RefPointer<Source> source = Source::make();
  // for debugging, we'll keep another value around to look at
  SourceImpl* obj = (SourceImpl*) source.value();
  (void) obj;

  TS_ASSERT(source);
  TS_ASSERT(source->getState() == Container::STATE_INITED);
}

void
SourceTest::testOpen() {
  openTestHelper(mSampleFile);
}

void
SourceTest::openTestHelper(const char* file)
{
  RefPointer<Source> source = Source::make();
  TS_ASSERT(source);

  int32_t retval = source->open(file, 0, false, false, 0, 0);
  TS_ASSERT(retval >= 0);
  TS_ASSERT(source->getState() == Container::STATE_OPENED);

  // now, let's get the meta-data
  retval = source->queryStreamMetaData();
  TS_ASSERT(retval >= 0);

  // let's call it again; if we accidentally call the FFmpeg method
  // twice it will cause a crash, but the Source object should hide that.
  retval = source->queryStreamMetaData();
  TS_ASSERT(retval >= 0);

  RefPointer<KeyValueBag> metadata = source->getMetaData();
  TS_ASSERT(metadata);

  int32_t n = metadata->getNumKeys();
  TS_ASSERT_EQUALS(0, n);
  for (int32_t i = 0; i < n; i++) {
    const char* key = metadata->getKey(i);
    const char* val = metadata->getValue(key, KeyValueBag::KVB_NONE);
    VS_LOG_DEBUG("%s, %s", key, val);
  }

  int32_t duration = (int32_t) source->getDuration();
  TSM_ASSERT_EQUALS("Unexpected duration", mFixture->duration, duration);
  
  int32_t filesize = (int32_t) source->getFileSize();
  TSM_ASSERT_EQUALS("Unexpected filesize", mFixture->filesize, filesize);

  TSM_ASSERT_EQUALS("Unexpected bitrate", mFixture->bitrate, source->getBitRate());

  TS_ASSERT(strcmp(file, source->getURL()) == 0);

  TS_ASSERT(!source->canStreamsBeAddedDynamically());

  RefPointer<SourceFormat> format = source->getSourceFormat();
  TSM_ASSERT("format not set", format);
  TSM_ASSERT("Unexpected format", strcmp("flv", format->getName())==0);

  retval = source->close();
  TS_ASSERT(retval >= 0);

  // and make sure we don't crash after a close.
  {
    LoggerStack stack;
    // quiet ffmpeg error
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

    TS_ASSERT_THROWS_ANYTHING(source->getNumStreams());
    TS_ASSERT_THROWS_ANYTHING(source->getDuration());
    TS_ASSERT_THROWS_ANYTHING(source->getBitRate());
    TS_ASSERT_THROWS_ANYTHING(source->getFileSize());
    TS_ASSERT_THROWS_ANYTHING(source->getStartTime());
    TS_ASSERT_THROWS_ANYTHING(source->getFlags());
    TS_ASSERT_THROWS_ANYTHING(source->getURL());
  }
}

void
SourceTest::testOpenDemuxerPrivatePropertySetting()
{
  RefPointer<Source> source = Source::make();
  // create a bag of options to pass in for an FLV input format.
  RefPointer<KeyValueBag> inputOptions = KeyValueBag::make();
  RefPointer<KeyValueBag> outputOptions = KeyValueBag::make();

  inputOptions->setValue("flv_metadata", "1");
  const char* INVALID_OPTION="not-a-valid-flv-private-option";
  const char* INVALID_VALUE="23828";
  inputOptions->setValue(INVALID_OPTION, INVALID_VALUE);
  
  const char* file = mSampleFile;

  int32_t r = source->open(file, 0, false, false, inputOptions.value(), 
      outputOptions.value());
  TS_ASSERT(r >= 0);
  TS_ASSERT(source->getState() == Container::STATE_OPENED);

  TS_ASSERT_EQUALS(1, outputOptions->getNumKeys());
  TSM_ASSERT("Expected option missing", outputOptions->getValue(INVALID_OPTION,
        KeyValueBag::KVB_NONE));

  r = source->close();
  TS_ASSERT(r >= 0);
}
void
SourceTest::testOpenResetInputFormat()
{
  RefPointer<SourceFormat> format = 0;
  RefPointer<Source> source = Source::make();
  TS_ASSERT(source);
  const char*file = mSampleFile;

  format = source->getSourceFormat();
  TS_ASSERT(!format);
  format = SourceFormat::findFormat("mp4");
  {
    LoggerStack stack;
    // quiet ffmpeg error
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);
    int32_t retval = source->open(file, format.value(), false, false, 0, 0);
    // and this should fail since the file is an FLV
    TS_ASSERT(retval < 0);
  }
}

void
SourceTest::testOpenCustomIO()
{
  StdioURLProtocolManager::registerProtocol("test");
  char url[2048];
  snprintf(url, sizeof(url), "test:%s", mSampleFile);

  // now, try open and close.
  openTestHelper(url);
}

void
SourceTest::testOpenWithoutCloseAutoCloses()
{
  LoggerStack stack;
  // quiet Source error when Source is destroyed
  stack.setGlobalLevel(Logger::LEVEL_ERROR, false);
  RefPointer<Source> source = Source::make();
  int32_t retval = source->open(mSampleFile, 0, false, false, 0, 0);
  TS_ASSERT(retval >= 0);

  // now this test will only fail under a memory leak test, but
  // if a source is destroyed without closing, it should attempt
  // to auto-close.
}

void
SourceTest::testOpenInvalidArguments()
{
  int32_t retval = 0;

  // each sub-test will be wrapped in a block so
  // I can squelch logging once I see the right stuff
  // happen
  {
    RefPointer<Source> source = Source::make();
    LoggerStack stack;
    // quiet Source error when Source is destroyed
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);
    retval = source->open(0, 0, false, false, 0, 0);
    TS_ASSERT(retval < 0);
  }
  {
    RefPointer<Source> source = Source::make();
    LoggerStack stack;
    // quiet Source error when Source is destroyed
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);
    retval = source->open("/foo/bar/ohsonotalidfile", 0, false, false, 0, 0);
    TS_ASSERT(retval < 0);
  }
}

void
SourceTest::testRead()
{
  RefPointer<Source> source = Source::make();
  TS_ASSERT(source);

  int32_t retval = source->open(mSampleFile, 0, false, true, 0, 0);
  TS_ASSERT(retval >= 0);
  TS_ASSERT(source->getState() == Container::STATE_OPENED);

  int64_t pktsRead = 0;
  RefPointer<MediaPacket> pkt = MediaPacket::make();
  do {
    retval = source->read(pkt.value());
    if (retval >= 0)
      ++pktsRead;
  } while (retval >= 0);
  TS_ASSERT(pktsRead > 0);
  // I happen to know that there are this number of audio and video packets in this file.
  TS_ASSERT_EQUALS(pktsRead, mFixture->packets);
  retval = source->close();
  TS_ASSERT(retval >= 0);
}
