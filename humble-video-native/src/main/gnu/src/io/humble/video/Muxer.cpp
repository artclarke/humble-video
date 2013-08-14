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

#include <io/humble/ferry/Logger.h>

#include "Muxer.h"
#include "VideoExceptions.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Muxer::Muxer(MuxerFormat* format, const char* filename,
    const char* formatName) {
  mState = STATE_INITED;

  mCtx = 0;
  int e = avformat_alloc_output_context2(&mCtx, format->getCtx(), formatName,
      filename);
  FfmpegException::check(e, "could not allocate output context");
  if (!mCtx) {
    VS_THROW(HumbleBadAlloc());
  }
  if (!filename || !*filename)
    mCtx->filename[0] = 0;
  mCtx->interrupt_callback.callback = Global::avioInterruptCB;
  mCtx->interrupt_callback.opaque = this;
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
Muxer::open(KeyValueBag *aInputOptions, KeyValueBag* aOutputOptions) {
  (void) aInputOptions;
  (void) aOutputOptions;
}

void
Muxer::close() {

}

const char*
Muxer::getURL() {
  return this->getFormatCtx()->filename;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
