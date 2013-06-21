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
 * Internal Only.  The JNIMemoryParachute is a very weird beast.  Don't look here.
 * <p>
 * Damn it... I told you not to.  So in normal cases Java handles out
 * of memory conditions by throwing an {@link OutOfMemoryError}.  However
 * when you add JNI to the mix, things get complicated.  In the ferry
 * system Java calls native code which in turns calls Java code.
 * <p>
 * In fact sometimes native code calls back into Java code to allocate
 * memory (using the {@link JNIMemoryAllocator}).  However a problem occurs
 * if the {@link JNIMemoryAllocator} runs out of heap memory -- it throws
 * an OutOfMemoryError back into the native code.
 * </p>
 * <p>
 * The native code is smart enough to see that, and to just return back to
 * it's calling Java code where the {@link OutOfMemoryError} should be
 * thrown again.
 * </p>
 * <p>
 * The problem is in the various JNI implementations -- they sometimes need
 * to allocate heap memory just to return from native code.  When they attempt
 * to do that to return, they run into the original {@link OutOfMemoryError},
 * and fail.  The error differs depending on JVM.  On Sun's Java JRE for
 * Windows it can result in Windows terminating the JVM.  On Linux it can
 * result in Java terminating itself with an uncatchable {@link OutOfMemoryError}.
 * </p>
 * <p>
 * So enter the {@link JNIMemoryParachute}.  The {@link JNIMemoryAllocator}
 * allocates an instance of a {@link JNIMemoryParachute} at startup.  The
 * {@link JNIMemoryParachute} just holds on to some heap memory (about 10k)
 * "just in case".  Then if the {@link JNIMemoryAllocator} fails to allocate,
 * it pulls the rip cord on the parachute, and forces a Garbage Collect, before
 * returning the JNI.  The hope is that 10K is enough heap space for the JNI
 * implementation to correctly return to the underlying Java layer and handle
 * the original {@link OutOfMemoryError}.
 * </p>
 * <p>Now this is really just a hail mary (at the expense of 10k of memory) -
 * you can still get failures if multiple threads are all trying to go through
 * the JNI layer at the same time, but this helps minimize the odds of that
 * long enough for you to catch an error.</p>
 * <p>See.  Told you to not read.</p>
 * 
 * @author aclarke
 *
 */
final class JNIMemoryParachute
{
  private final static JNIMemoryParachute mParachute;
  static {
    mParachute = new JNIMemoryParachute();
  }

  /**
   * Factory method for the parachute -- do not cache;  use this so that
   * we have a nice option for provider either thread-specific
   * or global parachutes.
   * 
   * @return a parachute to use for this memory request.
   */
  static JNIMemoryParachute getParachute()
  {
    return mParachute;
  }
  
  private volatile byte[] mPayload;
  private final int PAYLOAD_BYTES=10000;
  private JNIMemoryParachute()
  {
    // create a payload on first creation
    packChute();
  }
  
  
  /**
   * Checks the current parachute and recreates (reloads) our payload
   * if needed.
   * @return true on success pretty much all the time because...
   * @throws OutOfMemoryError if we can't reload the parachute
   */
  boolean packChute() throws OutOfMemoryError
  {
    if (mPayload != null)
      return true;
    
    // otherwise the payload is null
    synchronized(this)
    {
      if (mPayload != null)
        return true;
      try {
        //System.out.println("Packing the parachute! this=" + this);
        mPayload = new byte[PAYLOAD_BYTES];
        mPayload[0] = 'P';
        mPayload[1] = 'A';
        mPayload[2] = 'R';
        mPayload[3] = 'A';
        mPayload[4] = 'C';
        mPayload[5] = 'H';
        mPayload[6] = 'U';
        mPayload[7] = 'T';
        mPayload[8] = 'E';
        mPayload[9] = '!';
        return true;
      } catch (OutOfMemoryError e) {
        // we failed to create the parachute.  Also known as bad.
        // forward it on.
        throw e;
      }
    }
  }
  
  void pullCord()
  {
    // do one check outside the lock to avoid a sync if we're
    // not loaded.
    if (mPayload == null)
      return;
    synchronized(this)
    {
      if (mPayload == null)
        return;
      //System.out.println("Ripping the cord! this=" + this);
      // otherwise, we're going to free up our payload,
      // and do a quick allocation to suggest to Java to do
      // a GC if needed
      mPayload = null;
      try {
        byte[] discardBytes = new byte[4];
        discardBytes[1]=2;
      } catch (OutOfMemoryError e) {
        // ignore because the caller will deal with that.
        do {
          
        } while(false);
      }
    }
  }
  
}
