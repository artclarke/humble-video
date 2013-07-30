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
#include <io/humble/ferry/Buffer.h>
#include <io/humble/ferry/BufferImpl.h>

#include <stdexcept>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace ferry
{

  Buffer :: Buffer()
  {
  }

  Buffer :: ~Buffer()
  {
  }
  
  Buffer*
  Buffer :: make(io::humble::ferry::RefCounted* requestor, int32_t bufferSize)
  {
    return BufferImpl::make(requestor, bufferSize);
  }
  
  Buffer*
  Buffer :: make(RefCounted* requestor, void * bufToWrap,
      int32_t bufferSize,
      FreeFunc freeFunc,
      void * closure)
  {
    return BufferImpl::make(requestor, bufToWrap, bufferSize, freeFunc, closure);
  }
  

  Buffer*
  Buffer :: make(io::humble::ferry::RefCounted* requestor,
      Type type, int32_t numElements, bool zero)
  {
    VS_LOG_TRACE("making a buffer");
    return BufferImpl::make(requestor, type, numElements, zero);
  }
  
  int32_t
  Buffer :: getTypeSize(Type type)
  {
    return BufferImpl::getTypeSize(type);
  }

}}}
