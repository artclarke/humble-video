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

import io.humble.ferry.JNIMemoryManager;

import java.util.LinkedList;

public class MemoryTestHelper
{

  public static void forceJavaHeapWeakReferenceClear()
  {
    LinkedList<byte[]> leakedBytes = new LinkedList<byte[]>();
    try
    {
      while(true) {
        JNIMemoryManager.collect();
        leakedBytes.add(new byte[1024*1024]);
      }
    } catch (OutOfMemoryError e) {
    }
    leakedBytes.clear();
    // and try one more allocation
    byte[] lastAlloc = new byte[1024];
    lastAlloc[0] = 9;
    JNIMemoryManager.collect();
  }
}
