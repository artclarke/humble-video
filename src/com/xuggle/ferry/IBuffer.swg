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

%typemap(javapackage) com::xuggle::ferry::IBuffer, com::xuggle::ferry::IBuffer*, com::xuggle::ferry::IBuffer& "com.xuggle.ferry"

%typemap(javabody_derived) com::xuggle::ferry::IBuffer %{
  // IBuffer.swg: Start generated code
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>

  private volatile long swigCPtr;

  /**
   * <strong>
   * DO NOT USE THIS CONSTRUCTOR - USE {@link #make(RefCounted,int)} INSTEAD.
   * </strong>
   * <p>
   * Internal Only. Do not allocate this object using new.  Not part of public API.
   * </p>
   * <p>
   * Unfortunately this constructor is public because the internal
   * implementation needs it to be, but do not pass in values to this method
   * as you may end up crashing the virtual machine.
   * </p>
   *
   * @param ignore1 ignore.
   * @param ignore2 ignore.
   *
   */ 
  public $javaclassname(long ignore1, boolean ignore2) {
    super($imclassname.SWIG$javaclassnameUpcast(ignore1), ignore2);
    swigCPtr = ignore1;
  }
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
   * Releases ths instance of $javaclassname and frees any underlying
   * native memory.
   * <p>
   * {@inheritDoc}
   * </p> 
   */
  @Override
  public void delete()
  {
    do {} while(false); // remove a warning
    super.delete();
  }
  
  /**
   * Create a new $javaclassname object that is actually referring to the
   * exact same underlying Native object.
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
  

  private void validateArgs(Object array, int arrayLength,
      int arrayPos,
      int bufferSize,
      int bufferPos,
      int length)
  {
    if (arrayPos < 0)
      throw new IndexOutOfBoundsException();
    if (bufferPos < 0)
      throw new IndexOutOfBoundsException();
    if (length < 0)
      throw new IndexOutOfBoundsException();
    if (arrayPos+length > arrayLength)
      throw new IndexOutOfBoundsException();
    if (bufferPos + length > bufferSize)
      throw new IndexOutOfBoundsException();
  }
  
  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers bytes into this buffer from the given source array.
   * If there are more bytes to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no bytes are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which bytes are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first byte to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of bytes to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(byte[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer buffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (buffer == null)
        return;
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers bytes from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer bytes in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no bytes are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which bytes are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first byte to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of bytes to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, byte[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer buffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (buffer == null)
        return;
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers shorts into this buffer from the given source array.
   * If there are more shorts to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no shorts are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which shorts are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first short to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of shorts to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(short[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.ShortBuffer buffer = bbuffer.asShortBuffer();
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers shorts from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer shorts in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no shorts are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which shorts are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first short to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of shorts to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, short[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.ShortBuffer buffer = bbuffer.asShortBuffer();
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  
  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers ints into this buffer from the given source array.
   * If there are more ints to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no ints are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which ints are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first int to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of ints to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(int[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.IntBuffer buffer = bbuffer.asIntBuffer();
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers ints from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer ints in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no ints are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which ints are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first int to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of ints to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, int[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.IntBuffer buffer = bbuffer.asIntBuffer();
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }


  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers longs into this buffer from the given source array.
   * If there are more longs to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no longs are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which longs are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first long to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of longs to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(long[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.LongBuffer buffer = bbuffer.asLongBuffer();
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers longs from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer longs in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no longs are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which longs are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first long to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of longs to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, long[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.LongBuffer buffer = bbuffer.asLongBuffer();
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }
  
  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers doubles into this buffer from the given source array.
   * If there are more doubles to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no doubles are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which doubles are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first double to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of doubles to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(double[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.DoubleBuffer buffer = bbuffer.asDoubleBuffer();
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers doubles from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer doubles in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no doubles are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which doubles are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first double to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of doubles to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, double[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.DoubleBuffer buffer = bbuffer.asDoubleBuffer();
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers floats into this buffer from the given source array.
   * If there are more floats to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no floats are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which floats are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first float to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of floats to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(float[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.FloatBuffer buffer = bbuffer.asFloatBuffer();
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers floats from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer floats in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no floats are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which floats are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first float to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of floats to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, float[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.FloatBuffer buffer = bbuffer.asFloatBuffer();
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk put method.
   * <p>
   * This method transfers chars into this buffer from the given source array.
   * If there are more chars to be copied from the array than there is
   * space remaining at the specified destination offset,
   * then no chars are transferred and a java.nio.BufferOverflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and copying the bytes over, but is more efficient in
   * the {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param src The array from which chars are to be read.  Must be non null.
   * @param srcPos The offset within <code>src</code> of the first char to be read;
   *   must be non-negative and less than <code>src.length</code>
   * @param destPos The offset in the IBuffer where you want to copy
   *   data to.  Must be non-negative and less than <code>getBufferSize()</code>
   * @param length The number of chars to be read from <code>src</code>;
   *  must be non-negative and no larger than <code>src.length - srcPos</code>
   *  
   * @throws NullPointerException if <code>src</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *   mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient space
   *   in this {@link IBuffer}.
   *   
   */
  public void put(char[] src, int srcPos, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.CharBuffer buffer = bbuffer.asCharBuffer();
      buffer.clear();
      validateArgs(src, src.length, srcPos, buffer.limit(), destPos, length);
      buffer.position(destPos);
      buffer.put(src, srcPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /**
   * Absolute bulk get method.
   * <p>
   * This method transfers chars from this {@link IBuffer} into the given
   * <code>dest</code>
   * array. If there are fewer chars in the {@link IBuffer} starting
   * at position <code>srcPos</code> than are required
   * to satisfy the request, then no chars are
   * transferred and a BufferUnderflowException is thrown.
   * </p>
   * <p>
   * This method is equivalent to calling {@link #getByteBuffer(int, int)}
   * yourself and reading the bytes out, but is more efficient in the
   * {@link JNIMemoryManager.MemoryModel#NATIVE_BUFFERS} memory model.
   * </p>
   * 
   * @param srcPos The offset in the IBuffer where you want to start
   *        copying data from.
   *        Must be non-negative and less than <code>getBufferSize()</code>
   * @param dest The array to which chars are to be written. Must be non null.
   * @param destPos The offset within <code>dest</code> of the first char to be
   *        written; must be non-negative and less than <code>dest.length</code>
   * @param length The number of chars to be copied into <code>dest</code>; must be
   *        non-negative and no larger than <code>dest.length - destPos</code>
   * 
   * @throws NullPointerException if <code>dest</code> is null.
   * @throws IndexOutOfBoundsException if the preconditions on the arguments
   *         mentioned above are not honored.
   * @throws java.nio.BufferOverflowException if there is insufficient data in this
   *         {@link IBuffer} to satisfy the request.
   * 
   */
  public void get(int srcPos, char[] dest, int destPos, int length)
  {
    java.util.concurrent.atomic.AtomicReference<JNIReference> ref =
      new java.util.concurrent.atomic.AtomicReference<JNIReference>();
    java.nio.ByteBuffer bbuffer = this.getByteBuffer(0, this.getBufferSize(),
        ref);
    
    try
    {
      if (bbuffer == null)
        return;
      java.nio.CharBuffer buffer = bbuffer.asCharBuffer();
      validateArgs(dest, dest.length, destPos, buffer.limit(), srcPos, length);
      buffer.position(srcPos);
      buffer.get(dest, destPos, length);
      return;
    }
    finally
    {
      if (ref.get() != null)
        ref.get().delete();
    }
  }


  /**
   * Returns up to length bytes, starting at offset in the underlying
   * buffer we're managing.
   *
   * <p> 
   * 
   * The buffer position, mark are initialized to zero and limit
   * is set to the maximum capacity of this buffer.  For some
   * IBuffer contents, the actual usable data in this buffer will
   * be less that the limit.
   * 
   * </p>
   * <p>
   * If you want to control exactly when the underlying memory
   * beyind the returned {@link java.nio.ByteBuffer} is released,
   * use {@link #getByteBuffer(int, int, java.util.concurrent.atomic.AtomicReference)}.
   * </p>
   * @see #getByteBuffer(int, int, java.util.concurrent.atomic.AtomicReference)
   */
  public java.nio.ByteBuffer getByteBuffer(int offset, int length)
  {
    return getByteBuffer(offset, length, null);
  }

  

    /**
     * Returns up to length bytes, starting at offset in the underlying
     * buffer we're managing, and also passed back a {@link JNIReference}
     * that can optionally be used by the caller to free the underlying
     * native memory.
     *
     * <p> 
     * 
     * The buffer position, mark are initialized to zero and limit
     * is set to the maximum capacity of this buffer.  For some
     * IBuffer contents, the actual usable data in this buffer will
     * be less that the limit.
     * 
     * </p>
     * <p>
     *
     * If you use this method you are accessing the direct native
     * memory associated with this buffer.  That means changes you make
     * to this buffer are immediately reflected in the underlying
     * memory.
     *    
     * </p>
     * <p>
     *
     * Once you call this method,
     * the underlying native memory allocated will not be released until
     * all references to the returned ByteBuffer are no longer reachable and
     * at least one call to {@link JNIMemoryManager#gc()} has been
     * performed.  You can force the memory to be collect earlier
     * by using the {@link JNIReference} value returned when you
     * call this method.
     * </p><p>
     *  The {@link JNIMemoryManager#gc()} is called whenever
     * xuggler tries to allocate new memory for any Xuggler interface,
     * so normally you don't need to care about this.  If for some
     * reason no other Xuggler object is ever allocated, every
     * Xuggler object has
     * a finalizer as well that will do the right thing.
     *
     * </p>
     * <p>
     *
     * You can also start up
     * a separate thread to do continuously do Ferry garabage
     * collections by calling
     * {@link JNIMemoryManager#startCollectionThread()}.  This thread
     * will only wake up when it has work to do, so the overhead
     * is very low.  We don't turn it on by default since in
     * 99.999% of cases you don't need to worry about it.
     *  
     * </p>
     * 
     * @param offset The offset (in bytes) into the buffer managed by
     *   this IBuffer
     * @param length The requested length (in bytes) you want to access.
     *   The buffer returned may actually be longer than length.
     * @param referenceReturn If non null, on exit 
     *   calling {@link java.util.concurrent.atomic.AtomicReference#get()}
     *   on this value will return a {@link JNIReference} you can use
     *   for explicitly de-allocating the underlying native store
     *   of the {@link java.nio.ByteBuffer}.  Call
     *   {@link JNIReference#delete()} to do that.  <strong>Warning:</strong>
     *   if you do call {@link JNIReference#delete()} on the
     *   value returned in this parameter, then the returned byte buffer
     *   will be immediately invalid.
     * @return A java.nio.ByteBuffer that directly accesses
     *   the native memory this IBuffer manages, or null if
     *   error.
     */
  public java.nio.ByteBuffer getByteBuffer(int offset, int length,
      java.util.concurrent.atomic.AtomicReference<JNIReference> referenceReturn)
  {
    java.nio.ByteBuffer retval = this.java_getByteBuffer(offset, length);
    if (retval != null)
    {
      // increment the ref count of this class to reflect the
      // byte buffer
      java.util.concurrent.atomic.AtomicLong refCount =
        this.getJavaRefCount();
      refCount.incrementAndGet();
      
      // and use the byte buffer as the reference to track
      JNIReference ref = JNIReference.createNonFerryReference(
          this,
          retval, swigCPtr, refCount);
      if (referenceReturn != null)
        referenceReturn.set(ref);
      
      // and tell Java this byte buffer is in native order
      retval.order(java.nio.ByteOrder.nativeOrder());
    }
    return retval;
  }

  /**
   * Prints the same as {@link Object#toString()} but appends
   * the maximum number of bytes that will fit in this
   * {@link IBuffer}, the type of the {@link IBuffer}, and the maximum
   * size in units of {@link IBuffer.Type} that will fit in the
   * {@link IBuffer}.
   * {@inheritDoc}
   */
  @Override
  public String toString()
  {
    final StringBuilder builder = new StringBuilder();
    builder.append(super.toString());
    builder.append("[");
    builder.append("bytes=").append(getBufferSize()).append(";");
    builder.append("type=").append(getType()).append(";");
    builder.append("size=").append(getSize()).append(";");
    builder.append("]");
    return builder.toString();
  }
  
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<
  // IBuffer.swg
%}
%typemap(javadestruct, methodname="doNotSet", methodmodifiers="public synchronized") com::xuggle::ferry::IBuffer ""
%typemap(javadestruct_derived, methodname="doNotSet", methodmodifiers="public synchronized") com::xuggle::ferry::IBuffer ""

%{
static void IBuffer_javaDirectFreeFunc(void *, void * closure);
typedef jobject jNioByteArray;

%}

typedef jobject jNioByteArray;

%include <com/xuggle/ferry/IBuffer.h>
%{
  /**
   * This method is passed as a freefunc to the Buffer object.  Once
   * the IBuffer has no more references to it, this method will be called,
   * and will release the backing java.nio.ByteBuffer object that we got
   * data from.
   */
  static void
  IBuffer_javaDirectFreeFunc(void *, void * closure)
  {
    jobject globalRef = static_cast<jobject>(closure);
    JNIEnv* env = JNIHelper::sGetEnv();
    if (env && globalRef)
    {
      //fprintf(stderr, "Releasing global ref: %p\n", globalRef);
      env->DeleteGlobalRef(globalRef);
    }
  }
  
%}

%extend com::xuggle::ferry::IBuffer {
// Used so that extend methods can return a jNioByteArray and it gets mapped
// to java.nio.ByteBuffer

  public:
  
  /**
   * Internal only.  Do not use.
   */
   
  %javamethodmodifiers java_getByteBuffer() "private"
  jNioByteArray java_getByteBuffer(int32_t offset, int32_t length)
  {
    void * buffer = 0;
    jobject retval = 0;
    
    buffer = $self->getBytes(offset, length);
    if (buffer)
    {
      JNIEnv *env = JNIHelper::sGetEnv();
      if (env)
      {
        if (!env->ExceptionCheck())
          retval = env->NewDirectByteBuffer(buffer, length);
      }
    }
    return retval;
  }
    /**
     * Returns up to length bytes, starting at offset in the
     * underlying buffer we're managing.
     * <p> 
     * This method COPIES the data into the byte array being
     * returned..
     * </p><p>
     * If you don't NEED the direct access that getByteBuffer
     * offers (and most programs can in fact take the performance
     * hit of the copy), we recommend you use this method.
     * It's much harder to accidentally leave native memory lying
     * around waiting for cleanup then.
     * </p>
     * 
     * @param offset The offset (in bytes) into the buffer managed by this IBuffer
     * @param length The requested length (in bytes) you want to access.  The buffer returned may
     *   actually be longer than length.
     * 
     * @return A copy of the data that is in this IBuffer, or null
     *   if error.
     */
  jbyteArray getByteArray(int32_t offset, int32_t length)
  {
    jbyte * buffer = 0;
    jbyteArray retval = 0;
    
    buffer = static_cast<jbyte*>($self->getBytes(offset, length));
    if (buffer)
    {
      JNIEnv *env = JNIHelper::sGetEnv();
      if (env)
      {
        if (env->ExceptionCheck())
          return 0;
        retval = env->NewByteArray(length);
        if (env->ExceptionCheck())
        {
          if (retval) env->DeleteLocalRef(retval);
          retval = 0;
        }
        if (retval)
        {
          // copy the data into the byte array
          env->SetByteArrayRegion(retval, 0, length, buffer);
          if (env->ExceptionCheck())
          {
            // an error occurred; release our byte array
            // reference and return.
            env->DeleteLocalRef(retval);
            retval = 0;
          }
        }
      }
    }
    return retval;
  }

  /**
   * Allocate a new IBuffer, and copy the data in buffer into
   * the new IBuffer object.
   *
   * @param requestor An optional value telling the IBuffer class
   *   what object requested it. This is used for debugging memory leaks;
   *   it's a marker for the FERRY object (e.g. IPacket) that actually
   *   requested the buffer. If you're not an FERRY object, pass in null here.
   * @param buffer A java byte buffer for the data containing the
   *   data you want to copy.
   * @param offset The starting offset in buffer where you want
   *   to start copying.
   * @param length The total number of bytes you want to copy from buffer.
   *
   * @return a new IBuffer object with a copy of the data in buffer,
   *   or null on failure.
   */
   
  static IBuffer* make(com::xuggle::ferry::RefCounted* requestor,
     jbyteArray buffer, int32_t offset, int32_t length)
  {
    IBuffer* retval = 0;
    try
    {
      JNIEnv* env = JNIHelper::sGetEnv();
      if (!env)
        throw std::runtime_error("could not get java environment");
      
      if (env->ExceptionCheck())
        throw std::runtime_error("pending Java exception");

      if (!buffer)
        throw std::invalid_argument("no byte buffer passed in");
      
      jsize bufSize = env->GetArrayLength(buffer);
      if (env->ExceptionCheck())
        throw std::runtime_error("could not get java byteArray size");

      if (bufSize < offset + length)
        throw std::out_of_range("invalid offset and length");
      
      retval = IBuffer::make(requestor, length);
      if (!retval)
        throw std::runtime_error("could not allocate IBuffer");

      jbyte* bytes = static_cast<jbyte*>(retval->getBytes(0, length));
      if (!bytes)
        throw std::bad_alloc();
      
      // now try the copy
      env->GetByteArrayRegion(buffer, offset, length, bytes);
      if (env->ExceptionCheck())
        throw std::runtime_error("could not copy data into native IBuffer memory");
    }
    catch(std::exception & c)
    {
      VS_REF_RELEASE(retval);
    }
    return retval;

  }

  /**
   * Create a new IBuffer object that uses the direct byte buffer
   * passed in by reference (i.e. it directly uses the bytes in
   * the direct byte buffer).
   *
   * @param requestor An optional value telling the IBuffer class
   *   what object requested it. This is used for debugging memory leaks;
   *   it's a marker for the FERRY object (e.g. IPacket) that actually
   *   requested the buffer. If you're not an FERRY object, pass in null here.
   * @param directByteBuffer A direct {@link java.nio.ByteBuffer} object
   *   you want to use for your memory.  This must be a direct object --
   *   non direct objects will result in an JVM-dependent exception
   *   being thrown. 
   * @param offset The starting offset in directByteBuffer where you want
   *   to start copying.
   * @param length The total number of bytes you want to copy from
   *   directByteBuffer.
   *
   * @return a new IBuffer object that is using directByteBuffer
   *   behind the scenes, or null on failure.
   */
      
  static IBuffer* make(com::xuggle::ferry::RefCounted* requestor,
     jNioByteArray directByteBuffer, int32_t offset, int32_t length)
  {
    IBuffer * retval = 0;
    jobject globalRef = 0;
    JNIEnv* env = JNIHelper::sGetEnv();
    try
    {
      if (!env)
        throw std::runtime_error("could not get java environment");
      
      if (env->ExceptionCheck())
        throw std::runtime_error("pending Java exception");

      if (!directByteBuffer)
        throw std::invalid_argument("no byte buffer passed in");
      
      jclass byteBufferClass = env->FindClass("java/nio/ByteBuffer");
      if (env->ExceptionCheck() || !byteBufferClass)
        throw std::runtime_error("could not get find java/nio/ByteBuffer class");
      jboolean rightClass = env->IsInstanceOf(directByteBuffer,byteBufferClass);
      env->DeleteLocalRef(byteBufferClass);
      if (env->ExceptionCheck())
        throw std::runtime_error("could not get instanceof passed in object");
      if (!rightClass)
      {
        jclass cls=env->FindClass("java/lang/IllegalArgumentException");
        if (cls)
          env->ThrowNew(cls, "object passed in is not instance of java.nio.ByteBuffer");
        throw std::runtime_error("object not instanceof java.nio.ByteBuffer");
      }
      // let's figure out if this is a direct buffer
      int32_t availableLength = env->GetDirectBufferCapacity(directByteBuffer);
      if (env->ExceptionCheck())
        throw std::runtime_error("could not get java byteArray size");
      int8_t* javaBuffer = static_cast<int8_t*>(env->GetDirectBufferAddress(directByteBuffer));
      if (env->ExceptionCheck())
        throw std::runtime_error("could not get java direct byte buffer");
      
      if (availableLength == -1 || !javaBuffer)
      {
        jclass cls=env->FindClass("java/lang/IllegalArgumentException");
        if (cls)
          env->ThrowNew(cls, "object passed in is not instance of java.nio.ByteBuffer or this JVM doesn't allow native code to access direct buffers");
        throw std::runtime_error("object not instanceof java.nio.ByteBuffer");
      }


      if (availableLength < length + offset)
        throw std::runtime_error("not enough data in byte buffer");
      
      // Let's try creating a wrapper around this object.
      // Now, let's get a global reference to remember 
      globalRef = env->NewGlobalRef(directByteBuffer);
      if (env->ExceptionCheck())
        throw std::runtime_error("could not get global reference to passed in byte array");
            
      retval = IBuffer::make(requestor, javaBuffer+offset, length,
          IBuffer_javaDirectFreeFunc, globalRef);
      //fprintf(stderr, "Creating global ref: %p\n", globalRef);
      
      if (!retval)
        throw std::runtime_error("could not wrap java byte array");
      globalRef = 0;

    }
    catch (std::exception & c)
    {
      if (env && globalRef)
        env->DeleteGlobalRef(globalRef);
      globalRef = 0;
      VS_REF_RELEASE(retval);
    }
    
    return retval;
  }

}
