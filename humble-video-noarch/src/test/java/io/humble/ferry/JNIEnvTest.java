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

import org.junit.Test;

public class JNIEnvTest
{

  @Test
  public void testGetEnv()
  {
    assertNotNull(JNIEnv.getEnv());
  }
  @Test
  public void testGetOSFamily()
  {
    assertNotSame(JNIEnv.OSFamily.UNKNOWN, JNIEnv.getEnv().getOSFamily());
    assertEquals(JNIEnv.OSFamily.UNKNOWN, JNIEnv.getOSFamily(null));
    assertEquals(JNIEnv.OSFamily.UNKNOWN, JNIEnv.getOSFamily(""));
    assertEquals(JNIEnv.OSFamily.LINUX, JNIEnv.getOSFamily("Linux"));
    assertEquals(JNIEnv.OSFamily.WINDOWS, JNIEnv.getOSFamily("Windows"));
    assertEquals(JNIEnv.OSFamily.MAC, JNIEnv.getOSFamily("Mac"));
  }
  @Test
  public void testGetCPUArch()
  {
    assertNotSame(JNIEnv.CPUArch.UNKNOWN, JNIEnv.getEnv().getCPUArch());
    assertEquals(JNIEnv.CPUArch.UNKNOWN, JNIEnv.getCPUArch(null));
    assertEquals(JNIEnv.CPUArch.UNKNOWN, JNIEnv.getCPUArch(""));
    assertEquals(JNIEnv.CPUArch.X86, JNIEnv.getCPUArch("i386"));
    assertEquals(JNIEnv.CPUArch.X86_64, JNIEnv.getCPUArch("amd64"));
    assertEquals(JNIEnv.CPUArch.PPC, JNIEnv.getCPUArch("PowerPC"));
    assertEquals(JNIEnv.CPUArch.PPC64, JNIEnv.getCPUArch("PowerPC64"));

  }
}
