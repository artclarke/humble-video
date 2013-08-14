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
 * Muxer.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/customio/URLProtocolManager.h>

#include "Muxer.h"
#include "VideoExceptions.h"
#include "KeyValueBagImpl.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;
using namespace io::humble::video::customio;

namespace io {
namespace humble {
namespace video {

Muxer::Muxer(MuxerFormat* format, const char* filename,
    const char* formatName) {
  mState = STATE_INITED;
  mIOHandler = 0;
  mBufferLength = 2048;

  mCtx = 0;
  int e = avformat_alloc_output_context2(&mCtx, format ? format->getCtx() : 0, formatName,
      filename);
  FfmpegException::check(e, "could not allocate output context ");
  if (!mCtx) {
    VS_THROW(HumbleBadAlloc());
  }
  if (!filename || !*filename) mCtx->filename[0] = 0;
  mCtx->interrupt_callback.callback = Global::avioInterruptCB;
  mCtx->interrupt_callback.opaque = this;

  // now let's look at the output format; it should have been guessed.
  if (!format) {
    if (!mCtx->oformat) {
      avformat_free_context(mCtx);
      VS_THROW(
          HumbleRuntimeError::make(
              "could not determine format to use for muxer. Filename: %s. FormatName: %s",
              filename, formatName));
    }
    mFormat = MuxerFormat::make(mCtx->oformat);
  } else mFormat.reset(format, true);

  // determine if this format NEEDs a file.
  if (!mFormat->getFlag(ContainerFormat::NO_FILE)) {
    if (!*mCtx->filename) {
      avformat_free_context(mCtx);
      VS_THROW(
          HumbleRuntimeError::make(
              "No filename specified, but MuxerFormat needs a file. Filename: %s. FormatName: %s",
              filename, formatName));
    }
  }
}

Muxer::~Muxer() {
  if (mState == STATE_OPENED) {
    VS_LOG_ERROR(
        "Open Muxer destroyed without Muxer.close() being called. Closing anyway: %s",
        this->getURL());
    (void) this->close();
  }
  if (mCtx) avformat_free_context(mCtx);
}

Muxer*
Muxer::make(MuxerFormat *format, const char* filename, const char* formatName) {
  Global::init();

  RefPointer<Muxer> retval;

  if (!format) {
    if ((!filename || !*filename) && (!formatName && !*formatName)) {
      VS_THROW(HumbleInvalidArgument("cannot pass in all nulll parameters"));
    }
  }
  retval.reset(new Muxer(format, filename, formatName), true);
  return retval.get();
}

void
Muxer::setOutputBufferLength(int32_t size) {
  if (size <= 0)
  VS_THROW(HumbleInvalidArgument("size <= 0"));
  if (mState != STATE_INITED)
  VS_THROW(HumbleRuntimeError("Muxer object has already been opened"));
  mBufferLength = size;
}

int32_t
Muxer::getOutputBufferLength() {
  return mBufferLength;
}

void
Muxer::open(KeyValueBag *aInputOptions, KeyValueBag* aOutputOptions) {
  AVFormatContext* ctx = this->getFormatCtx();
  int retval = -1;
  if (mState != STATE_INITED) {
    VS_THROW(
        HumbleRuntimeError::make(
            "Open can only be called when container is in init state. Current state: %d",
            mState));
  }

  AVDictionary* tmp = 0;
  const char* url = ctx->filename;
  AVOutputFormat* fmt = mFormat ? mFormat->getCtx() : 0;


  // Let's check for custom IO
  mIOHandler = URLProtocolManager::findHandler(mCtx->filename,
      URLProtocolHandler::URL_WRONLY_MODE, 0);

  if (mIOHandler) {
    ctx->flags |= AVFMT_FLAG_CUSTOM_IO;
    // free and realloc the input buffer length
    uint8_t* buffer = (uint8_t*) av_malloc(mBufferLength);
    if (!buffer) {
      mState = STATE_ERROR;
      VS_THROW(HumbleBadAlloc());
    }

    // we will allocate ourselves an io context;
    // ownership of buffer passes here.
    ctx->pb = avio_alloc_context(buffer, mBufferLength, 1, mIOHandler,
        Container::url_read, Container::url_write, Container::url_seek);
    if (!ctx->pb) {
      av_free(buffer);
      mState = STATE_ERROR;
      VS_THROW(
          HumbleRuntimeError::make(
              "could not open url due to internal error: %s", url));
    }
  }
  // Check for passed in options
  KeyValueBagImpl* realOpts = dynamic_cast<KeyValueBagImpl*>(aInputOptions);
  if (realOpts) av_dict_copy(&tmp, realOpts->getDictionary(), 0);

  try {
    if (mIOHandler) {
      retval = mIOHandler->url_open(url, URLProtocolHandler::URL_WRONLY_MODE);
    } else if (!fmt || !(fmt->flags & AVFMT_NOFILE)) {
      retval = avio_open2(&ctx->pb, url, AVIO_FLAG_WRITE,
          &ctx->interrupt_callback, 0);
    }
    if (retval < 0) {
      mState = STATE_ERROR;
      FfmpegException::check(retval, "Error opening url: %s; ", url);
    }

    /* Write the stream header, if any. */
    retval = avformat_write_header(ctx, &tmp);
    if (retval < 0) {
      mState = STATE_ERROR;
      FfmpegException::check(retval, "Could not write header for url: %s. ", url);
    }
  } catch (std::exception & e) {
    if (tmp) av_dict_free(&tmp);
    throw;
  }
  KeyValueBagImpl* realUnsetOpts =
      dynamic_cast<KeyValueBagImpl*>(aOutputOptions);
  if (realUnsetOpts) realUnsetOpts->copy(tmp);
  if (tmp) av_dict_free(&tmp);

  mState = STATE_OPENED;
}

void
Muxer::close() {

  mState = STATE_CLOSED;
}

const char*
Muxer::getURL() {
  return this->getFormatCtx()->filename;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
