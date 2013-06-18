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

/* From Swig 1.3.33 */
/* javabody typemaps */

%include <enums.swg>
%include <stdint.i>
%typemap(javapackage) com::xuggle::ferry::RefCounted, com::xuggle::ferry::RefCounted *, com::xuggle::ferry::RefCounted & "com.xuggle.ferry"

%typemap(javabody) com::xuggle::ferry::RefCounted %{
  // JNIHelper.swg: Start generated code
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>
  private volatile long swigCPtr;
  /**
   * Internal Only.
   */
  protected boolean swigCMemOwn;
  private com.xuggle.ferry.JNIReference mRefCounter;
  private Long mLifecycleReference;

  // the next object looks like it is unused by the compiler, but that's not true; it most
  // definitely is used :)
  @SuppressWarnings("unused")
  private com.xuggle.ferry.JNINativeFinalizer mObjectToForceFinalize;
  private java.util.concurrent.atomic.AtomicLong mJavaRefCount;

  /**
   * Internal Only.  Not part of public API.
   */
  protected RefCounted(long cPtr, boolean cMemoryOwn) {
    this(cPtr, cMemoryOwn, new java.util.concurrent.atomic.AtomicLong(0L));
  }
  
  /**
   * Internal Only.  Not part of public API.
   */
  protected RefCounted(long cPtr, boolean cMemoryOwn,
      java.util.concurrent.atomic.AtomicLong ref)
  {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
    // Create the reference count that java-only objects will use
    // to avoid calling acquire and release on copyReference
    ref.incrementAndGet();
    mJavaRefCount = ref;
    if (swigCPtr != 0)
    {
      // This object must:
      // (a) have no references to any other java object
      // (b) Have a finalizer that just calls gc() on our JNIMemoryManager
      // It exists only to force gc() to be called even if someone calls the Xuggle
      // libraries once and never again
      mObjectToForceFinalize = new com.xuggle.ferry.JNINativeFinalizer();
      // This is what the weak reference will watch; this object should
      // (a) have no references to any other Java object
      // (b) MUST NOT have a finalizer
      // (c) MUST NOT acquire any other references to it except for the strong reference
      //     right here, and the weak reference in mRefCounter
      mLifecycleReference = new Long(swigCPtr);      
      // Force the creation of a new weak reference, will will actually pin itself
      // inside the reference class.
      mRefCounter = JNIReference.createReference(
          this,
          mLifecycleReference, swigCPtr,
          mJavaRefCount);
    }
  }

  /**
   * Internal Only.  Not part of public API.
   *
   * Get the raw value of the native object that obj is proxying for.
   *   
   * @param obj The java proxy object for a native object.
   * @return The raw pointer obj is proxying for.
   */
  public static long getCPtr(RefCounted obj) {
    if (obj == null) return 0;
    return obj.getMyCPtr();
  }
  
  /**
   * Internal Only.  Not part of public API.
   *
   * Get the raw value of the native object that we're proxying for.
   *   
   * @return The raw pointer we're proxying for.
   */
  public long getMyCPtr() {
    if (swigCPtr == 0) throw new NullPointerException("underlying native object already deleted");
    return swigCPtr;
  }
  
  /**
   * Releases any underlying native memory and marks this object
   * as invalid.
   * <p>
   * Normally Ferry manages when to release native memory.
   * </p>
   * <p>
   * In the unlikely event you want to control EXACTLY when a native 
   * object is released, each Xuggler object has a {@link #delete()}
   * method that you can use. Once you call {@link #delete()},
   * you must ENSURE your object is never referenced again from
   * that Java object -- Ferry tries to help you avoid crashes if you
   * accidentally use an object after deletion but on this but we
   * cannot offer 100% protection (specifically if another thread
   *  is accessing that object EXACTLY when you {@link #delete()} it). 
   * </p>
   */
   
   public void delete()
   {
     if(swigCPtr != 0) {
       // assigning to an object removes an incorrect java
       // compiler warning for some
       // generated files
       Object object = this;
       if (object instanceof RefCounted && mRefCounter != null) {
         mRefCounter.delete();
       } else if (swigCMemOwn) {
         swigCMemOwn = false;
       }

     }
     mJavaRefCount = null;
     mRefCounter = null;
     mObjectToForceFinalize = null;
     mLifecycleReference = null;
     swigCPtr = 0;
   }
   
  /**
   * Create a new RefCounted object that is actually referring to the
   * exact same underlying native object.
   *
   * @return the new Java object.
   */
  public $javaclassname copyReference() {
    if (swigCPtr == 0)
      return null;
    else
      return new $javaclassname(swigCPtr, swigCMemOwn, getJavaRefCount());
  }

  /**
   * Internal Only.  Do not use.
   */
  protected java.util.concurrent.atomic.AtomicLong getJavaRefCount()
  {
    return mJavaRefCount;
  }
   

    /**
     * Return the current reference count on this object.
     * <p>
     * The number returned represents the value at the instant
     * the message was called, and the value can change even
     * before this method returns.  Callers are advised not to
     * depend on the value of this except to check that
     * the value == 1.
     * </p>
     * <p>
     * If the value returned is one, and you know you have a valid
     * reference to that object, then congratulations; you are the
     * only person referencing that object.
     * </p>
     * @return The current ref count.
     */
  public long getCurrentRefCount()
  {
    return mJavaRefCount.get()+getCurrentNativeRefCount()-1;
  }
     // <<<<<<<<<<<<<<<<<<<<<<<<<<<
   // JNIHelper.swg: End generated code
  
 
 %}

