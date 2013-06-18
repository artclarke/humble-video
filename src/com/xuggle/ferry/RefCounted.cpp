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

#include "RefCounted.h"
#include "AtomicInteger.h"
#include "JNIHelper.h"
//#include "Logger.h"

//VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace com { namespace xuggle { namespace ferry {

  RefCounted :: RefCounted()
  {
    mRefCount = new AtomicInteger(0);
    mAllocator = 0;
  }

  RefCounted :: ~RefCounted()
  {
    if (mRefCount)
      delete mRefCount;
    mRefCount = 0;
    if (mAllocator)
      JNIHelper::sDeleteGlobalRef((jobject)mAllocator);
    mAllocator = 0;
  }

  void
  RefCounted :: setJavaAllocator(void* allocator)
  {
    if (mAllocator)
      JNIHelper::sDeleteGlobalRef((jobject)mAllocator);
    if (allocator)
      mAllocator = JNIHelper::sNewGlobalRef((jobject)allocator);
    else
      mAllocator = 0;
  }
  
  void*
  RefCounted :: getJavaAllocator()
  {
    return mAllocator;
  }
  
  int32_t
  RefCounted :: acquire()
  {
    //VS_LOG_DEBUG("acquire: %p", this);
    return mRefCount->incrementAndGet();
  }

  int32_t
  RefCounted :: release()
  {
    //VS_LOG_DEBUG("release: %p", this);
    int32_t retval = mRefCount->decrementAndGet();
    if (!retval)
      this->destroy();
    return retval;
  }

  int32_t
  RefCounted :: getCurrentRefCount()
  {
    return mRefCount->get();
  }

  void
  RefCounted :: destroy()
  {
    //VS_LOG_DEBUG("destroy: %p", this);
    // by default just call the destructor
    delete this;
  }

  RefCounted*
  RefCounted :: copyReference()
  {
    this->acquire();
    return this;
  }

}}}
