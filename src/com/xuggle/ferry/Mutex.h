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

#ifndef MUTEX_H_
#define MUTEX_H_

#include <com/xuggle/ferry/RefCounted.h>

namespace com { namespace xuggle { namespace ferry {

  /**
   * Internal Only.
   * <p>
   * This object exists so that Native code can get access to 
   * thread safe locking objects if they need it.
   * </p><p>
   * Implements a blocking Mutually-Exclusive lock
   * by wrapping a Java lock.
   * </p><p>
   * If not running inside Java, lock() and unlock()
   * are NO-OPs.
   * </p>
   */
  class VS_API_FERRY Mutex : public RefCounted
  {
  public:
    static Mutex * make();

    void lock();
    void unlock();
  protected:
    Mutex();
    virtual ~Mutex();
  private:
    jobject mLock;
    volatile int64_t mSpinCount;

    static bool init();
    static void initJavaBindings(JavaVM* vm, void* closure);
    static bool mInitialized;
    static jclass mClass;
    static jmethodID mConstructorMethod;


  };

}}}

#endif /*MUTEX_H_*/
