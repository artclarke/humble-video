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

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.locks.ReentrantLock;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Internal Only.
 * <p>
 * This object allocates large memory chunks and returns them to native code.
 * The native memory then pins raw bytes based on the byte[]s returned here. The
 * net effect is that Java ends up thinking it actually allocated the memory,
 * but since a {@link RefCounted} object will also maintain a reference to this
 * allocator, you can use this to detect instances of 'leaked' references in
 * your Java code.
 * </p>
 * <p>
 * This function is called DIRECTLY from native code; names of methods MUST NOT
 * CHANGE.
 * </p>
 * 
 * @author aclarke
 * 
 */

public final class JNIMemoryAllocator
{
  private static final Logger log = LoggerFactory
      .getLogger(JNIMemoryAllocator.class);

  // don't allocate this unless we malloc; most classes don't even
  // touch this memory manager with a 100 foot pole
  final private Set<byte[]> mBuffers =
    new HashSet<byte[]>();
  final private ReentrantLock mLock = 
    new ReentrantLock();
  final static private int MAX_ALLOCATION_ATTEMPTS = 5;
  final static private double FALLBACK_TIME_DECAY = 1.5;
  final static private boolean SHOULD_RETRY_FAILED_ALLOCS = true;

  private void addToBuffer(byte[] mem)
  {
    mLock.lock();
    try
    {
      if (!mBuffers.add(mem))
      {
        assert false : "buffers already added";
      }
    }
    finally
    {
      mLock.unlock();
    }
  }

  private void removeFromBuffer(byte[] mem)
  {
    mLock.lock();
    try
    {
      if (!mBuffers.remove(mem))
      {
        assert false : "buffer not in memory";
      }
    }
    finally
    {
      mLock.unlock();
    }
  }
  
  /**
   * Not for use outside the package
   */
  JNIMemoryAllocator()
  {
    
  }
  
  /**
   * Internal Only.  Allocate a new block of bytes. Called from native code.
   * <p>
   * Will retry many times if it can't get memory, backing off
   * in timeouts to get there.
   * </p>
   * <p>
   * Callers must eventually call {@link #free(byte[])} when done
   * with the bytes or a leak will result.
   * </p>
   * @param size
   *          # of bytes requested
   * @return An array of size or more bytes long, or null on failure.
   */

  public byte[] malloc(int size)
  {
    byte[] retval = null;
    // first check the parachute
    JNIMemoryParachute.getParachute().packChute();
    try
    {
      if (SHOULD_RETRY_FAILED_ALLOCS)
      {
        int allocationAttempts = 0;
        int backoffTimeout = 10; // start at 10 milliseconds
        while (true)
        {
          try
          {
            // log.debug("attempting malloc of size: {}", size);
            retval = new byte[size];
            // log.debug("malloced block of size: {}", size);
            // we succeed, so break out
            break;
          }
          catch (final OutOfMemoryError e)
          {
            // try clearing our queue now and do it again. Why?
            // because the first failure may have allowed us to
            // catch a RefCounted no longer in use, and the second
            // attempt may have freed that memory.

            // do a JNI collect before the alloc
            ++allocationAttempts;
            if (allocationAttempts >= MAX_ALLOCATION_ATTEMPTS)
            {
              // try pulling our rip cord
              JNIMemoryParachute.getParachute().pullCord();
              // do one last "hope gc" to free our own memory
              JNIReference.getMgr().gcInternal();
              // and throw the error back to the native code
              throw e;
            }

            log.debug("retrying ({}) allocation of {} bytes",
                allocationAttempts, size);
            try
            {
              // give the finalizer a chance
              if (allocationAttempts <= 1)
              {
                // first just yield
                Thread.yield();
              }
              else
              {
                Thread.sleep(backoffTimeout);
                // and slowly get longer...
                backoffTimeout = (int) (backoffTimeout * FALLBACK_TIME_DECAY);
              }
            }
            catch (InterruptedException e1)
            {
              // reset the interruption so underlying
              // code can also interrupt
              Thread.currentThread().interrupt();
              // and throw the error condition
              throw e;
            }
            // do a JNI collect before the alloc
            JNIReference.getMgr().gcInternal();
          }
        }
      }
      else
      {
        retval = new byte[size];
      }
      addToBuffer(retval);
      retval[retval.length - 1] = 0;
      
//      log.debug("malloc: {}({}:{})", new Object[]
//      {
//          retval.hashCode(), retval.length, size
//      });
      
    }
    catch (Throwable t)
    {
      // do not let an exception leak out since we go back to native code.
      retval = null;
    }
    return retval;
  }

  /**
   * Free memory allocated by the {@link #malloc(int)} method.
   * Called from native code.
   * 
   * @param mem
   *          the byes to be freed.
   */

  public void free(byte[] mem)
  {
    removeFromBuffer(mem);
//      log.debug("free:   {}({})", mem.hashCode(), mem.length);
  }

  /**
   * Internal Only.  Native method that tells a native objects (represented by the nativeObj
   * long pointer val) that this JNIMemoryAllocator is being used to allocate
   * it's large blocks of memory.
   * <p>
   * Follow that? No. That's OK... you really don't want to know.
   * </p>
   * 
   * @param nativeObj
   *          A C pointer (a la swig).
   * @param mgr
   *          Us.
   */

  public native static void setAllocator(long nativeObj, JNIMemoryAllocator mgr);

  /**
   * Internal Only.  Get the allocator for the underlying native pointer.
   * 
   * @param nativeObj
   *          The native pointer.
   * @return The allocator to use, or null.
   */

  public native static JNIMemoryAllocator getAllocator(long nativeObj);

}
