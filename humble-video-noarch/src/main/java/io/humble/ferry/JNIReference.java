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

package io.humble.ferry;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicLong;

import io.humble.ferry.FerryJNI;

/**
 * Returned by {@link Buffer#getByteBuffer(int, int, java.util.concurrent.atomic.AtomicReference)}
 * for users that want to explicitly manage when the returned {@link java.nio.ByteBuffer}
 * is released.
 * <p>
 * This class creates a {@link WeakReference} that Ferry classes will use for
 * memory management. We do this to avoid relying on Java's finalizer thread to
 * keep up and instead make every new native allocation first release any
 * unreachable objects.
 * </p><p>
 * Most times these objects are managed behind the scenes when you
 * call {@link RefCounted#delete()}.  But when we return
 * {@link java.nio.ByteBuffer} objects, there is no equivalent of
 * delete(), so this object can be used if you want to explicitly control
 * when the {@link ByteBuffer}'s underlying native memory is freed.
 * </p>
 * 
 */
public final class JNIReference extends WeakReference<Object>
{
  private final AtomicLong mSwigCPtr = new AtomicLong(0);

  // This memory manager will outlive the Java object we're referencing; that
  // means this class will sometimes show up as a potential leak, but trust us
  // here; ignore all the refs to here and see who else is holding the ref to
  // the JNIMemoryAllocator and that's your likely leak culprit (if we didn't
  // do this, then you'd get no indication of who is leaking your native object
  // so stop complaining now).
  private volatile JNIMemoryAllocator mMemAllocator;

  private final boolean mIsFerryObject;

  private final AtomicLong mJavaRefCount;
  
  // A static memory allocator that is only constructed once
  private static final JNIMemoryAllocator sMemAllocator = new JNIMemoryAllocator();
  private final boolean sUseStaticMemAllocator = true;
  
  // Only turn this to true if you need memory debugging on.  Given how
  // hot this code is, remembering debug info is an unnecessary extra
  // step.
  private static volatile boolean mMemoryDebugging = false;
  /* package */ static void setMemoryDebugging(boolean value) {
    mMemoryDebugging = value;
  }
  /* package */ static boolean isMemoryDebugging() {
    return mMemoryDebugging; 
  }
  private static class DebugInfo {
    private final int mHashCode;
    private final Class<? extends Object> mClass;
    public DebugInfo(Object aObject)
    {
      mClass = aObject.getClass();
      mHashCode = aObject.hashCode();
    }
    public Class<? extends Object> getObjectClass()
    {
      return mClass;
    }
    public int getObjectHashCode()
    {
      return mHashCode;
    }
  }
  final private DebugInfo mDebugInfo;
  private JNIReference(
      final Object proxy,
      final Object aReferent,
      final long nativeVal,
      final boolean isFerry,
      final AtomicLong javaRefCount)
  {
    super(aReferent, JNIMemoryManager.getMgr().getQueue());
    mIsFerryObject = isFerry;
    mJavaRefCount = javaRefCount;
    mSwigCPtr.set(nativeVal);
    if (mMemoryDebugging) {
      mDebugInfo = new DebugInfo(proxy);
    } else {
      mDebugInfo = null;
    }
    JNIMemoryManager.MemoryModel model = JNIMemoryManager.getMemoryModel();
    if (model == JNIMemoryManager.MemoryModel.JAVA_DIRECT_BUFFERS ||
        model == JNIMemoryManager.MemoryModel.NATIVE_BUFFERS)
      // don't use our allocators
      return;
    if (mJavaRefCount.get() == 1 && 
        FerryJNI.RefCounted_getCurrentNativeRefCount(nativeVal, null) == 1)
    {
      // it's only safe to set the allocator if you're the only reference
      // holder. Otherwise
      // we default to having a null allocator, and allocations are anonymous,
      // but at least
      // won't crash under heavy multi-threaded situations.
      if (sUseStaticMemAllocator)
        mMemAllocator = sMemAllocator;
      else
        mMemAllocator = new JNIMemoryAllocator();
      // we are the only owner of this object; tell it we're the object it can
      // allocate from
      JNIMemoryAllocator.setAllocator(nativeVal, mMemAllocator);
    }
    else
    {
      // This creates a Strong reference to the allocator currently being used
      // so that if the Java proxy object the native object depended upon goes
      // away, the memory manager it was using stays around until this reference
      // is killed
      mMemAllocator = JNIMemoryAllocator.getAllocator(nativeVal);
    }
  }

