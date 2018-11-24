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

#include <cstring>
#include "KeyValueBagTest.h"

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Demuxer.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

KeyValueBagTest :: KeyValueBagTest()
{
  mSampleFile[0] =0;
  char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
  if (!fixtureDirectory) {
    TSM_ASSERT("no fixture dir", false);
    throw new std::runtime_error("Must define environment variable VS_TEST_FIXTUREDIR");
  }
  snprintf(mSampleFile, sizeof(mSampleFile), "%s/%s", fixtureDirectory, "testfile.mp3");
  VS_LOG_TRACE("Sample File: %s", mSampleFile);
}

KeyValueBagTest :: ~KeyValueBagTest()
{
}

void
KeyValueBagTest :: setUp()
{

}

void
KeyValueBagTest :: tearDown()
{

}

void
KeyValueBagTest :: testCreation()
{
  RefPointer<KeyValueBag> metaData = KeyValueBag::make();
  TSM_ASSERT_EQUALS("should be zero", metaData->getNumKeys(), 0);
  metaData->setValue("foo", "bar");
  TSM_ASSERT_EQUALS("should be zero", metaData->getNumKeys(), 1);
  const char* value = metaData->getValue("foo", KeyValueBag::KVB_NONE);
  TSM_ASSERT("should be bar", strcmp("bar", value)==0);
  TSM_ASSERT("should be empty", !metaData->getValue("none",
        KeyValueBag::KVB_NONE));
}

void
KeyValueBagTest :: testDemuxerGetMetaData()
{
  RefPointer<Demuxer> source=Demuxer::make();
  source->open(mSampleFile, 0, false, true, 0, 0);

  RefPointer<KeyValueBag> meta = source->getMetaData();
  TSM_ASSERT("got meta data", meta);
  int32_t numKeys = meta->getNumKeys();
  TSM_ASSERT_EQUALS("should be right", numKeys, 9);
  for(int32_t i = 0; i < numKeys; i++)
  {
    const char* key = meta->getKey(i);
    TSM_ASSERT("should be found", key);
    TSM_ASSERT("should be found", *key);
    const char* value = meta->getValue(key, KeyValueBag::KVB_NONE);
    TSM_ASSERT("should be found", value);
    TSM_ASSERT("should be found", *value);
  }
  source->close();
}

