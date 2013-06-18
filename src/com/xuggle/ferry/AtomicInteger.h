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

#ifndef ATOMICINTEGER_H_
#define ATOMICINTEGER_H_

#include <com/xuggle/ferry/JNIHelper.h>

namespace com { namespace xuggle { namespace ferry {
/**
 * Internal only.  Atomic Integer represents Integers than can be updated
 * atomically from native code.
 * <p>
 * This object is NOT meant to be called from Java (in fact, that'd be stupid
 * since you'd just be calling from native code back into Java).  It's
 * here so that native code inside a JVM can have access to portable
 * thread-safe objects.
 * </p>  
 * <p>
 * And that said, this method is really only Atomic if running inside a
 * Java JVM (or other virtual machine that can provide the functionality).
 * If running in a standalone C++ program there
 * is no current guarantee of Atomicity.
 * </p><p>
 * The object just forwards to the Java object:
 * {@link java.util.concurrent.atomic.AtomicInteger}
 * </p>
 */
class VS_API_FERRY AtomicInteger
  {
    public:
      AtomicInteger();
      AtomicInteger(int32_t);
      ~AtomicInteger();

      int32_t get();
      void set(int32_t);

      int32_t getAndSet(int32_t);
      int32_t getAndIncrement();
      int32_t getAndDecrement();
      int32_t getAndAdd(int32_t);
      int32_t incrementAndGet();
      int32_t decrementAndGet();
      int32_t addAndGet(int32_t);

      /**
       * Compare the current value to expected, and if
       * they are equal, set the current value to update.
       * @param expected the value expected
       * @param update the value to update to
       * @return true if equal
       */
      bool compareAndSet(int32_t expected, int32_t update);

      /**
       * @return true if we're actually able to guarantee
       * atomicity; false if we can't.
       */
      bool isAtomic();

      friend class JNIHelper;

    private:
      static bool init();

      int32_t mNonAtomicValue;
      jobject mAtomicValue;
      static void initializeClass(JavaVM*, void*);
      void initializeObject();
      static jclass mClass;
      static jmethodID mConstructorMethod;
      static jmethodID mGetMethod;
      static jmethodID mSetMethod;
      static jmethodID mGetAndSetMethod;
      static jmethodID mGetAndIncrementMethod;
      static jmethodID mGetAndDecrementMethod;
      static jmethodID mGetAndAddMethod;
      static jmethodID mIncrementAndGetMethod;
      static jmethodID mDecrementAndGetMethod;
      static jmethodID mAddAndGetMethod;
      static jmethodID mCompareAndSetMethod;

      static bool mInitialized;
  };
}}}
#endif // ! ATOMICINTEGER_H_