  /**
   * Returns the {@link JNIMemoryManager} we're using.
   * 
   * @return the manager
   */
  static JNIMemoryManager getMgr()
  {
    return JNIMemoryManager.getMgr();
  }

  static JNIReference createReference(Object proxy, 
      Object aReferent, long swigCPtr,
      boolean isFerry, AtomicLong javaRefCount)
  {
    // Clear out any pending native objects
    JNIMemoryManager.getMgr().gcInternal();

    JNIReference ref = new JNIReference(
        proxy, aReferent,
        swigCPtr, isFerry, javaRefCount);
    JNIMemoryManager.getMgr().addReference(ref);
    //System.err.println("added  : "+ref+"; "+swigCPtr+" ("+isFerry+")");
    return ref;
  }
  static JNIReference createReference(Object proxy,
      Object aReferent, long swigCPtr,
      AtomicLong javaRefCount)
  {
    return createReference(proxy, aReferent, swigCPtr, true, javaRefCount);
  }
  static JNIReference createNonFerryReference(
      Object proxy, Object aReferent,
      long swigCPtr, AtomicLong javaRefCount)
  {
    return createReference(proxy, aReferent, swigCPtr, false, javaRefCount);
  }


  /**
   * Explicitly deletes the underlying native storage used by
   * the object this object references.  The underlying native
   * object is now no long valid, and attempts to use it could
   * cause unspecified behavior.
   * 
   */
  public void delete()
  {
    // acquire lock for minimum time
    final long swigPtr = mSwigCPtr.getAndSet(0);
    if (swigPtr != 0)
    {
      if (mJavaRefCount.decrementAndGet() == 0)
      {
        // log.debug("deleting: {}; {}", this, mSwigCPtr);
        FerryJNI.RefCounted_release(swigPtr, null);
      }
      // Free the memory manager we use
      mMemAllocator = null;
    }

  }

  /**
   * Returns true if this object derived from {@link RefCounted}.
   * @return true if a ferry object.
   */
  boolean isFerryObject()
  {
    return mIsFerryObject;
  }
 
  /**
   * Returns true if the underlying reference has had {@link #delete()} called.
   * @return True if {@link #delete()} was called.
   */
  boolean isDeleted()
  {
    return mSwigCPtr.get() == 0;
  }
  /**
   * Creates a string representation of this reference.  If the underlying
   * object this reference points to has been deleted, then "native" will
   * be zero.  If {@link JNIMemoryManager#isMemoryDebugging()} is true,
   * then the class and hashcode of the object this reference points to (or
   * used to point to) is also printed.
   * <p>
   * If the reference still points to an actual object, we will also print
   * the contents of that object.  It may return "null" in which case the
   * underlying object is no longer reachable, but if native != 0, it means
   * it has not yet been collected by Ferry.
   * </p>
   */
  @Override
  public String toString()
  {
    final StringBuilder builder = new StringBuilder();
    builder.append(super.toString());
    builder.append("[");
    builder.append("native=").append(mSwigCPtr.get()).append(";");
    if (mDebugInfo != null) {
      builder.append("proxyClass=").append(mDebugInfo.getObjectClass().getCanonicalName()).append(";");
      builder.append("hashCode=").append(mDebugInfo.getObjectHashCode()).append(";");
    }
    builder.append("object=[").append(get()).append("];");
    builder.append("];");
    
    return builder.toString();
  }
}
