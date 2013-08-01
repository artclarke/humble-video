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
 * PropertyNotFoundException.h
 *
 *  Created on: Jul 31, 2013
 *      Author: aclarke
 */

#ifndef PROPERTYNOTFOUNDEXCEPTION_H_
#define PROPERTYNOTFOUNDEXCEPTION_H_

#include <io/humble/ferry/HumbleException.h>
#include <cstring>
#include <cstdlib>
namespace io {
namespace humble {
namespace video {

class PropertyNotFoundException : public virtual io::humble::ferry::HumbleRuntimeError
{
public:
  PropertyNotFoundException(const std::string & message);
  /**
   * Raises a HumbleRuntimeError with a sprintf style string.
   * Does not allocate memory per se, but has a limit of 1k on
   * the message size.
   */
  static PropertyNotFoundException make(const char* format, ...);


  virtual
  ~PropertyNotFoundException() throw ();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* PROPERTYNOTFOUNDEXCEPTION_H_ */
