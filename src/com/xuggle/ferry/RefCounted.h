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

#ifndef REFCOUNTED_H_
#define REFCOUNTED_H_

// For size_t
#include <new>

// For int32_t
#include <inttypes.h>

// for std::bad_alloc
#include <stdexcept>

#include <com/xuggle/ferry/Ferry.h>

namespace com { namespace xuggle { namespace ferry {
  class AtomicInteger;

  /**
   * Parent of all Ferry objects -- it mains reference counts
   * in native code.
   * <p>
   * RefCounted objects cannot be made with new.  They must be
   * constructed with special factory methods, usually called
   * make(...).
   * </p>
   * <h2>Special note for developers in languages other than C++</h2>
   * <p>
   * You should not need to worry about this class very much.  Feel
   * free to ignore it.
   * </p>
   * <h2>Special note for C++ Users</h2>
   * <p>
   * Users of RefCounted objects in Native (C++) code must make
   * sure they acquire() a reference to an object if they
   * intend to keep using it after they have returned from
   * the method it was passed to, and
   * must call release() when done to ensure memory is freed.
   * </p>
   * <p>
   * Methods that return RefCounted objects on the stack are
   * expected to acquire() the reference for the caller, and
   * callers <b>must</b> release() any RefCounted object
   * returned on the stack.
   * <p>
   * For example:
   * </p>
   * <code>
   * <pre>
   * RefCounted * methodReturningRefCountedObject();
   * {
   *   mValueToReturn->acquire(); // acquire for caller
   *   return mValueToReturn; // and return
   * }
   *
   * {
   *   RefCounted *value = methodReturningRefCountedObject();
   *   ...
   *   // caller must release
   *   if (value)
   *     value->release();
   * }
   * </pre>
   * </code>
   */
  class VS_API_FERRY RefCounted
  {
  public:
    /**
     * Internal Only.  <strong>DO NOT USE FROM JAVA</strong>.
     * <p>
     * Acquire a reference to this object.
     * This increments the native internal ref count in native code by +1.  
     *  </p>
     * <p>
     * This method is called internally by Ferry in Java, and you should
     * not call it without knowing what you are doing.  But if you do
     * call it, make sure you call {@link #release()} once for each call
     * you make to this method.
     * </p>
     * @return The refcount after the acquire.  Note due to multi-threaded issues, you should not rely on this value,
     * as it may change before the method returns to you.
     */
    virtual int32_t acquire();

    /**
     * Internal Only.  <strong>DO NOT USE FROM JAVA</strong>.
     * <p>
     * This decrements the native internal ref count by -1; the object is destroyed if its ref count reaches zero.
     * </p>
     * <p>
     * This method is called internally by Ferry in Java, and you should
     * not call it without knowing what you are doing.  But if you do
     * call it, make sure you had previously called {@link #acquire()} once for each call
     * to {@link #release()} you make.
     * </p>
     * @return The ref count after the release.  Note due to multi-threaded issues, you should not rely on this value,
     * as it may change before the method returns to you.
     */
    virtual int32_t release();

#ifndef SWIG
    /**
     * Create a new Java object that refers to the same native object.
     * 
     * This method is meant for other language use like Java; it
     * will acquire the object but also force the creation of
     * a new proxy object in the target language that just
     * forwards to the same native object.
     * <p>
     * It is not meant for calling from C++ code; use the
     * standard acquire and release semantics for that.
     * </p>
     * @return A new Java object.
     */
    virtual RefCounted* copyReference();
#endif // ! SWIG

    /**
     * Return the current reference count on this object.
     *
     * The number returned represents the value at the instant
     * the message was called, and the value can change even
     * before this method returns.  Callers are advised not to
     * depend on the value of this except to check that
     * the value == 1.
     * <p>
     * If the value returned is one, and you know you have a valid
     * reference to that object, then congratulations; you are the
     * only person referencing that object.
     * </p>
     * @return The current ref count.
     */
    virtual int32_t getCurrentRefCount();
#ifndef SWIG
    // These are used by the JNIMemoryManager.cpp and wrapped there.
    /**
     * This method is public but not part of the standard API.
     *
     * RefCounted objects can have a Java Allocator associated
     * with them.  In general users of this library should
     * NEVER call this method, but it has to be public.
     * 
     * @param allocator An instance of a jobject we can use to do large memory allocation.
     *   The object should be of the com.xuggle.ferry.JNIMemoryAllocator java type.
     */
    void setJavaAllocator(void* allocator);
    /**
     * This method is public but not part of the standard API.
     *
     * @return Get the java allocator set.
     */
    void* getJavaAllocator();
#endif

  protected:
    /**
     * This method is called by RefCounted objects when their
     * Ref Count reaches zero and they are about to be destroyed.
     */
    virtual void destroy();

    RefCounted();
    virtual ~RefCounted();

    /**
     * This is the internal reference count, represented as
     * an AtomicInteger to make sure it is thread safe.
     */
    AtomicInteger* mRefCount;

    /**
     * Not part of public API.
     */
    void * mAllocator;
  };

#define VS_JNIUTILS_REFCOUNTED_MAKE(__class) \
    static __class* make() { \
        __class *obj = new __class(); \
        if (obj) { \
          (void) obj->acquire(); \
        } else { \
          throw std::bad_alloc(); \
        } \
        return obj; \
    } \
 private: \
    __class &operator=(const __class &); \
    __class(const __class &); \
    static void * operator new (size_t aSize) { return ::operator new(aSize); }

#define VS_JNIUTILS_REFCOUNTED_OBJECT(__class) \
    public: \
        VS_JNIUTILS_REFCOUNTED_MAKE(__class) \
    private:

#define VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(__className) \
    private: \
        VS_JNIUTILS_REFCOUNTED_MAKE(__className) \
    private:

}}}

#define VS_REF_ACQUIRE(__object) \
  do { \
    if (__object) { \
      (__object)->acquire(); \
    } \
  } while (0)

#define VS_REF_RELEASE(__object) \
  do { \
    if (__object) { \
      (__object)->release(); \
    } \
    (__object) = 0; \
  } while (0)

#endif /*REFCOUNTED_H_*/
