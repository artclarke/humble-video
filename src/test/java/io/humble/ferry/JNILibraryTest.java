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

import static org.junit.Assert.fail;

import org.junit.Test;

public class JNILibraryTest
{
  @Test
  public void testLoadSuccess()
  {
    final JNILibrary library = new JNILibrary("humble-video",
        new Long(0));
    JNILibrary.load("humble-video", library);
  }
  @Test
  public void testLoadFail()
  {
    final JNILibrary library = new JNILibrary("humble-notavalidlibrary",
        new Long(0));
    try {
      JNILibrary.load("humble-video", library);
      fail("library should not load");
    } catch (UnsatisfiedLinkError e) {
      // this is success; any other exception should bubble out and fail
    }
  }
  
}
