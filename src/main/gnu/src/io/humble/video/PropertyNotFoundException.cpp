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

#include "PropertyNotFoundException.h"

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

PropertyNotFoundException::PropertyNotFoundException(const std::string & msg)
: std::runtime_error(msg), HumbleRuntimeError(msg)
{
}

PropertyNotFoundException::~PropertyNotFoundException() throw () {
}

PropertyNotFoundException
PropertyNotFoundException::make(const char* fmt, ...) {
  const size_t bufLen=1048;
  char buf[bufLen];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, bufLen, fmt, ap);
  va_end(ap);
  std::string msg = buf;
  return PropertyNotFoundException(msg);
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
