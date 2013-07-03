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
 * SourceImpl.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: aclarke
 */
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/JNIHelper.h>
#include "SourceImpl.h"
#include "Global.h"
#include <io/humble/video/customio/URLProtocolManager.h>
#include "KeyValueBagImpl.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::video::customio;
using namespace io::humble::ferry;

extern "C"
{
/** Some static functions used by custom IO
 */
int
Container_url_read(void*h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_read(buf, size);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_read(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}
int
Container_url_write(void*h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_write(buf, size);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_write(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}

int64_t
Container_url_seek(void*h, int64_t position, int whence)
{
  int64_t retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_seek(position, whence);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_seek(%p, %lld) ==> %d", h, position,
      whence, retval);
  return retval;
}

}

namespace io {
namespace humble {
namespace video {

SourceImpl::SourceImpl() {
  mReadRetryMax = 1;
  mInputBufferLength = 0;
  mIOHandler = 0;
  mCtx = avformat_alloc_context();
  if (!mCtx) throw std::bad_alloc();
  // Set up thread interrupt capabilities
  mCtx->interrupt_callback.callback = Global::avioInterruptCB;
  mCtx->interrupt_callback.opaque = this;

  mState = Container::STATE_INITED;
}

SourceImpl::~SourceImpl() {
  if (mIOHandler) {
    if (mCtx->pb) av_freep(&mCtx->pb->buffer);
    av_freep(&mCtx->pb);
    delete mIOHandler;
    mIOHandler = 0;
  }
  avformat_free_context(mCtx);
}

int32_t
SourceImpl::open(const char *url, InputFormat* format,
    bool streamsCanBeAddedDynamically, bool queryMetaData,
    KeyValueBag* options, KeyValueBag* optionsNotSet) {
  (void) queryMetaData;
  (void) streamsCanBeAddedDynamically;

  int retval = -1;
  if (mState != Container::STATE_INITED) {
    VS_LOG_DEBUG("Open can only be called when container is in init state. Current state: %d", mState);
    return retval;
  }

  AVDictionary* tmp=0;

  if (format) {
    // acquire a long-lived reference
    mFormat.reset(format, true);
    mCtx->iformat = mFormat->getCtx();
  }

  // Let's check for custom IO
  mIOHandler = URLProtocolManager::findHandler(
         url,
         URLProtocolHandler::URL_RDONLY_MODE,
         0);

  if (mIOHandler) {
    if (mInputBufferLength <= 0)
      // default to 2k
      mInputBufferLength = 2048;
    // free and realloc the input buffer length
    uint8_t* buffer = (uint8_t*)av_malloc(mInputBufferLength);
    if (!buffer) {
      mState = Container::STATE_ERROR;
      return retval;
    }

    // we will allocate ourselves an io context;
    // ownership of buffer passes here.
    mCtx->pb = avio_alloc_context(
        buffer,
        mInputBufferLength,
        0,
        mIOHandler,
        Container_url_read,
        Container_url_write,
        Container_url_seek);
    if (!mCtx->pb) {
      av_free(buffer);
      mState = Container::STATE_ERROR;
      return retval;
    }
  }
  // Check for passed in options
  KeyValueBagImpl* realOpts = dynamic_cast<KeyValueBagImpl*>(options);
  if (realOpts)
    av_dict_copy(&tmp, realOpts->getDictionary(), 0);

  KeyValueBagImpl* realUnsetOpts = dynamic_cast<KeyValueBagImpl*>(optionsNotSet);
  if (realUnsetOpts)
    realUnsetOpts->copy(tmp);

  // Now call the real open method; this is done
  // in another function to ensure we clean up tmp
  // afterwards.
  retval = -1;
  VS_CHECK_INTERRUPT(retval, true);

  if (tmp)
    av_dict_free(&tmp);
  return retval;
}

SourceImpl*
SourceImpl::make() {
  return 0;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
