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

import java.util.concurrent.atomic.AtomicReference;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Proxy for {@link java.lang.Thread} that is called by Ferry native code.
 * <p>
 * This class is called from native code to tell if a thread
 * is interrupted.  It only exposes the methods that are called
 * from the native code, and adds logging to aid in debugging.
 * </p>
 * <p>
 * If allows a global handler to be set.  This can be useful for
 * catching threads that are started from native code as there is no other
 * way to detect those and interrupt them.  If set, <b>the global handler
 * will always be checked BEFORE the local thread</b> allowing users to 
 * always get a callback on each interrupt check if they want.
 * </p>
 * <p>
 * Do not change the name or location of this class without also
 * changing the JNIHelper.cpp code that refers to it.
 * </p>
 * @author aclarke
 * @since 5.0
 */
public class JNIThreadProxy extends Thread
{
  /**
   * Interface that global interrupt handlers should use.
   * @author aclarke
   *
   */
  public interface Interruptable {
    /**
     * Called BEFORE native code calls {@link java.lang.Thread#interrupted()}.
     * <p>
     * Please note that this method can be called from any thread
     * and it is the responsibility of the caller to ensure it is
     * thread safe.
     * </p>
     * 
     * @return true if the JNIThreadProxy should continue on an call {@link java.lang.Thread#interrupted()}.  false to stop 
     *   processing now (and tell callers the thread is NOT interrupted).
     */
    public boolean preInterruptCheck();
    
    /**
     * Called AFTER native code calls {@link java.lang.Thread#interrupted()} and allows
     * the caller to override the returned value if desired.
     * @param interruptStatus The value returned by {@link Thread#interrupted()} on the current thread
     * 
     * @return The value you actually want to return to native code.  Allows you to override the value returned.
     */
    public boolean postInterruptCheck(boolean interruptStatus);
  }
  private static final AtomicReference<Interruptable> mGlobalHandler = new AtomicReference<JNIThreadProxy.Interruptable>();
  
  private static final Logger log = LoggerFactory
      .getLogger(JNIThreadProxy.class);
  
  private static final ThreadLocal<JNIThreadProxy> mThreads = new ThreadLocal<JNIThreadProxy>();
  private final Thread mThread;
  private JNIThreadProxy(Thread thread)
  {
    mThread = thread;
  }
  
  /**
   * Gets the current global interrupt handler
   * @return The handler, or null if not set.
   */
  public static Interruptable getGlobalInterruptHandler()
  {
    return mGlobalHandler.get();
  }
  /**
   * Set a new global interrupt handler.
   * @param handler The new handler to use, or null to disable.
   * @return The previous handler, or null if none is set.
   */
  public static Interruptable setGlobalInterruptable(Interruptable handler)
  {
    return mGlobalHandler.getAndSet(handler);
  }
  /**
   * @see java.lang.Thread#currentThread
   * @return java.lang.Thread
   */
  public static Thread currentThread()
  {
    JNIThreadProxy retval = mThreads.get();
    if (retval == null) {
      retval = new JNIThreadProxy(Thread.currentThread());
      // store it in thread local storage
      mThreads.set(retval);
    }
    log.trace("currentThread: {}", retval.mThread);
    return retval;
  }
  
  /**
   * {@inheritDoc}
   */
  @Override
  public boolean isInterrupted()
  {
    final Thread thread = mThreads.get().mThread;
    final Interruptable handler = getGlobalInterruptHandler();
    boolean retval = false;
    if (handler != null) {
      retval = handler.preInterruptCheck();
      if (!retval)
        return retval;
    }
    retval = thread.isInterrupted();
    if (handler != null) {
      retval = handler.postInterruptCheck(retval);
    }
    return retval;
  }
  
  /**
   * {@inheritDoc}
   */
  @Override
  public void interrupt()
  {
    final Thread thread = mThreads.get().mThread;
    log.trace("interrupt (thread {})", thread);
    thread.interrupt();
    return;
  }
}
