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

%module Ferry
%{
#include <stdexcept>
#include <com/xuggle/ferry/JNIHelper.h>

extern "C" {
  SWIGEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *);
};

/*
 * This will be called if an when we're loaded
 * directly by Java.  If we're linked to via
 * C/C++ linkage on another library, they
 * must call sSetVM().
 */
SWIGEXPORT jint JNICALL
JNI_OnLoad(JavaVM *, void *)
{
  /* Because of static initialize in Mac OS, the only safe thing
   * to do here is return the version */
  return com::xuggle::ferry::JNIHelper::sGetJNIVersion();
}
#include <com/xuggle/ferry/RefCounted.h>
#include <com/xuggle/ferry/AtomicInteger.h>
#include <com/xuggle/ferry/Logger.h>
#include <com/xuggle/ferry/Mutex.h>
#include <com/xuggle/ferry/IBuffer.h>
#include <com/xuggle/ferry/RefCountedTester.h>

using namespace com::xuggle::ferry;

/**
 * Here to maintain BW-compatibility with Version 3.x of Xuggler;
 * can be removed when major version goes pass 3.
 */
extern "C" {
SWIGEXPORT jint JNICALL Java_com_xuggle_ferry_FerryJNI_RefCounted_1getCurrentNativeRefCount(JNIEnv *jenv, jclass jcls, jlong jarg1, jobject jarg1_);
SWIGEXPORT jint JNICALL Java_com_xuggle_ferry_FerryJNI_RefCounted_1getCurrentRefCount(JNIEnv *jenv, jclass jcls, jlong jarg1, jobject jarg1_)
{
  return Java_com_xuggle_ferry_FerryJNI_RefCounted_1getCurrentNativeRefCount(jenv, jcls, jarg1, jarg1_);
}


SWIGEXPORT void JNICALL
Java_com_xuggle_ferry_Ferry_init(JNIEnv *env, jclass)
{
  JavaVM* vm=0;
  if (!com::xuggle::ferry::JNIHelper::sGetVM()) {
    env->GetJavaVM(&vm);
    com::xuggle::ferry::JNIHelper::sSetVM(vm);
  }
}

}
%}
%pragma(java) jniclasscode=%{
  static {
    JNILibrary library = new JNILibrary("xuggle",
      new Long(com.xuggle.xuggler.Version.MAJOR_VERSION));
    JNILibrary.load("xuggle-xuggler", library);
    com.xuggle.ferry.Ferry.init();
    // This seems nuts, but it works around a Java 1.6 bug where
    // a race condition exists when JNI_NewDirectByteBuffer is called
    // from multiple threads.  See:
    // http://mail.openjdk.java.net/pipermail/hotspot-runtime-dev/2009-January/000382.html
    IBuffer buffer = IBuffer.make(null, 2);
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref
      = new java.util.concurrent.atomic.AtomicReference<JNIReference>(null);
    buffer.getByteBuffer(0,2, ref);
    JNIReference reference = ref.get();
    reference.delete();
    buffer.delete();
  }

  static void noop()
  {
  }
  /**
   * Internal Only.  Do not call.
   */
  public native static int getMemoryModel();
  /**
   * Internal Only.  Do not call.
   */
  public native static void setMemoryModel(int value);
  
%}
%pragma(java) moduleimports=%{
/**
 * Internal Only.
 * <p>
 * I meant that.
 * </p>
 * <p>
 * Really.  There's nothing here.
 * </p>
 */
%}
%pragma(java) modulecode=%{

  static {
    FerryJNI.noop();
  }
  
  /** call this to force a load of all native components.
   * This is NOT normally required but can be useful in
   * some circulstances.
   */
  public static void load()
  {
    
  }
  /**
   * Internal Only.  Do not use.
   */
  public native static void init();
  
  /**
   * Internal Only.  Do not use.
   *
   */
  public final static int release(long cptr)
  {
    return FerryJNI.RefCounted_release(cptr, (RefCounted)null);
  }
   
%}

%import <com/xuggle/ferry/JNIHelper.swg>

%include <com/xuggle/Xuggle.h>
%include <com/xuggle/ferry/Ferry.h>
%include <com/xuggle/ferry/AtomicInteger.h>
%include <com/xuggle/ferry/RefCounted.swg>
%include <com/xuggle/ferry/Logger.h>
%include <com/xuggle/ferry/Mutex.h>
%include <com/xuggle/ferry/IBuffer.swg>
%include <com/xuggle/ferry/RefCountedTester.h>

%typemap(javaimports) SWIGTYPE, SWIGTYPE*, SWIGTYPE& "import com.xuggle.ferry.*;"

