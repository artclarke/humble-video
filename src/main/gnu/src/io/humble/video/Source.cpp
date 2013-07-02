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

#include "Source.h"

#include <io/humble/ferry/Logger.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

Source::Source() {
  mCtx = avformat_alloc_context();
  if (!mCtx) {
    VS_LOG_ERROR("Could not allocate AVFormatContext");
    throw new std::bad_alloc();
  }
  mState = STATE_INITED;
}

Source::~Source() {
}

int32_t
Source::close() {
  return -1;
}
KeyValueBag*
Source::getMetaData() {
  return 0;
}
int64_t
Source::getFileSize() {
  int64_t retval = -1;

  if (!mCtx->pb || (mCtx->iformat && (mCtx->iformat->flags & AVFMT_NOFILE)))
    retval = 0;
  else {
    retval = avio_size(mCtx->pb);
    retval = FFMAX(0, retval);
  }
  return retval;
}
} /* namespace video */
} /* namespace humble */
} /* namespace io */
