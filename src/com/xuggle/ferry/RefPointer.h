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

#ifndef REFPOINTER_H_
#define REFPOINTER_H_

#ifdef VS_DEBUG
#include <assert.h>
#endif
#include <com/xuggle/ferry/RefCounted.h>
namespace com { namespace xuggle { namespace ferry
  {

  /**
   * This class is only useful from C++.
   *
   * If you pass a RefCounted pointer to a RefPointer it
   * assumes you want it to manage the ref-count associated
   * with the RefCounted pointer.
   *
   * That means, whenever the RefPointer is destroyed, it will
   * RefCounted#release() the pointer it was asked to manage.  It will not
   * call acquire() on the pointer for it's own use, but will
   * RefCounted#acquire() it if someone calls #get() for the caller to manage.
   *
   * This can be a handy way to make sure RefCounted#acquire() is called for
   * you in order to reduce the chance of leaking memory from C++
   * code.
   */
  template <class T> class RefPointer
  {
  public:
    /**
     * Default constructor; sets the managed pointer to 0.
     */
    RefPointer() : mPtr(0)
    {
    }

    /**
     * Construct a RefPointer from a RefCounted object, and
     * take over management of the ref-count this pointer had.
     *
     * (i.e. it does not RefCounted#acquire() the pointer, but instead
     * assumes it is now the owner.  The person who called
     * this method should not call RefCounted#release() on the passed
     * in pointer once this object is managing it).
     *
     * @param ptr The pointer to start managing.
     */
    RefPointer(T* ptr) : mPtr(ptr)
    {
    }

    /**
     * Copy another RefPointer object.
     *
     * Since the other RefPointer object will RefCounted#release() its ref-count
     * upon destruction, we RefCounted#acquire() the pointer for ourselves.
     *
     * @param rPtr The pointer to RefCounted#acquire() a reference from
     * @return A new object
     */
    RefPointer(const RefPointer& rPtr) : mPtr(rPtr.mPtr)
    {
      VS_REF_ACQUIRE(mPtr);
    }
    /**
     * Copy and acquire.
     *
     * @param rPtr The pointer to RefCounted#acquire() a reference from
     * @return A new object
     */
    RefPointer(RefPointer& rPtr) : mPtr(rPtr.mPtr)
    {
      VS_REF_ACQUIRE(mPtr);
    }

    /**
     * Assignment operator to copy another ref pointer.  We
     * RefCounted#acquire() since the other side will RefCounted#release() it's own
     * ref-count later.
     * @param rPtr The pointer to assign from
     * @return A reference to ourself
     */
    RefPointer& operator=(const RefPointer& rPtr)
    {
      reset(static_cast<T*>(rPtr.mPtr));
      VS_REF_ACQUIRE(mPtr);
      return *this;
    }

    /**
     * Assignment operator to copy another ref pointer.  We
     * RefCounted#acquire() since the other side will RefCounted#release() it's own
     * ref-count later.
     * @param rPtr The pointer to assign from
     * @return A reference to ourself
     */
    RefPointer& operator=(RefPointer& rPtr)
    {
      reset(static_cast<T*>(rPtr.mPtr));
      VS_REF_ACQUIRE(mPtr);
      return *this;
    }

    /**
     * Assignment from a RefCounted* where we take over ownership of the
     * refcount.
     *
     * This is a pretty handy way of doing:
     * <code>
     *   RefPointer<foo> = Obj->getFoo();
     * </code>
     * Since by convention getFoo() functions should RefCounted#acquire() the
     * returned value for the caller, this ensures you don't
     * accidentally leak a value.
     *
     * @param ptr The pointer to take over management of
     * @return a reference to ourself
     */
    RefPointer& operator=(T* ptr)
    {
      reset(ptr);
      return *this;
    }

    /**
     * Allow people to def-ref the RefPointer() without
     * RefCounted#acquire() the managed pointer.
     *
     * To avoid the cost of a run-time check, we DON'T check for
     * nullness of the managed pointer.
     *
     * @return The underlying RefCounted pointer we manage, WITHOUT an extra RefCounted#acquire() call
     */
    T* operator->() const
    {
      // we don't protect against a null defer here.
      return static_cast<T*>(mPtr);
    }


    /**
     * This convenience method allows the use of the following code
     * to determine if the underlying pointer is null or not:
     * <code>
     *   RefPointer<Foo> p(someObj);
     *   if (p) {
     *     // underlying pointer is valid
     *     p->DoSomething();
     *   }
     * </code>
     *
     * @return true if we're managing a non-null pointer
     */
    operator bool()
    {
      return mPtr!=0;
    }

    /**
     * Always release the ref-count of our underlying pointer.
     */
    virtual ~RefPointer()
    {
  #ifdef VS_DEBUG
      {
        if (mPtr)
        {
          // This check should never fail in a multi-thread
          // environment, even though the underlying
          // objects ref can change.
          // If the object is bad, this should cause
          // a READ error in memory tools
          int refCount = mPtr->getCurrentRefCount();
          assert(refCount >= 1);
        }
      }
  #endif // VS_DEBUG
      VS_REF_RELEASE(mPtr);
    }

    /**
     * Call RefCounted#acquire() on the managed pointer and return it.
     *
     * @return Return a copy of the managed pointer.  We RefCounted#acquire() the pointer for the caller
     *    for the caller.
     */
    T* get()
    {
      VS_REF_ACQUIRE(mPtr);
      return static_cast<T*>(mPtr);
    }

    /**
     * Return the managed pointer without calling RefCounted#acquire() on it.
     *
     * This is how you pass values to methods expecting a RefCounted*
     * <code>
     * <pre>
     * void someMethod(RefCounted* arg);
     * RefPointer<RefCounted> b = ....;
     * someMethod(b.value());
     * </pre>
     * </code>
     *
     * @return Return the managed pointer without calling RefCounted#acquire() on it.
     */
    T* value()
    {
      return static_cast<T*>(mPtr);
    }

    /**
     * Reset the managed pointer, calling RefCounted#release() on the previously managed pointer first.
     *
     * @param ptr The new value to manage.
     * @param acquire should we also acquire a reference?  defaults to no (i.e. we take don't call RefCounted#acquire() to take over).
     */
    void reset(T* ptr=0, bool acquire=false)
    {
      VS_REF_RELEASE(mPtr);
      mPtr = ptr;
      if (acquire)
        VS_REF_ACQUIRE(mPtr);
    }
  private:
    // And the underlying thing we manage.
    // Must derive from RefCounted; We used to keep
    // a RefCounted here, but it's much nicer to
    // a debugger if you keep the actual type here.
    T *mPtr;
  };

  }}}

#endif /*REFPOINTER_H_*/
