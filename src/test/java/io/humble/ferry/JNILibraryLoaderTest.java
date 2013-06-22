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

import java.util.List;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.ferry.JNILibraryLoader;
import io.humble.ferry.JNIReference;

public class JNILibraryLoaderTest
{
  private static final Logger log = LoggerFactory.getLogger(JNILibraryLoaderTest.class);
  @Before
  public void setUp()
  {
    JNILibraryLoader loader = JNILibraryLoader.getInstance();
    loader.setOS(null); // reset the OS
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
  public void testCreation()
  {
    JNILibraryLoader loader = JNILibraryLoader.getInstance();
    assertNotNull(loader);
  }
  
  @Test
  public void testGetOS()
  {
    JNILibraryLoader loader = JNILibraryLoader.getInstance();
    JNILibraryLoader.OSName os = loader.getOS();
    switch(os)
    {
    case Linux:
      assertTrue(System.getProperty("os.name", "").startsWith("Linux"));
      break;
    case MacOSX:
      assertTrue(System.getProperty("os.name", "").startsWith("Mac"));
      break;
    case Windows:
      assertTrue(System.getProperty("os.name", "").startsWith("Windows"));
      break;
    case Unknown:
      fail("should default to linux");
      break;
    }
  }
  
  @Test
  public void testGetSystemRuntimeLibraryPathVar()
  {
    JNILibraryLoader.OSName[] osTypes = new JNILibraryLoader.OSName[]{
        JNILibraryLoader.OSName.Linux,
        JNILibraryLoader.OSName.Windows,
        JNILibraryLoader.OSName.MacOSX
    };
    String varNames[] = new String[]{
        "LD_LIBRARY_PATH",
        "PATH",
        "DYLD_LIBRARY_PATH"
    };

    for(int i = 0; i < varNames.length; i++)
    {
      String expected = varNames[i];
      JNILibraryLoader.OSName os = osTypes[i];
      JNILibraryLoader loader = JNILibraryLoader.getInstance();
      loader.setOS(os);
      String actual = loader.getSystemRuntimeLibraryPathVar();
      log.trace("Comparing System Runtime Library Path: {} vs {}", expected, actual);
      assertEquals("unexpected variable name", expected, actual);
    }
  }

  /**
   * This test just prints the generated values out to the log file; it should
   * always succeed, but you should eye-ball the results to make sure they are right.
   */
  @Test
  public void testGenerateFilenamesLinux()
  {
    JNILibraryLoader loader = JNILibraryLoader.getInstance();
    List<String> files = loader.getLibraryCandidates("foo", new Long("1"));
    for(String file: files)
    {
      log.debug("candidate: {}", file);
    }
  }
}
