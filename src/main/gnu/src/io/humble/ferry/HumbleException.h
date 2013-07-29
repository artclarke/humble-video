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
 * HumbleException.h
 *
 *  A collection of exceptions that can be safely used in Humble Software.
 *
 *  On some compiles (ok, just GCC) on some operating systems (OK, just
 *  those with ELF binaries) it will also generate stack traces
 *  which can then sometimes make it back into Java.
 *
 *  Created on: Jul 14, 2013
 *      Author: aclarke
 */

#ifndef HUMBLEEXCEPTION_H_
#define HUMBLEEXCEPTION_H_

#include <stdexcept>
#include <io/humble/ferry/Ferry.h>

namespace io {
namespace humble {
namespace ferry {

class VS_API_FERRY HumbleStackTrace
{
public:
  int
  getNumFrames() const {
    return mNumFrames;
  }
  const char * const *
  getFrameDescriptions() const {
    return mSymbols;
  }
  const char*
  getFrameDescription(int i) const {
    return (mSymbols && i >= 0 && i < mNumFrames) ? mSymbols[i] : 0;
  }
  const void*
  getFrameAddress(int i) const {
    return (i >= 0 && i < mNumFrames) ? mFrames[i] : 0;
  }
  const void * const *
  getFrameAddresses() const {
    return mFrames;
  }

  /**
   * Each subclass must define this so we can
   * raise in place with true polymorphic support.
   */
  virtual void
  raise() const = 0;
  HumbleStackTrace(const HumbleStackTrace &);
protected:
  HumbleStackTrace();

  virtual
  ~HumbleStackTrace();

private:
  void *mFrames[256];
  char** mSymbols;
  int mNumFrames;
};

class VS_API_FERRY HumbleInvalidArgument : public virtual std::invalid_argument,
    public virtual HumbleStackTrace
{
public:
  explicit
  HumbleInvalidArgument(const std::string & arg) :
      std::invalid_argument(arg) {
  }
  virtual
  ~HumbleInvalidArgument() throw () {
  }
  virtual void
  raise() const {
    throw *this;
  }

};

class VS_API_FERRY HumbleRuntimeError : public virtual std::runtime_error,
    public virtual HumbleStackTrace
{
public:
  explicit
  HumbleRuntimeError(const std::string & arg) :
      std::runtime_error(arg) {
  }
  virtual
  ~HumbleRuntimeError() throw () {
  }
  /**
   * Raises a HumbleRuntimeError with a sprintf style string.
   * Does not allocate memory per se, but has a limit of 1k on
   * the message size.
   */
  static HumbleRuntimeError make(const char* format, ...);

  virtual void
  raise() const {
    throw *this;
  }
};

class VS_API_FERRY HumbleInterruptedException : public virtual HumbleRuntimeError
{
public:
  HumbleInterruptedException() :
      std::runtime_error("thread interrupted"), HumbleRuntimeError(
          "thread interrupted") {
  }
  virtual
  ~HumbleInterruptedException() throw () {
  }
  virtual void
  raise() const {
    throw *this;
  }
};

class VS_API_FERRY HumbleBadAlloc : public virtual std::bad_alloc,
    public virtual HumbleStackTrace
{
public:
  explicit
  HumbleBadAlloc() :
      std::bad_alloc() {
  }
  virtual
  ~HumbleBadAlloc() throw () {
  }
  virtual void
  raise() const {
    throw *this;
  }
};

} /* namespace ferry */
} /* namespace humble */
} /* namespace io */

#endif /* HUMBLEEXCEPTION_H_ */
