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

import static org.junit.Assert.*;
import org.junit.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.ferry.Mutex;

public class MutexTest
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  private Mutex mMutex=null;
  private boolean mTestFail = false;
  @Before
  public void setUp()
  {
    if (mMutex != null)
      mMutex.delete();
    mMutex = Mutex.make();
    assertTrue("didn't get a mutex", mMutex != null);
    mTestFail = false;
  }

  @Test
  public void testUncompetitiveLockAndUnlock()
  {
    long before = -1;
    long after = -1;
    // this should be VERY quick...
    before = System.currentTimeMillis();
    for (int i = 0; i<10;i++)
    {
      mMutex.lock();
      mMutex.unlock();
    }
    after = System.currentTimeMillis();
    assertTrue("not really fast", after-before < 100);    
  }
  
  @Test(timeout=5000)
  public void testCompetitiveLockAndUnlock()
  {
    long before = -1;
    long after = -1;
    Thread competitor = new Thread(new Runnable() {
      public void run()
      {
        log.debug("Competitor is running");
        mMutex.lock();
        log.debug("Got the lock");

        synchronized(mMutex) {
          // let the other thread know we have the lock
          mMutex.notifyAll();
          log.debug("Notified other thread");
        }

        try {
          log.debug("Going to sleep");

          Thread.sleep(1000);
        }
        catch (InterruptedException ex)
        {
          log.error("interrupted while sleeping");
          mTestFail = true;
        }
        mMutex.unlock();
        log.debug("And exiting the thread");

      }
    }, "Competitor");
    // this should be VERY slow...
    before = System.currentTimeMillis();
    synchronized(mMutex) {
      try {
        log.debug("Starting the competitor");
        competitor.start();
        log.debug("Waiting for the competitor to get the lock");
        mMutex.wait();
      }
      catch (InterruptedException ex)
      {
        fail("was interrupted");
      }
    }
    // now we've been signaled that the other thread
    // has the lock. try to take it (should block).
    log.debug("Trying the lock... should wait...");
    mMutex.lock();
    log.debug("Got the lock.");
    after = System.currentTimeMillis();
    assertTrue("too fast", after-before > 500);
    try {
      competitor.join();
    }
    catch (InterruptedException ex)
    {
      fail("was interrupted");
    }
    log.debug("releasing the lock");
    mMutex.unlock();
    assertTrue("we appear to have failed in the other thread", !mTestFail);
  }
}
