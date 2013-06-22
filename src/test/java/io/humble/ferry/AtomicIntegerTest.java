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

import org.junit.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.ferry.AtomicInteger;

import junit.framework.TestCase;

public class AtomicIntegerTest extends TestCase
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());

  private AtomicInteger ai;
  private int val;
  
  @Before
  public void setUp()
  {
    log.debug("Executing test case: {}", this.getName());
    ai = null;
    val = 0;
  }
  @Test
  public void testCreation()
  {
    ai = new AtomicInteger();
    val = ai.get();
    assertTrue("not initialized correctly", val == 0);
    
    ai = new AtomicInteger(5);
    val = ai.get();
    assertTrue("not intialized correctly", val == 5);
  }
  
  @Test
  @Ignore
  public void testIsAtomic()
  {
    ai = new AtomicInteger();
    assertTrue("is not using JVM for atomic locks", ai.isAtomic());
  }
  
  @Test
  public void testSet()
  {
    ai = new AtomicInteger(8);
    val = ai.get();
    assertTrue(val == 8);
    ai.set(15);
    val = ai.get();
    assertTrue(val == 15);
  }
  
  @Test
  public void testGetAndDoSomethingMethods()
  {
    ai = new AtomicInteger();
    
    val = ai.getAndAdd(5);
    assertTrue(val == 0);
    assertTrue(ai.get() == 5);
    
    val = ai.getAndIncrement();
    assertTrue(val == 5);
    assertTrue(ai.get() == 6);
    
    val = ai.getAndDecrement();
    assertTrue(val == 6);
    assertTrue(ai.get() == 5);
    
    val = ai.getAndSet(11);
    assertTrue(val == 5);
    assertTrue(ai.get() == 11);
  }
  
  @Test
  public void testDoSomethingAndGetMethods()
  {
    ai = new AtomicInteger();
    val = ai.addAndGet(5);
    assertTrue(val == 5);
    assertTrue(ai.get() == 5);
    
    val = ai.incrementAndGet();
    assertTrue(val == 6);
    assertTrue(ai.get() == 6);
    
    val = ai.decrementAndGet();
    assertTrue(val == 5);
    assertTrue(ai.get() == 5);    
  }

  @Test
  public void testCompareAndSet()
  {
    ai = new AtomicInteger(10);
    val = ai.get();

    assertTrue(val == 10);
    boolean result = false;
    
    result = ai.compareAndSet(5, 8);
    val = ai.get();
    assertTrue(result == false);
    assertTrue(val == 10);
    
    result = ai.compareAndSet(10, 8);
    val = ai.get();
    assertTrue(result == true);
    assertTrue(val == 8);
    
  }
  
  @Test
  public void testMultiThreadCompetition()
  {
    Thread threads[] = new Thread[10];
    int i = 0;
    
    ai = new AtomicInteger(0);
    
    for(i = 0; i< threads.length; i++)
    {
      // fire up each of our threads
      threads[i] = new Thread(new Runnable() {
        public void run() {
          for (int j = 0; j < 10000; j++)
            ai.incrementAndGet();
        }
      });
      threads[i].start();
    }

    for(i = 0; i< threads.length; i++)
    {
      // and wait for each of our threads to finish
      try {
        threads[i].join();
      } catch (InterruptedException ex)
      {
        log.debug("Got an interruption");
        fail("got an interruption");
      }
    }
    log.debug("AI final value = {}", ai.get());
    assertTrue("value not as expected", ai.get() == 100000);
  }
}
