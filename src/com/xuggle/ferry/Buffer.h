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

#ifndef BUFFER_H_
#define BUFFER_H_

#include <com/xuggle/ferry/IBuffer.h>

namespace com { namespace xuggle { namespace ferry
{

  class Buffer : public IBuffer
  {
    VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(Buffer);
  public:
    
    virtual void* getBytes(int32_t offset, int32_t length);
    virtual int32_t getBufferSize();

    /**
     * Allocate a new buffer of at least bufferSize.
     */
    static VS_API_FERRY Buffer* make(RefCounted* requestor,
        int32_t bufferSize);
    
    /**
     * Create an iBuffer that wraps the given buffer, and calls
     * FreeFunc(buffer, closure) on it when we believe it's safe
     * to destruct it.
     */
    static VS_API_FERRY Buffer* make(RefCounted* requestor, void * bufToWrap, int32_t bufferSize,
     FreeFunc freeFunc, void * closure);
    
    virtual Type getType();
    virtual void setType(Type);
    virtual int32_t getSize(); 

    static Buffer*
    make(com::xuggle::ferry::RefCounted* requestor,
        Type type, int32_t numElements, bool zero);
    
    static int32_t getTypeSize(Type type);
  protected:
    Buffer();
    virtual ~Buffer();
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

#endif /*BUFFER_H_*/
