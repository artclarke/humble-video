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
#include <cstring>
#include <cstdlib>

#include "StdioURLProtocolHandlerTest.h"

#include <io/humble/video/io/StdioURLProtocolManager.h>

using namespace io::humble::video::io;

VS_LOG_SETUP(VS_CPP_PACKAGE);

StdioURLProtocolHandlerTest :: StdioURLProtocolHandlerTest()
{
     char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
     if (fixtureDirectory)
       snprintf(mFixtureDir, sizeof(mFixtureDir), "%s", fixtureDirectory);
     else {
       VS_TUT_ENSURE("no fixture dir", false);
       throw new std::runtime_error("Must define environment variable VS_TEST_FIXTUREDIR");
     }
     FIXTURE_DIRECTORY = mFixtureDir;
     SAMPLE_FILE = "testfile.flv";
     snprintf(mSampleFile, sizeof(mSampleFile), "%s/%s", mFixtureDir, SAMPLE_FILE);
     VS_LOG_INFO("Sample File: %s", mSampleFile);
}

StdioURLProtocolHandlerTest :: ~StdioURLProtocolHandlerTest()
{
}

void
StdioURLProtocolHandlerTest :: setUp()
{
  
}

void
StdioURLProtocolHandlerTest :: tearDown()
{
  URLProtocolManager::unregisterAllProtocols();
}

void
StdioURLProtocolHandlerTest :: testCreation()
{
  StdioURLProtocolManager::registerProtocol("test");
  URLProtocolHandler* handler = StdioURLProtocolManager::findHandler("test:foo", 0,0);
  VS_TUT_ENSURE("", handler);
  delete handler;
}

void
StdioURLProtocolHandlerTest :: testOpenClose()
{
  StdioURLProtocolManager::registerProtocol("test");
  URLProtocolHandler* handler = StdioURLProtocolManager::findHandler("test:foo", 0,0);
  VS_TUT_ENSURE("", handler);

  int retval = 0;
  retval = handler->url_open(mSampleFile, URLProtocolHandler::URL_RDONLY_MODE);
  VS_TUT_ENSURE("", retval >= 0);

  retval = handler->url_close();
  VS_TUT_ENSURE("", retval >= 0);
  delete handler;
}

void
StdioURLProtocolHandlerTest :: testRead()
{
  StdioURLProtocolManager::registerProtocol("test");
  URLProtocolHandler* handler = StdioURLProtocolManager::findHandler("test:foo", 0,0);
  VS_TUT_ENSURE("", handler);

  int retval = 0;
  retval = handler->url_open(mSampleFile, URLProtocolHandler::URL_RDONLY_MODE);
  VS_TUT_ENSURE("", retval >= 0);

  int32_t totalBytes = 0;
  do {
    unsigned char buf[2048];
    retval = handler->url_read(buf, (int)sizeof(buf));
    if (retval > 0)
      totalBytes+= retval;
  } while (retval > 0);
  VS_TUT_ENSURE_EQUALS("", 4546420, totalBytes);

  retval = handler->url_close();
  VS_TUT_ENSURE("", retval >= 0);
  delete handler;
}

void
StdioURLProtocolHandlerTest :: testSeek()
{
  StdioURLProtocolManager::registerProtocol("test");
  URLProtocolHandler* handler = StdioURLProtocolManager::findHandler("test:foo", 0,0);
  VS_TUT_ENSURE("", handler);

  int retval = 0;
  retval = handler->url_open(mSampleFile, URLProtocolHandler::URL_RDONLY_MODE);
  VS_TUT_ENSURE("", retval >= 0);

  int64_t offset = 0;

  offset = handler->url_seek(0, URLProtocolHandler::SK_SEEK_SIZE);
  VS_TUT_ENSURE_EQUALS("", 4546420, offset);

  // now ensure we can read back all the data
  int32_t totalBytes = 0;
  do {
    unsigned char buf[2048];
    retval = handler->url_read(buf, (int)sizeof(buf));
    if (retval > 0)
      totalBytes+= retval;
  } while (retval > 0);
  VS_TUT_ENSURE_EQUALS("", 4546420, totalBytes);

  retval = handler->url_close();
  VS_TUT_ENSURE("", retval >= 0);
  delete handler;
}

void
StdioURLProtocolHandlerTest :: testSeekableFlags()
{
  StdioURLProtocolManager::registerProtocol("test");
  URLProtocolHandler* handler = StdioURLProtocolManager::findHandler("test:foo", 0,0);
  VS_TUT_ENSURE("", handler);
  URLProtocolHandler::SeekableFlags flags = handler->url_seekflags("test:foo", 0);
  VS_TUT_ENSURE_EQUALS("", URLProtocolHandler::SK_SEEKABLE_NORMAL, flags);
  delete handler;
}

void
StdioURLProtocolHandlerTest :: testReadWrite()
{
  StdioURLProtocolManager::registerProtocol("test");
  URLProtocolHandler* handler = StdioURLProtocolManager::findHandler("test:foo", 0,0);
  VS_TUT_ENSURE("", handler);
  const char* OUT_FILE="test:StdioURLProtocolHandlerTest_testReadWrite.flv";
  URLProtocolHandler* writeHandler = StdioURLProtocolManager::findHandler(OUT_FILE, 0, 0);
  VS_TUT_ENSURE("", writeHandler);

  int retval = 0;
  retval = handler->url_open(mSampleFile, URLProtocolHandler::URL_RDONLY_MODE);
  VS_TUT_ENSURE("", retval >= 0);

  retval = writeHandler->url_open(OUT_FILE, URLProtocolHandler::URL_WRONLY_MODE);
  VS_TUT_ENSURE("", retval >= 0);

  int32_t totalBytes = 0;
  do {
    int bytesToWrite;
    unsigned char buf[2048];
    retval = handler->url_read(buf, (int)sizeof(buf));
    if (retval > 0)
      totalBytes+= retval;
    bytesToWrite = retval;
    retval = writeHandler->url_write(buf, bytesToWrite);
    VS_TUT_ENSURE_EQUALS("", bytesToWrite, retval);
  } while (retval > 0);
  VS_TUT_ENSURE_EQUALS("", 4546420, totalBytes);

  retval = handler->url_close();
  VS_TUT_ENSURE("", retval >= 0);

  retval = writeHandler->url_close();
  VS_TUT_ENSURE("", retval >= 0);

  delete handler;
  delete writeHandler;
}

