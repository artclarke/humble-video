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

package io.humble.ferry_test;

import static org.junit.Assert.*;
import io.humble.ferry.JNIMemoryManager;
import io.humble.ferry.JNIReference;
import io.humble.ferry.RefCounted;
import io.humble.ferry.RefCountedTester;
import io.humble.ferry.JNIMemoryManager.MemoryModel;

import java.util.Collection;
import java.util.LinkedList;
import java.util.concurrent.atomic.AtomicBoolean;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;


@RunWith(Parameterized.class)
public class RefCountedExhaustiveTest
{
  @Parameters
  public static Collection<Object[]> getModels()
  {
    Collection<Object[]> retval = new LinkedList<Object[]>();
    // add all the models.
    for(MemoryModel model: JNIMemoryManager.MemoryModel.values())
      retval.add(new Object[]{
          model
      });
    return retval;
  }

  public RefCountedExhaustiveTest(JNIMemoryManager.MemoryModel model)
  {
    JNIMemoryManager.setMemoryModel(model);
  }
  
  @Before
  public void setUp()
  {
    JNIMemoryManager.getMgr().flush();
  }
   
  @Test(timeout=60*1000)
  public void testReferenceCountingLoadTestOfDeath() throws InterruptedException
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
    RefCountedTester obj = RefCountedTester.make();
    for(int i = 0; i < 1000; i++)
    {
      RefCountedTester copy = obj.copyReference();
      assertNotNull("could not copy reference", copy);
    }
    obj=null;
    
    // Force a java collection
    while(JNIReference.getMgr().getNumPinnedObjects() > 0)
      MemoryTestHelper.forceJavaHeapWeakReferenceClear();
    
    assertEquals("Looks like we leaked an object",
        0, JNIReference.getMgr().getNumPinnedObjects());        

  }

  @Test
  public void testCopyReferenceLoadTest()
  {
    assertEquals(0, JNIMemoryManager.getMgr().getNumPinnedObjects());

    RefCounted obj = RefCountedTester.make();
    
    for(int i = 0; i < 100000; i++)
    {
      RefCounted copy = obj.copyReference();
      copy.delete();
    }
    obj.delete();
    assertEquals(0, JNIMemoryManager.getMgr().getNumPinnedObjects());
  }
  
  @Test
  public void testCopyReferenceLoadTestMultiThreaded() throws InterruptedException
  {
    assertEquals(0, JNIMemoryManager.getMgr().getNumPinnedObjects());
    final RefCounted obj = RefCountedTester.make();

    final int NUM_THREADS=100;
    final int NUM_ITERS=10000;
    final AtomicBoolean start = new AtomicBoolean(false);
    
    Thread[] threads = new Thread[NUM_THREADS];
    for(int i = 0; i < threads.length; i++)
    {
      threads[i] = new Thread(
          new Runnable(){
            public void run()
            {
              synchronized(start)
              {
                while(!start.get())
                  try
                  {
                    start.wait();
                  }
                  catch (InterruptedException e)
                  {
                    Thread.currentThread().interrupt();
                  }
              }
//              System.out.println("Thread started: "+Thread.currentThread().getName());
              for(int i = 0; i < NUM_ITERS; i++)
              {
                RefCounted copy = obj.copyReference();
                copy.delete();
              }
            }},
          "thread_"+i);
    }
    for(int i = 0; i < threads.length; i++)
    {
      threads[i].start();
    }
    synchronized(start)
    {
      start.set(true);
      start.notifyAll();
    }
    for(int i = 0; i < threads.length; i++)
    {
      threads[i].join();
//      System.out.println("Thread finished: "+threads[i].getName());
    }
    obj.delete();
    assertEquals(0, JNIMemoryManager.getMgr().getNumPinnedObjects());
  }
  
  @Test(timeout=5*60*1000)
  public void testJNIMemoryManagerHeapExpansion()
  {
    LinkedList<RefCountedTester> heldRefs = new LinkedList<RefCountedTester>();
    JNIMemoryManager mgr = JNIMemoryManager.getMgr();
    mgr.flush();
    mgr.setMinimumReferencesToCache(1024);
    int maxItems = 10000;
    // 10000 should cause several heap expansions to occur
    for(int i = 0; i < maxItems; i++)
    {
      heldRefs.add(RefCountedTester.make());
    }
    assertEquals("didn't pin as many as it should", maxItems, mgr.getNumPinnedObjects());
    // now release them.
    heldRefs.clear();
    while(mgr.getNumPinnedObjects() != 0) {
      MemoryTestHelper.forceJavaHeapWeakReferenceClear();
      // Do a collection
      mgr.gc(true);
    }
    assertEquals("didn't pin as many as it should", 0, mgr.getNumPinnedObjects());
    // this should cause the heap to shrink, and then grow
    for(int i = 0; i < maxItems/2; i++)
    {
      heldRefs.add(RefCountedTester.make());
    }
    assertEquals("didn't pin as many as it should", maxItems/2, mgr.getNumPinnedObjects());
    // now release them.
    heldRefs.clear();
    // and force a collection
    while(mgr.getNumPinnedObjects() != 0) {
      MemoryTestHelper.forceJavaHeapWeakReferenceClear();
      // Do a collection
      mgr.gc(true);
    }
    assertEquals("didn't pin as many as it should", 0, mgr.getNumPinnedObjects());
  }
  
}
