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
 * VideoExceptions.h
 *
 *  Created on: Jul 31, 2013
 *      Author: aclarke
 */

#ifndef VIDEOEXCEPTIONS_H_
#define VIDEOEXCEPTIONS_H_

#include <io/humble/ferry/HumbleException.h>
#include <cstring>
#include <cstdlib>
namespace io {
namespace humble {
namespace video {

/**
 * Creates the definition of a video exception. You must place a call
 * to VS_EXCEPTION_MAKE_MAKER in a C++ file somewhere to create the ::make(...) method.
 */
#define VS_VIDEO_EXCEPTION_DEF(__class) \
    class __class : public virtual io::humble::ferry::HumbleRuntimeError \
    { \
    public: \
      explicit \
      __class(const std::string & msg) : std::runtime_error(msg), io::humble::ferry::HumbleRuntimeError(msg) {}; \
      static __class make(const char* format, ...); \
      virtual \
      ~__class() throw () {}; \
      virtual void \
      raise() const { \
        throw *this; \
      } \
    }

/**
 * Thrown by {@link Configurable} classes if a named property
 * is not found.
 */
VS_VIDEO_EXCEPTION_DEF(PropertyNotFoundException);

/**
 * Thrown by {@link Configurable} classes if a named property
 * is not found.
 */
class FfmpegException : public virtual io::humble::ferry::HumbleRuntimeError
{
public:
  explicit
  FfmpegException(const std::string & msg) : std::runtime_error(msg), io::humble::ferry::HumbleRuntimeError(msg) {};
  static FfmpegException make(const char* format, ...);
  /**
   * Checks if errorCode <= 0 and if so, throws the appropriate exception for it.
   */
  static void check(int32_t errorCode, const char* format, ...);
  virtual
  ~FfmpegException() throw () {};
  virtual void
  raise() const {
    throw *this;
  }
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* VIDEOEXCEPTIONS_H_ */
