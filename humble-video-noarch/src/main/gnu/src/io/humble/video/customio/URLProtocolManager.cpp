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

#include <exception>
#include <stdexcept>
#include <cstring>
#include <io/humble/video/customio/URLProtocolManager.h>

namespace io { namespace humble { namespace video { namespace customio
{
using namespace std;


static const char*
URLProtocolManager_VALID_PROTOCOL_CHARS =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789+-.";

const char *
URLProtocolManager::parseProtocol(
    char * dest,
    int32_t dest_len,
    const char* url)
{
  if (!dest || dest_len <=0 || !url || !*url)
    return 0;
  size_t url_len = strlen(url);
  if (url_len <= 1)
    return 0;

  size_t proto_len = strspn(url, URLProtocolManager_VALID_PROTOCOL_CHARS);
  if (proto_len <= 0)
    return 0;

  // file protocol
  if (url[proto_len] != ':' && url[proto_len] != ',') {
    return 0;
  }
  if (url[0] && url[1] == ':') {
    return 0;
  }
  if (proto_len >= (size_t)dest_len)
    // not enough space
    return 0;
  strncpy(dest, url, proto_len);
  dest[proto_len] = 0;

  return dest;
}

URLProtocolManager* URLProtocolManager :: mFirstProtocol = 0;

URLProtocolManager*
URLProtocolManager :: getNextProtocol(URLProtocolManager *prev)
{
  return prev ? prev->mNext : mFirstProtocol;
}

URLProtocolManager*
URLProtocolManager :: registerProtocol(URLProtocolManager* mgr)
{
  if (mgr) {
    URLProtocolManager**p;
    p = &mFirstProtocol;
    while(*p) p = &(*p)->mNext;
    *p = mgr;
  }
  return mgr;
}

int
URLProtocolManager :: unregisterAllProtocols()
{
  int total = 0;
  URLProtocolManager *p=mFirstProtocol;
  while (p) {
    URLProtocolManager* casualty = p;
    p = p->mNext;
    delete casualty;
    ++total;
  }
  mFirstProtocol=0;
  return total;
}

URLProtocolManager*
URLProtocolManager :: unregisterProtocol(URLProtocolManager * mgr)
{
  if (mgr) {
    URLProtocolManager **p;
    p = &mFirstProtocol;
    while(*p && *p != mgr)
      p = &(*p)->mNext;
    if (*p)
      *p = mgr->mNext;
    mgr->mNext = 0;
  }
  return mgr;
}

URLProtocolManager :: URLProtocolManager(
    const char * aProtocolName)
{
  if (aProtocolName == NULL || *aProtocolName == 0)
    throw invalid_argument("bad argument to protocol manager");
  size_t len = strlen(aProtocolName);
  size_t validChars = strspn(aProtocolName, URLProtocolManager_VALID_PROTOCOL_CHARS);
  if (validChars != len)
    throw invalid_argument("protocol name contains invalid characters");

  mProtoName = new char[len+1];
  strcpy(mProtoName, aProtocolName);
  mNext = 0;
}

URLProtocolManager :: ~URLProtocolManager()
{
  delete [] mProtoName;
}

URLProtocolManager*
URLProtocolManager :: findProtocol(const char* protocol,
    const char* url,
    int,
    URLProtocolManager *previous)
{
  char proto[256];
  if (!protocol || !*protocol) {
    // parse the protocol from the URL
    protocol = URLProtocolManager::parseProtocol(proto, sizeof(proto), url);
    if (!protocol || !*protocol)
      return 0;
  }
  URLProtocolManager *p = previous ? previous : mFirstProtocol;
  while(p && strcmp(protocol, p->mProtoName) != 0)
    p = p->mNext;
  return p;
}

URLProtocolHandler*
URLProtocolManager :: findHandler(
    const char* url,
    int flags,
    URLProtocolManager *previous)
{
  URLProtocolManager* mgr = findProtocol(0, url, flags, previous);
  return mgr ? mgr->getHandler(url, flags) : 0;
}

/*
 * Here lie static functions that forward to the right class function in the protocol manager
 */
int
URLProtocolManager :: url_open(void* h, const char* url, int flags)
{
  int retval = -1;

  try
  {
    URLProtocolHandler* handler = URLProtocolManager::getHandler(h);
    retval = handler->url_open(url, flags);
  }
  catch (...)
  {
    retval = -1;
  }
  return retval;
}

URLProtocolHandler *
URLProtocolManager :: getHandler(void *h)
{
  if (!h)
    throw invalid_argument("missing handle");
  return (URLProtocolHandler*)h;
}

int
URLProtocolManager :: url_close(void *h)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = URLProtocolManager::getHandler(h);
    retval = handler->url_close();
    // when we close that handler is no longer valid; you must re-open to re-use.
    delete handler;
  }
  catch (...)
  {
    retval = -1;
  }
  return retval;
}

int
URLProtocolManager :: url_read(void *h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = URLProtocolManager::getHandler(h);
    retval = handler->url_read(buf, size);
  }
  catch(...)
  {
    retval = -1;
  }
  return retval;
}

int
URLProtocolManager :: url_write(void *h, const unsigned char* buf,
    int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = URLProtocolManager::getHandler(h);
    retval = handler->url_write(buf, size);
  }
  catch(...)
  {
    retval = -1;
  }
  return retval;
}

int64_t
URLProtocolManager :: url_seek(void *h, int64_t position,
    int whence)
{
  int64_t retval = -1;
  try
  {
    URLProtocolHandler* handler = URLProtocolManager::getHandler(h);
    retval = handler->url_seek(position, whence);
  }
  catch(...)
  {
    retval = -1;
  }
  return retval;
}

URLProtocolHandler::SeekableFlags
URLProtocolManager :: url_seekflags(void *h, const char* url, int flags)
{
  URLProtocolHandler::SeekableFlags retval = URLProtocolHandler::SK_NOT_SEEKABLE;
  try
  {
    URLProtocolHandler* handler = URLProtocolManager::getHandler(h);
    retval = handler->url_seekflags(url, flags);
  }
  catch(...)
  {
  }
  return retval;
}
}}}}
