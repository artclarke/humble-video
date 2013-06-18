/*******************************************************************************
 * Copyright (c) 2008, 2010 Xuggle Inc.  All rights reserved.
 *  
 * This file is part of Xuggle-Xuggler-Main.
 *
 * Xuggle-Xuggler-Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xuggle-Xuggler-Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Xuggle-Xuggler-Main.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include <cstring>

#include <com/xuggle/ferry/Logger.h>
#include <com/xuggle/ferry/JNIMemoryManager.h>

#include <com/xuggle/ferry/Buffer.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace com { namespace xuggle { namespace ferry
{
  uint8_t Buffer :: mTypeSize[] = {
      // Must be in right order
      sizeof(uint8_t),
      sizeof(int8_t),
      sizeof(uint16_t),
      sizeof(int16_t),
      sizeof(uint32_t),
      sizeof(int32_t),
      sizeof(uint64_t),
      sizeof(int64_t),
      sizeof(float),
      sizeof(double),
      0
  };
  Buffer :: Buffer() : mBuffer(0), mBufferSize(0)
  {
    mFreeFunc = 0;
    mClosure = 0;
    mInternallyAllocated = false;
    mType = IBUFFER_UINT8; // bytes
  }

  Buffer :: ~Buffer()
  {
    if (mBuffer)
    {
      VS_ASSERT(mBufferSize, "had buffer but no size");
      if (mInternallyAllocated)
        JNIMemoryManager::free(mBuffer);
      else if (mFreeFunc)
        mFreeFunc(mBuffer, mClosure);
      mBuffer = 0;
      mBufferSize = 0;
      mFreeFunc = 0;
      mClosure = 0;
    }
  }


  void*
  Buffer :: getBytes(int32_t offset, int32_t length)
  {
    void* retval = 0;

    if (length == 0)
      length = mBufferSize - offset;

    if ((length > 0) && (length + offset <= mBufferSize))
      retval = ((unsigned char*) mBuffer)+offset;

    return retval;
  }

  int32_t
  Buffer :: getBufferSize()
  {
    return mBufferSize;
  }

  Buffer*
  Buffer :: make(com::xuggle::ferry::RefCounted* requestor, int32_t bufferSize)
  {
    Buffer* retval = 0;
    if (bufferSize <= 0)
      return 0;
    
    void * allocator = requestor ? requestor->getJavaAllocator() : 0;
    void *buffer = JNIMemoryManager::malloc(allocator, bufferSize);
    if (!buffer)
      return 0;
      
    retval = Buffer::make();
    if (!retval) {
      JNIMemoryManager::free(buffer);
      return 0;
    }
    retval->mBuffer = buffer; 
    retval->mBufferSize = bufferSize;
    retval->mInternallyAllocated = true;
    return retval;
  }

  Buffer*
  Buffer :: make(com::xuggle::ferry::RefCounted* /*unused*/, void *bufToWrap, int32_t bufferSize,
      FreeFunc freeFunc, void *closure)
  {
    Buffer * retval = 0;
    
    if (bufToWrap && bufferSize>0)
    {
      retval = Buffer::make();
      if (retval)
      {
        retval->mFreeFunc = freeFunc;
        retval->mClosure = closure;
        retval->mBufferSize = bufferSize;
        retval->mBuffer = bufToWrap;
        retval->mInternallyAllocated = false;
      }
    }
    return retval;
  }
  
  IBuffer::Type
  Buffer :: getType()
  {
    return mType;
  }
  
  void
  Buffer :: setType(Type type)
  {
    mType = type;
  }
  
  Buffer*
  Buffer :: make(com::xuggle::ferry::RefCounted* requestor,
      Type type, int32_t numElements, bool zero)
  {
    if (numElements <= 0)
      return 0;
    if (type < 0 || type >= IBUFFER_NB)
      return 0;
    
    int32_t bytesRequested = numElements*mTypeSize[(int32_t)type];
    Buffer *retval = Buffer::make(requestor, bytesRequested);
    if (retval)
    {
      retval->mType = type;
      if (zero)
        memset(retval->getBytes(0, bytesRequested), 0, bytesRequested);
    }
    return retval;
  }
  
  int32_t
  Buffer :: getTypeSize(Type type)
  {
    if (type < 0 || type >= IBUFFER_NB)
      return 0;
    return mTypeSize[(int32_t)type];
  }
  
  int32_t
  Buffer :: getSize()
  {
    if (mType < 0 || mType >= IBUFFER_NB)
      return 0;
    return getBufferSize()/mTypeSize[(int32_t)mType];
  }
}}}
