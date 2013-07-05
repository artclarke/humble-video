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

%typemap (javacode) io::humble::video::MediaData,io::humble::video::MediaData*,io::humble::video::MediaData& %{

  // used to correct timezone offsets for timestamp format 

  private static final long TIME_OFFSET = -java.util.Calendar.getInstance()
    .getTimeZone().getRawOffset();

  /** The default time stamp format. */
  
  public static final String DEFAULT_TIME_STAMP_FORMAT =
    "%1$tH:%1$tM:%1$tS.%1$tL";
/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(byte[], int, int, int)
 */
public void put(byte[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, byte[], int, int)
 */
public void get(int srcPos, byte[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}

/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(char[], int, int, int)
 */
public void put(char[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, char[], int, int)
 */
public void get(int srcPos, char[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}

/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(short[], int, int, int)
 */
public void put(short[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, short[], int, int)
 */
public void get(int srcPos, short[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}

/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(int[], int, int, int)
 */
public void put(int[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, int[], int, int)
 */
public void get(int srcPos, int[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}

/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(long[], int, int, int)
 */
public void put(long[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, long[], int, int)
 */
public void get(int srcPos, long[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}

/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(float[], int, int, int)
 */
public void put(float[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, float[], int, int)
 */
public void get(int srcPos, float[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}

/**
 * Absolute bulk put method.
 * <p>
 * This method delegates to the same put method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#put(double[], int, int, int)
 */
public void put(double[] src, int srcPos, int destPos, int length)
{
  io.humble.ferry.IBuffer buffer = getDataCached();
  if (buffer == null)
    return;
  buffer.put(src, srcPos, destPos, length);
  return;
}

/**
 * Absolute bulk get method.
 * <p>
 * This method delegates to the same get method on the
 * underlying {@link io.humble.ferry.IBuffer} returned from {@link #getData()} ,
 * but may be more efficient in some {@link io.humble.ferry.JNIMemoryManager.MemoryModel}
 * configurations.
 * </p>
 * @see io.humble.ferry.IBuffer#get(int, double[], int, int)
 */
public void get(int srcPos, double[] dest, int destPos, int length)
{
   io.humble.ferry.IBuffer buffer = getDataCached();
   if (buffer == null)
     return;
   buffer.get(srcPos, dest, destPos, length);
   return;
}


  /**
   * Gets the underlying {@link java.nio.ByteBuffer} for this {@link
   * MediaData} object.  Users may modify the contents of the
   * ByteBuffer and their changes will be reflected in the underlying
   * memory.  See {@link io.humble.ferry.IBuffer} for the warnings
   * associated with the use of raw memory.  The buffer position and
   * mark are initialized to zero, and the limit is initialized to the
   * number of elements in the buffer.
   *
   * <p>
   *
   * This is a convenience method to allow you to avoid getting the
   * IBuffer object that has the actual data.
   *
   * </p>
   *
   * @return The underlying ByteBuffer
   * 
   * @see io.humble.ferry.IBuffer#getByteBuffer(int, int)
   */

  public java.nio.ByteBuffer getByteBuffer()
  {
    return getByteBuffer(null);
  }

  /**
   * Gets the underlying {@link java.nio.ByteBuffer} for this {@link
   * MediaData} object.  Users may modify the contents of the
   * ByteBuffer and their changes will be reflected in the underlying
   * memory.  See {@link io.humble.ferry.IBuffer} for the warnings
   * associated with the use of raw memory.  The buffer position and
   * mark are initialized to zero, and the limit is initialized to the
   * number of elements in the buffer.
   *
   * <p>
   *
   * This is a convenience method to allow you to avoid getting the
   * IBuffer object that has the actual data.
   *
   * </p>
   *
   * @param ref A reference that on return from this method will
   *   contain a {@link io.humble.ferry.JNIReference} object
   *   that can be used for explicit memory management.
   * @return The underlying ByteBuffer
   * 
   * @see io.humble.ferry.IBuffer#getByteBuffer(int, int, java.util.concurrent.atomic.AtomicReference)
   */

  public java.nio.ByteBuffer getByteBuffer(
      java.util.concurrent.atomic.AtomicReference<io.humble.ferry.JNIReference> ref) {
    if (ref != null)
      ref.set(null);
      
    io.humble.ferry.IBuffer buf = this.getDataCached();
    if (buf == null)
      return null;
    java.nio.ByteBuffer byteBuffer = buf.getByteBuffer(0, this.getSize(), ref);
    if (byteBuffer != null)
    {
      byteBuffer.position(0);
      byteBuffer.mark();
      byteBuffer.limit(this.getSize());
    }
    return byteBuffer;
  }


  /**
   * Get a string representation of the time stamp for this {@link
   * MediaData}.  The time is formatted as: <b>HH:MM:SS.ms</b>
   *
   * @return the printable string form of the time stamp of this media
   *
   * @see #getFormattedTimeStamp(String)
   * @see #DEFAULT_TIME_STAMP_FORMAT
   */

  public String getFormattedTimeStamp()
  {
    return getFormattedTimeStamp(DEFAULT_TIME_STAMP_FORMAT);
  }

  /**
   * Get a string representation of the time stamp for this {@link
   * MediaData}.  The format of the resulting string is specified by
   * the format parameter.  See {@link java.util.Formatter} for 
   * details on how to specify formats, however a good place to start
   * is with the following format: <b>%1$tH:%1$tM:%1$tS.%1$tL</b>
   *
   * @param format the format for the time stamp string
   *
   * @return the printable string form of the timestamp
   * 
   * @see #getFormattedTimeStamp()
   * @see #DEFAULT_TIME_STAMP_FORMAT
   * @see java.util.Formatter
   */

  public String getFormattedTimeStamp(String format)
  {
    String retval = null;
    java.util.Formatter formatter = new java.util.Formatter();

    try {
      Rational timeBase = getTimeBase();
      if (timeBase == null)
        timeBase = Rational.make(1,(int)Global.DEFAULT_PTS_PER_SECOND);
      retval = formatter.format(format,
          (long)(getTimeStamp() * timeBase.getDouble() * 1000) +
          TIME_OFFSET).toString();
      timeBase.delete();
    } finally {
      formatter.close();
    }
    return retval;
  }
  
  /**
   * The data used by {@link #getDataCached()}.
   */
  private io.humble.ferry.IBuffer mData;

  /**
   * Get any underlying raw data available for this object.
   * 
   * @return The raw data, or null if not accessible.
   */
  public io.humble.ferry.IBuffer getData()
  {
    io.humble.ferry.IBuffer retval = getData_internal();
    if (retval == null)
    {
       if (mData != null)
         mData.delete();
       mData = retval;
    } else {
      if (mData == null) {
        mData = retval.copyReference();
      } else {
        if (!mData.equals(retval))
        {
          mData.delete();
          mData = retval.copyReference();
        }
      }
    }
    return retval;
  }
  
  /**
   * Sets the underlying buffer used by this object.
   * <p>
   * This is an advanced method and is not recommended for use by those
   * who don't fully understand how IBuffers work.  Implementations of
   * {@link MediaData} may behave in undefined ways if the buffer you
   * pass in is not big enough for what you ask them to do (e.g. they may
   * discard your buffer and allocate a larger one if they need more space).
   * It is up to the caller to ensure the buffer passed in is large enough,
   * and is not simultaneously in use by another part of the system.
   * </p>
   * @param buffer The buffer to set.
   */
  public void setData(io.humble.ferry.IBuffer buffer)
  {
    if (buffer == null)
    {
      if (mData != null)
        mData.delete();
      mData = buffer;
    } else {
      if (mData == null) {
        mData = buffer.copyReference();
      } else {
        if (!mData.equals(buffer))
        {
          mData.delete();
          mData = buffer.copyReference();
        }
      }
    }
    setData_internal(buffer);
  }

  /**
   * Get any underlying raw data available for this object, but do
   * not create a new Java wrapping object for it.
   *
   * <p>
   * Do not call {@link io.humble.ferry.RefCounted#delete()} on
   * the returned value. 
   * </p>
   * 
   * @return The raw data, or null if not accessible.
   * @since 3.2
   */
  public io.humble.ferry.IBuffer getDataCached()
  {
    if (mData == null)
      mData = getData_internal(); // Try crossing the boundary anyway
    return mData;
  }

  /**
   * Releases this instance of $javaclassname and frees any underlying
   * native memory.
   * <p>
   * {@inheritDoc}
   * </p> 
   */
  @Override
  public void delete()
  {
    if (mData != null) {
      mData.delete();
      mData = null;
    }
    super.delete();
  }


  
%}
%javamethodmodifiers io::humble::video::MediaData::getData "protected"
%javamethodmodifiers io::humble::video::MediaData::setData "protected"
%rename(getData_internal) io::humble::video::MediaData::getData;
%rename(setData_internal) io::humble::video::MediaData::setData;

%include <io/humble/video/MediaData.h>
