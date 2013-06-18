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

#include "AtomicInteger.h"
#include "JNIHelper.h"

namespace com { namespace xuggle { namespace ferry {
  jclass AtomicInteger::mClass = 0;
  jmethodID AtomicInteger::mConstructorMethod = 0;
  jmethodID AtomicInteger::mGetMethod = 0;
  jmethodID AtomicInteger::mSetMethod = 0;
  jmethodID AtomicInteger::mGetAndSetMethod = 0;
  jmethodID AtomicInteger::mGetAndIncrementMethod = 0;
  jmethodID AtomicInteger::mGetAndDecrementMethod = 0;
  jmethodID AtomicInteger::mGetAndAddMethod = 0;
  jmethodID AtomicInteger::mIncrementAndGetMethod = 0;
  jmethodID AtomicInteger::mDecrementAndGetMethod = 0;
  jmethodID AtomicInteger::mAddAndGetMethod = 0;
  jmethodID AtomicInteger::mCompareAndSetMethod = 0;

  // static initialization
  bool AtomicInteger::mInitialized = false;

  bool
  AtomicInteger::init()
  {
    // have the JNIHelper initialize us once it gets a VM
    if (!mInitialized)
    {
      JNIHelper::sRegisterInitializationCallback(initializeClass, 0);
      mInitialized = true;
    }
    return mInitialized;
  }
  void
  AtomicInteger::initializeClass(JavaVM*, void*)
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    if (env && !mClass)
    {
      // We're inside a JVM, let's get to work.
      jclass cls=env->FindClass("java/util/concurrent/atomic/AtomicInteger");
      if (cls)
      {
        // find our constructors
        mConstructorMethod = env->GetMethodID(cls,
            "<init>", "()V");
        // and find all our methods
        mGetMethod=env->GetMethodID(cls, "get", "()I");
        mSetMethod=env->GetMethodID(cls, "set", "(I)V");
        mGetAndSetMethod=env->GetMethodID(cls, "getAndSet", "(I)I");
        mCompareAndSetMethod=env->GetMethodID(cls,
            "compareAndSet", "(II)Z");
        mGetAndIncrementMethod=env->GetMethodID(cls,
            "getAndIncrement", "()I");
        mGetAndDecrementMethod=env->GetMethodID(cls,
            "getAndDecrement", "()I");
        mGetAndAddMethod=env->GetMethodID(cls,
            "getAndAdd", "(I)I");
        mIncrementAndGetMethod=env->GetMethodID(cls,
            "incrementAndGet", "()I");
        mDecrementAndGetMethod=env->GetMethodID(cls,
            "decrementAndGet", "()I");
        mAddAndGetMethod=env->GetMethodID(cls,
            "addAndGet", "(I)I");

        // keep a reference around
        mClass = (jclass)env->NewWeakGlobalRef(cls);
      }
    }
  }

  void
  AtomicInteger::initializeObject()
  {
    // we can actually use the JNI stuff.
    init();
    mAtomicValue = 0;
    mNonAtomicValue = 0;
    JNIEnv *env=JNIHelper::sGetEnv();
    if (env)
    {
      if (!mClass) {
        // we're being called by a JNIHelper
        // initialization callback, but before
        // our own. Go ahead and call us.
        AtomicInteger::initializeClass(JNIHelper::sGetVM(), 0);
      }
      if (mClass)
      {
        jobject newValue = env->NewObject(mClass, mConstructorMethod);
        if (newValue)
        {
          mAtomicValue = env->NewGlobalRef(newValue);
          env->DeleteLocalRef(newValue);
        }
      }
    }
  }

  AtomicInteger::AtomicInteger()
  {
    initializeObject();
  }

  AtomicInteger::AtomicInteger(int32_t val)
  {
    mNonAtomicValue = 0;
    mAtomicValue = 0;
    initializeObject();
    this->set(val);
  }

  AtomicInteger::~AtomicInteger()
  {
    if (mAtomicValue)
    {
      JNIEnv *env=JNIHelper::sGetEnv();
      if (env)
      {
        env->DeleteGlobalRef(mAtomicValue);
        mAtomicValue = 0;
      }
    }
  }

  int32_t
  AtomicInteger::get()
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t val = 0;

    if (mAtomicValue && env)
      val = env->CallIntMethod(mAtomicValue,
          mGetMethod);
    else
      val = mNonAtomicValue;
    return val;
  }

  void
  AtomicInteger::set(int32_t newval)
  {
    JNIEnv *env=JNIHelper::sGetEnv();

    if (mAtomicValue && env)
      env->CallVoidMethod(mAtomicValue,
          mSetMethod, newval);
    else
      mNonAtomicValue=newval;
  }

  int32_t
  AtomicInteger::getAndSet(int32_t newval)
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mGetAndSetMethod, newval);
    else {
      retval = mNonAtomicValue;
      mNonAtomicValue = newval;
    }
    return retval;
  }

  int32_t
  AtomicInteger::getAndIncrement()
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mGetAndIncrementMethod);
    else
      retval = mNonAtomicValue++;
    return retval;
  }

  int32_t
  AtomicInteger::getAndDecrement()
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mGetAndDecrementMethod);
    else
      retval = mNonAtomicValue--;
    return retval;
  }

  int32_t
  AtomicInteger::getAndAdd(int32_t newval)
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mGetAndAddMethod, newval);
    else {
      retval = mNonAtomicValue;
      mNonAtomicValue += newval;

    }
    return retval;
  }

  int32_t
  AtomicInteger::incrementAndGet()
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mIncrementAndGetMethod);
    else
      retval = (++mNonAtomicValue);
    return retval;
  }

  int32_t
  AtomicInteger::decrementAndGet()
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mDecrementAndGetMethod);
    else
      retval = (--mNonAtomicValue);
    return retval;
  }

  int32_t
  AtomicInteger::addAndGet(int32_t newval)
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    int32_t retval = 0;

    if (mAtomicValue && env)
      retval = env->CallIntMethod(mAtomicValue,
          mAddAndGetMethod, newval);
    else {
      mNonAtomicValue += newval;
      retval = mNonAtomicValue;
    }
    return retval;
  }

  bool
  AtomicInteger::compareAndSet(int32_t expected, int32_t update)
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    bool retval = false;

    if (mAtomicValue && env)
      retval = env->CallBooleanMethod(mAtomicValue,
          mCompareAndSetMethod, expected, update);
    else {
      retval = (mNonAtomicValue == expected);
      if (retval)
      {
        mNonAtomicValue = update;
      }
    }
    return retval;
  }

  bool
  AtomicInteger::isAtomic()
  {
    return init() && mAtomicValue != 0;
  }

}}}