%typemap(javabody) SWIGTYPE %{
  // JNIHelper.swg: Start generated code
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>

  private volatile long swigCPtr;
  /**
   * Internal Only.
   */
  protected boolean swigCMemOwn;
  @SuppressWarnings("unused")
  private JNINativeFinalizer mUnusedVariableToAllowImports;
  
  /**
   * Internal Only.  
   * 
   * DO NOT USE: Do not allocate this method using new.  Not part of public API.
   * <p>
   * Unfortunately this constructor is public because the internal
   * implementation needs it to be, but do not pass in values to this method
   * as you may end up crashing the virtual machine.
   * </p>
   *
   * @param cPtr A C pointer to direct memory; did we mention don't call this.
   * @param cMemoryOwn I'm not even going to tell you.  Stop it.  Go away.
   *
   */ 
  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    swigCPtr = cPtr;
    swigCMemOwn = cMemoryOwn;
  }

  /**
   * Internal Only.  Not part of public API.
   *
   * Get the raw value of the native object that obj is proxying for.
   *   
   * @param obj The java proxy object for a native object.
   * @return The raw pointer obj is proxying for.
   */
  public static long getCPtr($javaclassname obj) {
    if (obj == null) return 0;
    return obj.getMyCPtr();
  }
  
  /**
   * Internal Only.  Not part of public API.
   *
   * Get the raw value of the native object that we're proxying for.
   *   
   * @return The raw pointer we're proxying for.
   */  
  public long getMyCPtr() {
    if (swigCPtr == 0) throw new IllegalStateException("underlying native object already deleted");
    return swigCPtr;
  }

  /**
   * Compares two values, returning true if the underlying objects in native code are the same object.
   *
   * That means you can have two different Java objects, but when you do a comparison, you'll find out
   * they are the EXACT same object.
   *
   * @return True if the underlying native object is the same.  False otherwise.
   */
  public boolean equals(Object obj) {
    boolean equal = false;
    if (obj instanceof $javaclassname)
      equal = ((($javaclassname)obj).swigCPtr == this.swigCPtr);
    return equal;
  }
  
  /**
   * Get a hashable value for this object.
   *
   * @return the hashable value.
   */
  public int hashCode() {
     return (int)swigCPtr;
  }
  
  /**
   * Internal Only.  Finalize this object.  Note this should only exists on non RefCounted objects.
   */
  protected void finalize()
  {
    delete();
  }

  // <<<<<<<<<<<<<<<<<<<<<<<<<<<
  // JNIHelper.swg: End generated code
  /**
   * Releases any underlying native memory and marks this object
   * as invalid.
   * <p>
   * Normally Ferry manages when to release native memory.
   * </p>
   * <p>
   * In the unlikely event you want to control EXACTLY when a native 
   * object is released, each Xuggler object has a {@link #delete()}
   * method that you can use. Once you call {@link #delete()},
   * you must ENSURE your object is never referenced again from
   * that Java object -- Ferry tries to help you avoid crashes if you
   * accidentally use an object after deletion but on this but we
   * cannot offer 100% protection (specifically if another thread
   *  is accessing that object EXACTLY when you {@link #delete()} it). 
   * </p>
   */
  
