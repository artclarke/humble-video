/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef BUFFERIMPL_H_
#define BUFFERIMPL_H_

#include <io/humble/ferry/Buffer.h>

namespace io { namespace humble { namespace ferry
{

  class BufferImpl : public Buffer
  {
    VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(BufferImpl);
  public:
    
    virtual void* getBytes(int32_t offset, int32_t length);
    virtual int32_t getBufferSize();

    /**
     * Allocate a new buffer of at least bufferSize.
     */
    static VS_API_FERRY BufferImpl* make(RefCounted* requestor,
        int32_t bufferSize);
    
    /**
     * Create an iBuffer that wraps the given buffer, and calls
     * FreeFunc(buffer, closure) on it when we believe it's safe
     * to destruct it.
     */
    static VS_API_FERRY BufferImpl* make(RefCounted* requestor, void * bufToWrap, int32_t bufferSize,
     FreeFunc freeFunc, void * closure);
    
    virtual Type getType();
    virtual void setType(Type);
    virtual int32_t getSize(); 

    static BufferImpl*
    make(io::humble::ferry::RefCounted* requestor,
        Type type, int32_t numElements, bool zero);
    
    static int32_t getTypeSize(Type type);
  protected:
    BufferImpl();
    virtual ~BufferImpl();
  private:
    void* mBuffer;
    FreeFunc mFreeFunc;
    void* mClosure;
    int32_t mBufferSize;
    bool mInternallyAllocated;
    Type mType;
    static uint8_t mTypeSize[];
  };

}}}

#endif /*BUFFERIMPL_H_*/
