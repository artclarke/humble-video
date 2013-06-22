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

import java.util.Collection;
import java.util.LinkedList;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import io.humble.ferry.RefCountedTester;
import io.humble.ferry.JNIReference;
import io.humble.ferry.JNIMemoryManager.MemoryModel;

@RunWith(Parameterized.class)
public class RefCountedTest
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

  public RefCountedTest(JNIMemoryManager.MemoryModel model)
  {
    JNIMemoryManager.setMemoryModel(model);
  }
   
  
  @After
  public void tearDown()
  {
    // Always do a collection on tear down, as we want to find leaked objects
    // and don't want things easily collectable screwing up hprof if running
    System.gc();
  }
  
  @AfterClass
  public static void tearDownClass() throws InterruptedException
  {
    System.gc();
    Thread.sleep(1000);
    JNIReference.getMgr().gc();
  }
  
  @Test
  public void testCorrectStartingRefCount()
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
    RefCountedTester obj = RefCountedTester.make();
    assertEquals("starting ref count", 1, obj.getCurrentRefCount());
    obj.delete();
  }
  
  @Test
  public void testJavaCopyKeepsRefcountConstant()
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
    RefCountedTester obj = RefCountedTester.make();
    assertEquals("starting ref count", 1, obj.getCurrentRefCount());
    RefCountedTester javaCopy = obj;
    assertEquals("java copy should keep ref the same", 1, javaCopy.getCurrentRefCount());
    javaCopy.delete();
    obj.delete();
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
  }
  
  @Test(timeout=20*1000)
  public void testNativeCopyRefcountIncrement() throws InterruptedException
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
    RefCountedTester obj = RefCountedTester.make();
    assertEquals("starting ref count", 1, obj.getCurrentRefCount());
    RefCountedTester nativeCopy = RefCountedTester.make(obj);
    assertEquals("native copy should increment", 2, obj.getCurrentRefCount());
    assertEquals("native copy should increment", 2, nativeCopy.getCurrentRefCount());
    nativeCopy.delete();
    nativeCopy = null;
    assertEquals("native copy should be decremented", 1, obj.getCurrentRefCount());
    obj.delete();
    obj = null;
    while(JNIReference.getMgr().getNumPinnedObjects() > 0)
    {
      byte[] bytes = new byte[1024*1024];
      bytes[0] = 0;
      JNIReference.getMgr().gc();
    }
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
  }
  
  @Test(timeout=20*1000)
  public void testCopyByReference() throws InterruptedException
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());
    RefCountedTester obj1 = RefCountedTester.make();
    
    RefCountedTester obj2 = obj1.copyReference();
    
    assertTrue("should look like different objects", obj1 != obj2);
    assertTrue("should be equal though", obj1.equals(obj2));
    assertEquals("should have same ref count", obj1.getCurrentRefCount(), obj2.getCurrentRefCount());
    assertEquals("should have ref count of 2", 2, obj2.getCurrentRefCount());
    
    obj1.delete(); obj1 = null;
    assertEquals("should now have refcount of 1", 1, obj2.getCurrentRefCount());
    obj2.delete();
    obj2 = null;
    while(JNIReference.getMgr().getNumPinnedObjects() > 0)
    {
      byte[] bytes = new byte[1024*1024];
      bytes[0] = 0;
      JNIReference.getMgr().gc();
    }
    assertEquals("should be no objects for collection", 0, JNIReference.getMgr().getNumPinnedObjects());    
  }
  
  /**
   * This test tries to make sure the Java gargage collector
   * eventually forces a collection of RefCounted objects, even
   * if we don't call our own mini-gc methods.
   * <p>
   * It's disabled though because it can occasionally fail due
   * to Java just deciding it doesn't need to collect yet.  But
   * the fact that it passes sometimes does show that a collection
   * will occur.
   * @throws InterruptedException if interrupted
   */
  @Test(timeout=20*1000)
  public void testGarbageCollectionDoesEventuallyReleaseNativeReferences()
  throws InterruptedException
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());

    RefCountedTester obj1 = RefCountedTester.make();
    
    RefCountedTester obj2 = obj1.copyReference();
    
    assertTrue("should look like different objects", obj1 != obj2);
    assertTrue("should be equal though", obj1.equals(obj2));
    assertEquals("should have same ref count", obj1.getCurrentRefCount(), obj2.getCurrentRefCount());
    assertEquals("should have ref count of 2", 2, obj2.getCurrentRefCount());
    
    obj1 = null;
    
    // obj1 should now be unreachable, so if we try a Garbage collection it should get caught.
    while(JNIReference.getMgr().getNumPinnedObjects() > 1)
    {
      byte[] bytes = new byte[1024*1024];
      bytes[0] = 0;
      JNIReference.getMgr().gc();
    }
    // at this point the Java proxy object will be unreachable, but should be sitting in the
    // reference queue and also awaiting finalization.  The finalization should have occurred by now.
    assertEquals("should be only the first object for collection",
        1, JNIReference.getMgr().getNumPinnedObjects());        
    assertEquals("should have a ref refcount of 1", 1, obj2.getCurrentRefCount());

    obj2.delete();
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());

  }
  
  @Test(timeout=20*1000)
  public void testJNIWeakReferenceFlushQueue() throws InterruptedException
  {
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());

    RefCountedTester obj1 = RefCountedTester.make();
    assertEquals("should be no objects for collection", 
        1, JNIReference.getMgr().getNumPinnedObjects());
    
    RefCountedTester obj2 = obj1.copyReference();
    assertEquals("should be no objects for collection", 
        2, JNIReference.getMgr().getNumPinnedObjects());
    
    assertTrue("should look like different objects", obj1 != obj2);
    assertTrue("should be equal though", obj1.equals(obj2));
    assertEquals("should have same ref count",
        obj1.getCurrentRefCount(), obj2.getCurrentRefCount());
    assertEquals("should have ref count of 2", 2, obj2.getCurrentRefCount());
    
    assertEquals("should be no objects for collection", 
        2, JNIReference.getMgr().getNumPinnedObjects());

    //obj1.delete();
    obj1 = null;
    
    // obj1 should now be unreachable, so if we try a Garbage collection it should get caught.
    while(obj2.getCurrentRefCount() > 1)
    {
      byte[] bytes = new byte[1024*1024];
      bytes[0] = 0;
      JNIReference.getMgr().gc();
    }
    assertEquals("should now have a ref refcount of 1", 1, obj2.getCurrentRefCount());
    assertEquals("should be only the first object for collection",
        1, JNIReference.getMgr().getNumPinnedObjects());

    obj2.delete();
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());

  }
  
  /**
   * Tests that calling .delete() on a RefCounted object and
   * then derefing it raises an exception, but does not crash
   * the JVM
   */
  @Test(expected=NullPointerException.class)
  public void testDeleteThenCallRaisesException()
  {
    RefCountedTester obj = RefCountedTester.make();
    obj.delete();
    // this should raise an exception
    obj.getCurrentRefCount();
    assertEquals("should be no objects for collection", 
        0, JNIReference.getMgr().getNumPinnedObjects());

  }
 
}