%}

// Drop Java Finalization because we put it in the body.
%typemap(javafinalize) SWIGTYPE ""

// Derived proxy classes
%typemap(javabody_derived) SWIGTYPE %{
  // JNIHelper.swg: Start generated code
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>
  /**
   * This method is only here to use some references and remove
   * a Eclipse compiler warning.
   */
  @SuppressWarnings("unused")
  private void noop()
  {
    IBuffer.make(null, 1);
  }
   
  private volatile long swigCPtr;

  /**
   * Internal Only.
   */
  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    super($imclassname.SWIG$javaclassnameUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }
  
  /**
   * Internal Only.
   */
  protected $javaclassname(long cPtr, boolean cMemoryOwn,
      java.util.concurrent.atomic.AtomicLong ref)
  {
    super($imclassname.SWIG$javaclassnameUpcast(cPtr),
     cMemoryOwn, ref);
    swigCPtr = cPtr;
  }
    
  /**
   * Internal Only.  Not part of public API.
   *
   * Get the raw value of the native object that obj is proxying for.
   *   
   * @param obj The java proxy object for a native object.
   * @return The raw pointer obj is proxying for.
   */
  public static long getCPtr($javaclassname obj) {
    if (obj == null) return 0;
    return obj.getMyCPtr();
  }

  /**
   * Internal Only.  Not part of public API.
   *
   * Get the raw value of the native object that we're proxying for.
   *   
   * @return The raw pointer we're proxying for.
   */  
  public long getMyCPtr() {
    if (swigCPtr == 0) throw new IllegalStateException("underlying native object already deleted");
    return swigCPtr;
  }
  
  /**
   * Create a new $javaclassname object that is actually referring to the
   * exact same underlying native object.
   *
   * @return the new Java object.
   */
  @Override
  public $javaclassname copyReference() {
    if (swigCPtr == 0)
      return null;
    else
      return new $javaclassname(swigCPtr, swigCMemOwn, getJavaRefCount());
  }

  /**
   * Compares two values, returning true if the underlying objects in native code are the same object.
   *
   * That means you can have two different Java objects, but when you do a comparison, you'll find out
   * they are the EXACT same object.
   *
   * @return True if the underlying native object is the same.  False otherwise.
   */
  public boolean equals(Object obj) {
    boolean equal = false;
    if (obj instanceof $javaclassname)
      equal = ((($javaclassname)obj).swigCPtr == this.swigCPtr);
    return equal;
  }
  
  /**
   * Get a hashable value for this object.
   *
   * @return the hashable value.
   */
  public int hashCode() {
     return (int)swigCPtr;
  }
  
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<
  // JNIHelper.swg: End generated code
  %}
  

// For RefCounted and children
%typemap(javadestruct, methodname="doNotSet", methodmodifiers="public synchronized") com::xuggle::ferry::RefCounted ""
%typemap(javadestruct_derived, methodname="doNotSet", methodmodifiers="public synchronized") SWIGTYPE ""


%typemap(check) SWIGTYPE *self %{
  if (!$1) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
      "invalid native object; delete() likely already called");
    return $null;
  }
%}

%exception
{
  // JNIHelper.swg: Start generated code
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>
  try
  {
    $action
  }
  catch(std::invalid_argument & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/IllegalArgumentException");
      jenv->ThrowNew(cls, e.what());
    }
    return $null;
  }
  catch(std::bad_alloc & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      // OK, this is bad and may mean we can't do things like 
      // allocate a new class at this time; but we should have
      // one lying around
      com::xuggle::ferry::JNIHelper *helper = 
        com::xuggle::ferry::JNIHelper::getHelper();
      if (helper) {
        helper->throwOutOfMemoryError();
      }
    }
    return $null;
  }
  catch(std::exception & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, e.what());
    }
    return $null;
  }
  catch(...)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, "Unhandled and unknown native exception");
    }
    return $null;
  }
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<
  // JNIHelper.swg: End generated code
}

// This makes any type labeled jNioByteArray show up in swig wrappers
// as a java.nio.ByteBuffer type
%typemap(jtype)  jNioByteArray, const jNioByteArray &  "java.nio.ByteBuffer";
%typemap(jstype) jNioByteArray, const jNioByteArray &  "java.nio.ByteBuffer";
