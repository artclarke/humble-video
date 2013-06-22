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

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/IBuffer.h>
#include <io/humble/ferry/Buffer.h>

#include <stdexcept>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace ferry
{

  IBuffer :: IBuffer()
  {
  }

  IBuffer :: ~IBuffer()
  {
  }
  
  IBuffer*
  IBuffer :: make(io::humble::ferry::RefCounted* requestor, int32_t bufferSize)
  {
    return Buffer::make(requestor, bufferSize);
  }
  
  IBuffer*
  IBuffer :: make(RefCounted* requestor, void * bufToWrap,
      int32_t bufferSize,
      FreeFunc freeFunc,
      void * closure)
  {
    return Buffer::make(requestor, bufToWrap, bufferSize, freeFunc, closure);
  }
  

  IBuffer*
  IBuffer :: make(io::humble::ferry::RefCounted* requestor,
      Type type, int32_t numElements, bool zero)
  {
    return Buffer::make(requestor, type, numElements, zero);
  }
  
  int32_t
  IBuffer :: getTypeSize(Type type)
  {
    return Buffer::getTypeSize(type);
  }

}}}
