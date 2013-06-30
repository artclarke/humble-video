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

#ifndef JAVAURLPROTOCOLHANDLER_H_
#define JAVAURLPROTOCOLHANDLER_H_

#include <io/humble/video/io/URLProtocolHandler.h>
#include <jni.h>

namespace io { namespace humble { namespace video { namespace io
  {
  class JavaURLProtocolManager;
  class VS_API_HUMBLE_VIDEO_IO JavaURLProtocolHandler : public URLProtocolHandler
  {
  public:
    JavaURLProtocolHandler(
        JavaURLProtocolManager* mgr,
        jobject aJavaProtocolHandler);
    virtual ~JavaURLProtocolHandler();

    // Now, let's have our forwarding functions
    int url_open(const char *url, int flags);
    int url_close();
    int url_read(unsigned char* buf, int size);
    int url_write(const unsigned char* buf, int size);
    int64_t url_seek(int64_t position, int whence);
    SeekableFlags url_seekflags(const char* url, int flags);

  private:
    void cacheJavaMethods(jobject aProtoHandler);
    jobject mJavaProtoHandler;
    jmethodID mJavaUrlOpen_mid;
    jmethodID mJavaUrlClose_mid;
    jmethodID mJavaUrlRead_mid;
    jmethodID mJavaUrlWrite_mid;
    jmethodID mJavaUrlSeek_mid;
    jmethodID mJavaUrlIsStreamed_mid;

  };
  }}}}
#endif /*JAVAURLPROTOCOLHANDLER_H_*/
