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

#include <cstring>

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/JNIMemoryManager.h>

#include <io/humble/ferry/BufferImpl.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace ferry
{
  uint8_t BufferImpl :: mTypeSize[] = {
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
  BufferImpl :: BufferImpl() : mBuffer(0), mBufferSize(0)
  {
    mFreeFunc = 0;
    mClosure = 0;
    mInternallyAllocated = false;
    mType = BUFFER_UINT8; // bytes
  }

  BufferImpl :: ~BufferImpl()
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
  BufferImpl :: getBytes(int32_t offset, int32_t length)
  {
    void* retval = 0;

    if (length == 0)
      length = mBufferSize - offset;

    if ((length > 0) && (length + offset <= mBufferSize))
      retval = ((unsigned char*) mBuffer)+offset;

    return retval;
  }

  int32_t
  BufferImpl :: getBufferSize()
  {
    return mBufferSize;
  }

  BufferImpl*
  BufferImpl :: make(io::humble::ferry::RefCounted* requestor, int32_t bufferSize)
  {
    BufferImpl* retval = 0;
    if (bufferSize <= 0)
      VS_THROW(HumbleInvalidArgument("bufferSize must be > 0"));
    
    void * allocator = requestor ? requestor->getJavaAllocator() : 0;
    void *buffer = JNIMemoryManager::malloc(allocator, bufferSize);
    if (!buffer)
      VS_THROW(HumbleBadAlloc());
      
    try {
      retval = BufferImpl::make();

      retval->mBuffer = buffer;
      retval->mBufferSize = bufferSize;
      retval->mInternallyAllocated = true;
    } catch (std::bad_alloc & e) {
      JNIMemoryManager::free(buffer);
      throw;
    }
    return retval;
  }

  BufferImpl*
  BufferImpl :: make(io::humble::ferry::RefCounted* /*unused*/, void *bufToWrap, int32_t bufferSize,
      FreeFunc freeFunc, void *closure)
  {
    BufferImpl * retval = 0;

    if (!bufToWrap)
      VS_THROW(HumbleInvalidArgument("bufToWrap must be non null"));

    if (bufferSize <= 0)
      VS_THROW(HumbleInvalidArgument("bufferSize must be > 0"));

    if (bufToWrap && bufferSize>0)
    {
      retval = BufferImpl::make();
      retval->mFreeFunc = freeFunc;
      retval->mClosure = closure;
      retval->mBufferSize = bufferSize;
      retval->mBuffer = bufToWrap;
      retval->mInternallyAllocated = false;
    }
    return retval;
  }
  
  Buffer::Type
  BufferImpl :: getType()
  {
    return mType;
  }
  
  void
  BufferImpl :: setType(Type type)
  {
    mType = type;
  }
  
  BufferImpl*
  BufferImpl :: make(io::humble::ferry::RefCounted* requestor,
      Type type, int32_t numElements, bool zero)
  {
    if (numElements <= 0)
      VS_THROW(HumbleInvalidArgument("numElements must be > 0"));
    if (type < 0 || type >= BUFFER_NB)
      VS_THROW(HumbleInvalidArgument("invalid type"));
    
    int32_t bytesRequested = numElements*mTypeSize[(int32_t)type];
    BufferImpl *retval = BufferImpl::make(requestor, bytesRequested);
    retval->mType = type;
    if (zero)
      memset(retval->getBytes(0, bytesRequested), 0, bytesRequested);
    return retval;
  }
  
  int32_t
  BufferImpl :: getTypeSize(Type type)
  {
    if (type < 0 || type >= BUFFER_NB)
      return 0;
    return mTypeSize[(int32_t)type];
  }
  
  int32_t
  BufferImpl :: getSize()
  {
    if (mType < 0 || mType >= BUFFER_NB)
      return 0;
    return getBufferSize()/mTypeSize[(int32_t)mType];
  }
}}}
