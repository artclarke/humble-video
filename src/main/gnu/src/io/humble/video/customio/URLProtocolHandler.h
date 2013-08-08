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

#ifndef URLPROTOCOLHANDLER_H_
#define URLPROTOCOLHANDLER_H_

#include <stdint.h>

#include <io/humble/video/customio/CustomIO.h>

namespace io { namespace humble { namespace video { namespace customio
{
  class URLProtocolManager;
  class VS_API_HUMBLE_VIDEO_CUSTOMIO URLProtocolHandler
  {
  public:
    typedef enum SeekFlags {
      SK_SEEK_SET=0,
      SK_SEEK_CUR=1,
      SK_SEEK_END=2,
      SK_SEEK_SIZE=0x10000,
    } SeekFlags;
    typedef enum OpenFlags {
      URL_RDONLY_MODE=0,
      URL_WRONLY_MODE=1,
      URL_RDWR_MODE=2,
    } OpenFlags;

    typedef enum SeekableFlags {
      SK_NOT_SEEKABLE   =0x00000000,
      SK_SEEKABLE_NORMAL=0x00000001,
    } SeekableFlags;
    virtual ~URLProtocolHandler();
    virtual const char* getProtocolName();
    virtual URLProtocolManager *getProtocolManager() { return mManager; }

    // Now, let's have our forwarding functions
    virtual int url_open(const char *url, int flags)=0;
    virtual int url_close()=0;
    virtual int url_read(unsigned char* buf, int size)=0;
    virtual int url_write(const unsigned char* buf, int size)=0;
    virtual int64_t url_seek(int64_t position, int whence)=0;
    virtual SeekableFlags url_seekflags(const char* url, int flags)=0;

  protected:
    URLProtocolHandler(
        URLProtocolManager* mgr);

  private:
    URLProtocolManager* mManager;
  };
}}}}
#endif /*URLPROTOCOLHANDLER_H_*/
