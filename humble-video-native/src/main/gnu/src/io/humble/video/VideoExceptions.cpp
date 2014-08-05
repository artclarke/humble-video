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
 * PropertyNotFoundException.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/ferry/Logger.h>
#include "HumbleVideo.h"
#include "VideoExceptions.h"

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.Exception);

namespace io {
namespace humble {
namespace video {

VS_EXCEPTION_MAKE_MAKER(PropertyNotFoundException);
VS_EXCEPTION_MAKE_MAKER(FfmpegException);

void
FfmpegException::check(int32_t error, const char* fmt, ...) {
  if (error >= 0) {
    VS_CHECK_INTERRUPT(true);
    return;
  }

  const size_t bufLen = 1048;
  char buf[bufLen];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, bufLen, fmt, ap);
  va_end(ap);
  std::string msg = buf;

  const size_t ffmpegErrorStrLen = 512;
  char ffmpegErrorStr[ffmpegErrorStrLen];
  // get the error string
  av_make_error_string(ffmpegErrorStr, ffmpegErrorStrLen, error);

  const size_t finalStrLen = 1048;
  char finalStr[finalStrLen];

  snprintf(finalStr, finalStrLen, "%s: %s (%d)", buf, ffmpegErrorStr, error);

  switch (error)
  {
  case AVERROR_BSF_NOT_FOUND :
  case AVERROR_DECODER_NOT_FOUND :
  case AVERROR_DEMUXER_NOT_FOUND :
  case AVERROR_ENCODER_NOT_FOUND :
  case AVERROR_FILTER_NOT_FOUND :
  case AVERROR_MUXER_NOT_FOUND :
  case AVERROR_OPTION_NOT_FOUND :
  case AVERROR_PROTOCOL_NOT_FOUND :
  case AVERROR_STREAM_NOT_FOUND : {
    // for all NOT_FOUND use PropertyNotFound
    VS_THROW(PropertyNotFoundException(finalStr));
    break;
  }
  case AVERROR(EIO):
  case AVERROR_EOF :
  case AVERROR(ENOENT):
  case AVERROR(EPIPE): {
    // Use IOException
    VS_THROW(io::humble::ferry::HumbleIOException(finalStr));
    break;
  }
  case AVERROR(ENOMEM): {
    JNIHelper* helper = JNIHelper::getHelper();
    if (helper) helper->throwOutOfMemoryError();
    break;
  }
  case AVERROR(EAGAIN):
  case AVERROR(EINTR): {
    VS_THROW(HumbleInterruptedException(finalStr));
    break;
  }
  case AVERROR(EINVAL): {
    VS_THROW(HumbleInvalidArgument(finalStr));
    break;
  }
  case AVERROR_EXIT :
  case AVERROR_BUG :
  case AVERROR_BUFFER_TOO_SMALL :
  case AVERROR_EXTERNAL :
  case AVERROR_INVALIDDATA :
  case AVERROR_PATCHWELCOME :
  case AVERROR_BUG2 :
  case AVERROR_UNKNOWN :
  case AVERROR_EXPERIMENTAL:
  case AVERROR(EDOM):
  case AVERROR(EILSEQ):
  case AVERROR(ENOSYS):
  case AVERROR(ERANGE):
  default: {
    VS_THROW(FfmpegException::make(finalStr));
    break;
  }
  }
  // finally, check if we're just interrupted
  VS_CHECK_INTERRUPT(true);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
