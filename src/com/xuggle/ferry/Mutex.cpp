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

#include "Mutex.h"
#include "JNIHelper.h"
#include <cstdio>

namespace com
{
namespace xuggle
{
namespace ferry
{

jclass Mutex :: mClass = 0;
jmethodID Mutex :: mConstructorMethod = 0;

bool Mutex :: mInitialized = false;

Mutex :: Mutex()
{
  mLock = 0;
  mSpinCount = 0;
}

Mutex :: ~Mutex()
{
  JNIEnv *env = JNIHelper::sGetEnv();
  if (env)
  {
    if (mSpinCount > 0)
    {
      // OK, this is bad; someone left us in a bad
      // condition
      fprintf(stderr, "Destroying monitor %p with non-zero spin count\n",
          this);
      while (mSpinCount > 0)
      {
        this->unlock();
      }
    }
    if (mLock)
      env->DeleteGlobalRef(mLock);
  }
  mLock = 0;
}

bool
Mutex :: init()
{
  if (!mInitialized)
  {
    JNIHelper::sRegisterInitializationCallback(initJavaBindings, 0);
    mInitialized = true;
  }
  return mInitialized;
}

void
Mutex :: initJavaBindings(JavaVM*, void*)
{
  JNIEnv *env = JNIHelper::sGetEnv();
  if (env && !mClass)
  {
    // We're inside a JVM, let's get to work.
    jclass cls = env->FindClass("java/lang/Object");
    if (cls)
    {
      // and find all our methods

      mConstructorMethod = env->GetMethodID(cls, "<init>", "()V");

      // keep a reference around
      mClass = (jclass) env->NewWeakGlobalRef(cls);
    }
  }
}

void
Mutex :: lock()
{
  if (!mInitialized)
    Mutex::init();

  if (mLock)
  {
    JNIEnv *env = JNIHelper::sGetEnv();
    if (env)
    {
      //fprintf(stderr, " PRE-ENTER: %p\n", mLock);
      if (env->ExceptionCheck())
      {
        // have a pending java exception; don't try the lock
        throw std::runtime_error("pending java exception; not locking");
      }
      // testing that a thrown ERROR doesn't cause core-dumps
      // throw std::runtime_error("bad panda; no shoots for you");
      
      if (env->MonitorEnter(mLock) != JNI_OK)
      {
        fprintf(stderr, "Could not enter lock: %p\n", mLock);
        throw std::runtime_error("failed to enter monitor; not locking");
      }
      else
      {
        if (env->ExceptionCheck()) {
          env->MonitorExit(mLock);
          throw std::runtime_error("failed to enter monitor due to "
              "pending exception; not locking");
        } else {
          // we're in the lock now!
          ++mSpinCount;
        }
      }
      //fprintf(stderr, "POST-ENTER: %p\n", mLock);
    }
  }
}

void
Mutex :: unlock()
{
  if (!mInitialized)
    Mutex::init();

  if (mLock)
  {
    JNIEnv *env = JNIHelper::sGetEnv();
    if (env)
    {
      //fprintf(stderr, "  PRE-EXIT: %p\n", mLock);
      if (mSpinCount <= 0)
      {
        // we called unlock without a matching successful lock;
        // we must fail.
        throw std::runtime_error("unlock attempt on unlocked mutex");
      }
      // reduce the spin count regardless of result while still under
      // the lock
      --mSpinCount;
      
      if (env->MonitorExit(mLock) != JNI_OK)
      {
        // this can fail in out of memory situations but there's not much
        // we can do.
        //fprintf(stderr, "Could not exit lock: %p\n", mLock);
        throw std::runtime_error("failed attempt to unlock mutex");
      }
      //fprintf(stderr, " POST-EXIT: %p\n", mLock);
    }
  }
}

Mutex*
Mutex :: make()
{
  Mutex* retval = 0;
  jobject newValue = 0;
  JNIEnv *env = 0;

  try
  {
    if (!mInitialized)
      Mutex::init();

    env = JNIHelper::sGetEnv();
    if (env)
    {
      if (!mClass)
      {
        // we're being called BEFORE our initialization
        // callback was called, but during someone
        // elses initialization route, so we can be
        // sure we have a JVM.
        Mutex::initJavaBindings(JNIHelper::sGetVM(), 0);
      }
      if (mClass && mConstructorMethod)
      {
        retval = new Mutex();
        if (!retval)
          throw std::bad_alloc();
        retval->acquire();

        if (env->ExceptionCheck())
          throw std::bad_alloc();
        
        // we're running within Java
        newValue = env->NewObject(mClass, mConstructorMethod);
        if (!newValue)
          throw std::bad_alloc();
        
        if (env->ExceptionCheck())
          throw std::bad_alloc();
        
        retval->mLock = env->NewGlobalRef(newValue);
        
        if (!retval->mLock)
          throw std::bad_alloc();
        
        if (env->ExceptionCheck())
          throw std::bad_alloc();
        env->DeleteLocalRef(newValue);
        newValue = 0;
      }
    }
  }
  catch (std::bad_alloc & e)
  {
    VS_REF_RELEASE(retval);
    throw e;
  }
  catch (std::exception &e)
  {
    fprintf(stderr, "got uncaught error creating mutex: %p", retval);
    VS_REF_RELEASE(retval);
  }
  if (env && newValue)
    env->DeleteLocalRef(newValue);
  return retval;
}
}
}
}
