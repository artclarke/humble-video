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

/**
 * 
 * Internal Only.
 * 
 * An Opaque handle that holds a Native mPointer/handle.
 * 
 * This class holds a opaque long value that can be used to pass into JNI C
 * function that expect a ** in C.
 * <p>
 * For example, a function like this in native code:
 * </p>
 * <code>
 * <pre>
 *   int url_open(URLContext **handlePtr, char *name, int flags);
 *   int url_read(URLContext *handle, void* buf, int len);
 * </pre>
 * </code> might map to the following Java method: <code>
 * <pre>
 * public class Example {
 *   public native int url_open(JNIPointerReference p, String name, int flags);
 *   public native int url_read(JNIPointerReference p, byte[] buf, int len);
 *   public void example() {
 *      int retval = 0;
 *      JNIPointerReference p;
 *      // p.setPointer is called by the native function 
 *      retval = url_open(p, "foo", 0);
 *      
 *      // p.getPointer is called by the native function
 *      byte[] buf = new bytes[1024];
 *      retval = url_read(p, buf, buf.length);
 *   }
 * </pre>
 * </code>
 * 
 * <p>
 * <b>IMPORTANT: DO NOT RENAME THIS CLASS OR METHODS IN IT. NATIVE CODE DEPENDS
 * ON THE NAMES AND SIGNATURES.</b>
 * </p>
 * 
 */
public class JNIPointerReference
{
  private long mPointer;

  /**
   * Internal Only.
   */
  public JNIPointerReference()
  {
    mPointer = -1;
  }
  
  @SuppressWarnings("unused")
  // This method is "private" but we assume it'll be called from
  // native code (that can override protections).
  private synchronized long getPointer()
  {
    return mPointer;
  }

  // This method is "private" but we assume it'll be called from
  // native code (that can override protections).
  @SuppressWarnings("unused")
  private synchronized long setPointer(long newVal)
  {
    long oldVal = mPointer;
    mPointer = newVal;
    return oldVal;
  }

  /**
   * Internal Only.
   */
  public String toString()
  {
    return "native:" + mPointer;
  }
}
