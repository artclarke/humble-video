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
#include "SourceTest.h"
#include <io/humble/video/SourceImpl.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;
using namespace io::humble::video;

SourceTest::SourceTest() {
}

SourceTest::~SourceTest() {
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
  RefPointer<Source> source = Source::make();
  TS_ASSERT(source);
  char file[2048];
  const char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
// mov opening causes leak on mac os x, and I cannot
// track down why right now due to no internet access. ABC.
//  const char *sample = "testfile_h264_mp4a_tmcd.mov";
  const char *sample="testfile.flv";
  if (fixtureDirectory && *fixtureDirectory)
    snprintf(file, sizeof(file), "%s/%s", fixtureDirectory, sample);
  else
    snprintf(file, sizeof(file), "./%s", sample);

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
  TSM_ASSERT_EQUALS("Unexpected duration", 149264000, duration);
  
  int32_t filesize = (int32_t) source->getFileSize();
  TSM_ASSERT_EQUALS("Unexpected filesize", 4546420, filesize);

  TSM_ASSERT_EQUALS("Unexpected bitrate", 243671, source->getBitRate());

  TS_ASSERT(strcmp(file, source->getURL()) == 0);

  TS_ASSERT(!source->canStreamsBeAddedDynamically());

  RefPointer<InputFormat> format = source->getInputFormat();
  TSM_ASSERT("format not set", format);
  TSM_ASSERT("Unexpected format", strcmp("flv", format->getName())==0);

  retval = source->close();
  TS_ASSERT(retval >= 0);
}

void
SourceTest::testDemuxerPrivatePropertySetting()
{
  RefPointer<Source> source = Source::make();
  // create a bag of options to pass in for an FLV input format.
  RefPointer<KeyValueBag> inputOptions = KeyValueBag::make();
  RefPointer<KeyValueBag> outputOptions = KeyValueBag::make();

  inputOptions->setValue("flv_metadata", "1");
  const char* INVALID_OPTION="not-a-valid-flv-private-option";
  const char* INVALID_VALUE="23828";
  inputOptions->setValue(INVALID_OPTION, INVALID_VALUE);
  
  char file[2048];
  const char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
  const char *sample="testfile.flv";
  if (fixtureDirectory && *fixtureDirectory)
    snprintf(file, sizeof(file), "%s/%s", fixtureDirectory, sample);
  else
    snprintf(file, sizeof(file), "./%s", sample);

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
