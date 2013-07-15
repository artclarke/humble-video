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
 * HumbleException.cpp
 *
 *  Created on: Jul 14, 2013
 *      Author: aclarke
 */

#ifdef __GNUC__
/** We are compiling in GCC */
#include <execinfo.h>
#endif // __GNUG__
#include "HumbleException.h"
#include <cstring>
#include <cstdlib>

namespace io {
namespace humble {
namespace ferry {

HumbleStackTrace::HumbleStackTrace() {
  mSymbols = 0;
  mNumFrames = 0;
  // time to generate a stack trace
#ifdef __GNUC__
  mNumFrames = backtrace(mFrames, sizeof(mFrames)/sizeof(*mFrames));
  if (mNumFrames > 0)
    mSymbols = backtrace_symbols(mFrames, mNumFrames);
#endif
}

HumbleStackTrace::HumbleStackTrace(const HumbleStackTrace & e) {
  mNumFrames = e.mNumFrames;
  memcpy(mFrames, e.mFrames, mNumFrames);
  if (e.mSymbols) {
    mSymbols = (char**)malloc(mNumFrames * sizeof(char*));
    if (mSymbols)
      memcpy(mSymbols, e.mSymbols, mNumFrames);
    // if there was a malloc error, just let it pass.
  }
}

HumbleStackTrace::~HumbleStackTrace() {
  if (mSymbols) {
    free(mSymbols);
  }
}


} /* namespace ferry */
} /* namespace humble */
} /* namespace io */
