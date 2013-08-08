/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Container.h
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#include <stdexcept>
#include <cstring>

#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/ferry/Logger.h>

#include <io/humble/video/customio/StdioURLProtocolHandler.h>
#include <io/humble/video/customio/StdioURLProtocolManager.h>

#ifdef _WIN32
#    ifdef __MINGW32__
#        define fseeko fseeko64
#        define ftello ftello64
#    else
#        define fseeko _fseeki64
#        define ftello _ftelli64
#    endif
#endif

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video { namespace customio
{

StdioURLProtocolHandler :: StdioURLProtocolHandler(
    StdioURLProtocolManager* mgr) : URLProtocolHandler(mgr)
{
  mFile = 0;
}

StdioURLProtocolHandler :: ~StdioURLProtocolHandler()
{
  reset();
}

void
StdioURLProtocolHandler :: reset()
{
  (void) url_close();
}
int
StdioURLProtocolHandler :: url_open(const char *url, int flags)
{
  if (!url || !*url)
    return -1;
  reset();
  const char * mode;

  switch(flags) {
    case URLProtocolHandler::URL_RDONLY_MODE:
      mode="rb";
      break;
    case URLProtocolHandler::URL_WRONLY_MODE:
      mode="wb";
      break;
    case URLProtocolHandler::URL_RDWR_MODE:
      mode="rb+";
      break;
    default:
      return -1;
  }

  // The URL MAY contain a protocol string.  Find it now.
  char proto[256];
  const char* protocol = URLProtocolManager::parseProtocol(proto, sizeof(proto), url);
  if (protocol)
  {
    size_t protoLen = strlen(protocol);
    // skip past it
    url = url + protoLen;
    if (*url == ':' || *url == ',')
      ++url;
  }
//  fprintf(stderr, "protocol: %s; url: %s; mode: %s\n", protocol, url, mode);
  mFile = fopen(url, mode);
  if (!mFile)
    return -1;
  return 0;
}

int
StdioURLProtocolHandler :: url_close()
{
  int retval = -1;
  if (!mFile)
    return -1;
  retval = fclose(mFile);
  mFile = 0;
  return retval;
}

int64_t
StdioURLProtocolHandler :: url_seek(int64_t position,
    int whence)
{
  if (!mFile)
    return -1;

  int stdioWhence;
  switch(whence) {
    case SK_SEEK_SET:
      stdioWhence = SEEK_SET;
      break;
    case SK_SEEK_CUR:
      stdioWhence = SEEK_CUR;
      break;
    case SK_SEEK_END:
      stdioWhence = SEEK_END;
      break;
    case SK_SEEK_SIZE:
      return url_getsize();
      break;
    default:
      // fuckies
      return -1;
      break;
  }
  if(fseeko(mFile, (off_t)position, stdioWhence) < 0)
    return -1;
  else
    return position;
}

int64_t
StdioURLProtocolHandler :: url_getsize()
{
  int64_t retval = -1;
  if (!mFile)
    return -1;

  // cache the current position
  off_t curPos = ftello(mFile);

  // seek to end
  int r = fseeko(mFile, 0, SEEK_END);
  if (r >= 0) {
    retval = (int64_t) ftello(mFile);
  }

  // set back to old position
  (void) fseeko(mFile, curPos, SEEK_SET);
  return retval;
}

int
StdioURLProtocolHandler :: url_read(unsigned char* buf, int size)
{
  if (!mFile)
    return -1;
  return (int) fread(buf, 1, size, mFile);
}

int
StdioURLProtocolHandler :: url_write(const unsigned char* buf, int size)
{
  if (!mFile)
    return -1;
  return (int) fwrite(buf, 1, size, mFile);
}

URLProtocolHandler::SeekableFlags
StdioURLProtocolHandler :: url_seekflags( const char*, int)
{
  return URLProtocolHandler::SK_SEEKABLE_NORMAL;
}

}}}}
