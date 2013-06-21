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
 * A set of utilities that can be used to find out
 * information about the Native environment that
 * we are running within.
 * @author aclarke
 *
 */
public class JNIEnv
{
  /**
   * The CPU architecture (i.e. chip architecture).
   * @author aclarke
   *
   */
  enum CPUArch{
    /** 32-bit systems such as the i386, i486, i586 or i686 */
    X86,
    /** 64-bit systems based on the x86 family */
    X86_64,
    /** 32-bit PowerPC based systems */
    PPC,
    /** 64-bit PowerPC based systems */
    PPC64,
    /** A chip architecture unknown to {@link Ferry}. */
    UNKNOWN,
  }
  /** The Operating system family */
  enum OSFamily {
    /** Linux family of operating systems */
    LINUX,
    /** Macintosh (darwin) family of operating systems */
    MAC,
    /** Microsoft Windows family of operating systems */
    WINDOWS,
    /** An unknown OS that {@link Ferry} knows nothing about */
    UNKNOWN,
  }
  /**
   * Our singleton.
   */
  private static final JNIEnv mEnv = new JNIEnv();
  
  private final CPUArch mCPUArch;
  private final OSFamily mOSFamily;
  
  /** Private. Only one {@link JNIEnv} is allowed per Java Virtual machine. */
  private JNIEnv() {
    mCPUArch = getCPUArch(System.getProperty("os.arch"));
    mOSFamily = getOSFamily(System.getProperty("os.name"));
  }
  
  /**
   * Get the (static) {@link JNIEnv} for this JVM.
   * @return the environment
   */
  public static JNIEnv getEnv() { return mEnv; }

  /**
   * Get the CPU architecture based on the passed in javaCPUArch specifier.
   * 
   * The string should be of a format returned from {@link System#getProperty(String)}
   * for the property "os.arch".
   * 
   * @param javaCPU the string to parse
   * @return the CPU architecture
   * @see System#getProperty(String)
   */
  public static CPUArch getCPUArch(String javaCPU)
  {
    final CPUArch javaArch;
    final String javaCPUArch = javaCPU != null ? javaCPU.toLowerCase() : "";
    
    // first parse the java arch
    if (javaCPUArch.startsWith("x86_64") ||
        javaCPUArch.startsWith("amd64") ||
        javaCPUArch.startsWith("ia64")) {
      javaArch = CPUArch.X86_64;
    } else if (
        javaCPUArch.startsWith("ppc64") ||
        javaCPUArch.startsWith("powerpc64")
        ) {
      javaArch = CPUArch.PPC64;
    } else if (
        javaCPUArch.startsWith("ppc") ||
        javaCPUArch.startsWith("powerpc")
        ) {
      javaArch = CPUArch.PPC;
    } else if (
        javaCPUArch.contains("86")
        )
    {
      javaArch = CPUArch.X86;
    } else {
      javaArch = CPUArch.UNKNOWN;
    }
    return javaArch;
  }
  
  /**
   * Return a CPUArch from parsing a GNU autoconf triple.
   * 
   * For example "x86_64-w64-mingw32" will return {@link JNIEnv.CPUArch#X86_64}
   * and "ppc-apple-darwin" will return {@link JNIEnv.CPUArch#PPC}.
   * 
   * @param gnuString the GNU string
   * @return the architecture
   */
  public static CPUArch getCPUArchFromGNUString(String gnuString)
  {
    final String nativeCpu = gnuString.toLowerCase();
    final CPUArch nativeArch;
    // then the native arch
    if (nativeCpu.startsWith("x86_64") ||
        nativeCpu.startsWith("amd64") ||
        nativeCpu.startsWith("ia64"))
      nativeArch = CPUArch.X86_64;
    else if (
        nativeCpu.startsWith("ppc64") ||
        nativeCpu.startsWith("powerpc64")
        )
      nativeArch = CPUArch.PPC64;
    else if (
        nativeCpu.startsWith("ppc") ||
        nativeCpu.startsWith("powerpc")
        )
      nativeArch = CPUArch.PPC;
    else if (
        nativeCpu.contains("86")
        )
      nativeArch = CPUArch.X86;
    else
      nativeArch = CPUArch.UNKNOWN;
 
    return nativeArch;
  }
  
  /**
   * Get the OSFamily based on the passed in osName specifier.
   * 
   * The string should be of a format returned from {@link System#getProperty(String)}
   * for the property "os.name".
   * 
   * @param osName the string to parse
   * @return the OSFamily
   * @see System#getProperty(String)
   */
  public static OSFamily getOSFamily(String osName)
  {
    final OSFamily retval;
    
    if (osName != null && osName.length() > 0)
    {
      if (osName.startsWith("Windows"))
        retval = OSFamily.WINDOWS;
      else if (osName.startsWith("Mac"))
        retval = OSFamily.MAC;
      else if (osName.startsWith("Linux"))
        retval = OSFamily.LINUX;
      else
        retval = OSFamily.UNKNOWN;
    } else
      retval = OSFamily.UNKNOWN;
    return retval;
  }
  
  /**
   * Return an OS Family from parsing a GNU autoconf triple.
   * 
   * For example "x86_64-w64-mingw32" will return {@link JNIEnv.OSFamily#WINDOWS}
   * and "ppc-apple-darwin" will return {@link JNIEnv.OSFamily#MAC}.
   * 
   * @param gnuString the GNU string
   * @return the OSFamily
   */
  public static OSFamily getOSFamilyFromGNUString(String gnuString)
  {
    final String nativeOs = (gnuString != null ? gnuString.toLowerCase() : "");
    final OSFamily retval;
    if (nativeOs.startsWith("mingw") || nativeOs.startsWith("cygwin"))
      retval = OSFamily.WINDOWS;
    else if (nativeOs.startsWith("darwin"))
      retval = OSFamily.MAC;
    else if (nativeOs.startsWith("linux"))
      retval = OSFamily.LINUX;
    else
      retval = OSFamily.UNKNOWN;
    return retval;
  }

  /**
   * @return the {@link JNIEnv.CPUArch} of this Java Virtual Machine.
   */
  public CPUArch getCPUArch()
  {
    return mCPUArch;
  }

  /**
   * @return the {@link JNIEnv.OSFamily} of this Java Virtual Machine.
   */
  public OSFamily getOSFamily()
  {
    return mOSFamily;
  }
}
