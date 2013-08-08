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

import java.net.URL;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.jar.Attributes;
import java.util.jar.Manifest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class JNIManifest {
  private static Logger log = LoggerFactory.getLogger(JNIManifest.class);
  private final static String HUMBLE_APP="Humble-Native-App";
  private final static String HUMBLE_ROOT="Humble-Native-Root";
  private final static String HUMBLE_PATH="Humble-Native-Paths";
  private final static String HUMBLE_BUNDLES="Humble-Native-Bundles";
  
  enum CPUArch{
    X86,
    X86_64,
    PPC,
    PPC64,
    UNKNOWN,
  }
  enum OS {
    LINUX,
    MAC,
    WINDOZE,
    UNKNOWN,
  }
  private final String mName;
  private final String mPath;
  private final String mCPU;
  private final String mOS;
  private final String mVendor;
  
  /**
   * @return the name
   */
  public String getName()
  {
    return mName;
  }
  /**
   * @return the path
   */
  public String getPath()
  {
    return mPath;
  }
  /**
   * @return the cPU
   */
  public String getCPU()
  {
    return mCPU;
  }
  /**
   * @return the oS
   */
  public String getOS()
  {
    return mOS;
  }
  /**
   * @return the vendor
   */
  public String getVendor()
  {
    return mVendor;
  }
  
  @Override
  public String toString()
  {
    final StringBuilder builder = new StringBuilder();
    builder.append(super.toString());
    builder.append("[");
    builder.append("name=");
    builder.append(mName);
    builder.append("; ");

    builder.append("path=");
    builder.append(mPath);
    builder.append("; ");

    builder.append("cpu=");
    builder.append(mCPU);
    builder.append("; ");
    
    builder.append("os=");
    builder.append(mOS);
    builder.append("; ");
    
    builder.append("vendor=");
    builder.append(mVendor);
    builder.append("; ");
    
    builder.append("]");
    return builder.toString();
  }
  public static JNIManifest create(URL url, String appName, Properties props)
  {
    final String name = props.getProperty(HUMBLE_APP);
    final String root=props.getProperty(HUMBLE_ROOT);
    final String paths=props.getProperty(HUMBLE_PATH);
    final String bundles=props.getProperty(HUMBLE_BUNDLES);
    return create(url, appName, name, root, paths, bundles);
  }
  public static JNIManifest create(URL url, String appName, Manifest manifest)
  {
    final Attributes attr = manifest.getMainAttributes();
    final String name = attr.getValue(HUMBLE_APP);
    final String root=attr.getValue(HUMBLE_ROOT);
    final String paths=attr.getValue(HUMBLE_PATH);
    final String bundles=attr.getValue(HUMBLE_BUNDLES);
    return create(url, appName, name, root, paths, bundles);
  }

  public static JNIManifest create(
      URL url,
      String appName,
      String name, String root, String aPaths, String aBundles)
  {
    // check for name matching
    if (name == null || name.compareTo(name)!= 0)
    {
      log.trace("{}: no match.  \"{}\" vs \"{}\"", new Object[]{HUMBLE_APP, appName, name});
      return null;
    }
    if (root == null)
      return null;
    if (aBundles == null)
      return null;
    if (aPaths == null)
      return null;
    
    // check for equal specification of bundles and paths
    final StringTokenizer pathTokens = new StringTokenizer(aPaths, ";");
    final StringTokenizer bundleTokens = new StringTokenizer(aBundles, ";");
    final List<String> paths = new LinkedList<String>();
    final List<String> bundles = new LinkedList<String>();
    while(pathTokens.hasMoreTokens()) {
      final String path = pathTokens.nextToken();
      paths.add(path);
    }
    while(bundleTokens.hasMoreTokens()) {
      final String bundle = bundleTokens.nextToken();
      bundles.add(bundle);
    }
    if (paths.size() != bundles.size()) {
      log.debug("path and bundle strings have different number of entries; invalid manifest: {}", appName);
      return null;
    }
    
    Iterator<String> bundleIter = bundles.iterator();
    Iterator<String> pathIter = paths.iterator();
    while(bundleIter.hasNext()) {
      final String bundle = bundleIter.next();
      final String path = pathIter.next();
      // parse the bundle
      StringTokenizer bundleParser = new StringTokenizer(bundle, "-");
      if (!bundleParser.hasMoreTokens())
        continue;
      final String nativeCpu = bundleParser.nextToken();
      if (!bundleParser.hasMoreTokens())
        continue;
      final String nativeVendor = bundleParser.nextToken();
      if (!bundleParser.hasMoreTokens())
        continue;
      final String nativeOs = bundleParser.nextToken();
      
      // now we have our triplet
      if (validMatch(nativeCpu, nativeOs)) 
      {
        final StringBuilder nativePath = new StringBuilder();
        
        if (!root.equals(".") && !root.equals("")) {
          nativePath.append(root);
          if (!root.endsWith("/"))
            nativePath.append("/");
        }
        if (!path.equals(".") && !path.equals("")) {
          if (path.startsWith("/")) {
            final String subPath = path.substring(1);
            nativePath.append(subPath);
            if (!subPath.endsWith("/"))
              nativePath.append("/");
          } else {
            nativePath.append(path);
            if (!path.endsWith("/"))
              nativePath.append("/");
          }
        }
        
        final JNIManifest retval = new JNIManifest(appName, nativePath.toString(), nativeCpu, nativeOs, nativeVendor);
        log.trace("found matching manifest: {}", retval);
        return retval;
      }
    }
    return null;
  }
  
  private JNIManifest(
      String appName, String nativePath,
      String cpu, String os, String vendor) {
    mName = appName;
    mPath = nativePath;
    mCPU = cpu;
    mOS = os;
    mVendor = vendor;
  }
  private static boolean validMatch(
      String cpu, String os)
  {
    JNIEnv.OSFamily javaOs = JNIEnv.getEnv().getOSFamily();
    JNIEnv.OSFamily nativeOs = JNIEnv.getOSFamilyFromGNUString(os);
    
    if (nativeOs != JNIEnv.OSFamily.UNKNOWN &&
        javaOs == nativeOs)
      return validMatchCPU(cpu);
    else
      return false;
  }

  private static boolean validMatchCPU(String aNativeCpu)
  {
    final JNIEnv.CPUArch javaArch;
    final JNIEnv.CPUArch nativeArch;
    final String nativeCpu = aNativeCpu.toLowerCase();
    
    javaArch = JNIEnv.getEnv().getCPUArch();
    if (nativeCpu.startsWith("universal"))
      // a special case where the native library claims to not care about arch.
      // this only happens on Mac/Darwin with universal builds.
      nativeArch = javaArch;
    else
      nativeArch = JNIEnv.getCPUArchFromGNUString(nativeCpu);
  
    // then compare
    return (nativeArch != JNIEnv.CPUArch.UNKNOWN && nativeArch == javaArch);
  }
}
