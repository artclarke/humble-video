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

import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.jar.Manifest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JNILibrary implements Comparable<JNILibrary> {
  private static final Logger log = LoggerFactory.getLogger(JNILibrary.class);

  private static final String HUMBLE_TEMP_EXTENSION = ".humble";

  private static final Map<String, List<JNIManifest>> mManifestLists = new HashMap<String, List<JNIManifest>>();
  private static final Map<String, JNILibrary> mAttemptedLibraries = new HashMap<String, JNILibrary>();

  final static private Object mLock = new Object();

  private static List<JNIManifest> getNativeManifests(String appName) {
    final List<JNIManifest> cached = mManifestLists.get(appName);
    if (cached != null)
      return cached;

    // well, it's not cached. let's do this the hard way.
    final List<JNIManifest> retval = new LinkedList<JNIManifest>();
    final ClassLoader loader = JNILibrary.class.getClassLoader();

    try {
      final Enumeration<URL> manifests = loader
          .getResources("META-INF/MANIFEST.MF");
      while (manifests.hasMoreElements()) {
        final URL url = manifests.nextElement();
        log.trace("Examining manifest: {}", url);
        final InputStream stream = url.openStream();
        if (stream != null) {
          final Manifest jarManifest = new Manifest(stream);
          if (jarManifest != null) {
            final JNIManifest manifest = JNIManifest.create(url, appName,
                jarManifest);
            if (manifest != null) {
              log.trace("found manifest: {}; url: {}", manifest, url);
              retval.add(manifest);
            }
          }
        }
      }
    } catch (IOException e) {
      log.debug("could not open manifest: {}", e);
    }

    // add to cache
    mManifestLists.put(appName, retval);
    return retval;
  }

  private final String mName;
  private final Long mVersion;

  private boolean mLoadAttempted;
  private boolean mLoadSuccessful;

  // a static initializer
  static {
    deleteTemporaryFiles();
  }

  public JNILibrary(String name, Long version) {
    if (name == null || name.length() <= 0)
      throw new IllegalArgumentException("need a valid name");
    mName = name;
    mVersion = version;
    mLoadAttempted = false;
    mLoadSuccessful = false;
  }

  public String getName() {
    return mName;
  }

  public Long getVersion() {
    return mVersion;
  }

  public boolean isLoadAttempted() {
    return mLoadAttempted;
  }

  public boolean isLoadSuccessful() {
    return mLoadSuccessful;
  }

  @Override
  public String toString() {
    return super.toString() + "[ name=" + mName + "; version=" + mVersion
        + "; ]";
  }

  /**
   * Load the given library into the given application.
   * 
   * This method first searches in the classpath for native libraries that are
   * bundled in there, and only if no matches are found, will it search the
   * run-time paths of each OS.
   * 
   * @param appname
   *          the name of the application. This should match what shows up in
   *          jar manifests or native property files.
   * @param library
   *          the library object
   * @throws UnsatisfiedLinkError
   *           if library cannot be loaded.
   */
  @SuppressWarnings("deprecation")
  public static void load(String appname, JNILibrary library) {
    // we force ALL work on all libraries to be synchronized
    synchronized (mLock) {
      deleteTemporaryFiles();
      try {
        library.load(appname);
      } catch (UnsatisfiedLinkError e) {
        // failed; faill back to old way
        JNILibraryLoader.loadLibrary(library.getName(), library.getVersion());
      }
    }
  }

  private void load(String appName) throws UnsatisfiedLinkError,
      SecurityException {
    if (mLoadAttempted) {
      if (mLoadSuccessful)
        return;
      else
        throw new UnsatisfiedLinkError(
            "already attempted and failed to load library: " + getName());
    }
    mLoadAttempted = true;
    // finally attempt to load ourselves
    loadFromClasspath(appName);
    mLoadSuccessful = false;
  }

  private void loadFromClasspath(String appName) {
    final JNILibrary priorAttempt = mAttemptedLibraries.get(getName());
    if (priorAttempt != null) {
      if (priorAttempt.mLoadSuccessful)
        return;
      else
        throw new UnsatisfiedLinkError(
            "previously attempted to load library and it failed: "
                + priorAttempt.getName());
    }

    // from the manifests build a list of candidate library names to try
    final List<String> libraryURLs = generateCandidateLibraryURLs(appName,
        getName());

    // finally go through each one until we get a load
    for (String url : libraryURLs) {
      if (unpackLibrary(url)) {
        return;
      }
    }
    // if we get all the way hwere, we did NOT succeed
    throw new UnsatisfiedLinkError("could not load library: " + getName());

  }

  private void doJNILoad(String url) {
    try {
      log.trace("Attempt: library load of library: {}; url: {}", new Object[] {
          getName(), url });

      System.load(url);
      log.trace("Success: library load of library: {}; url: {}", new Object[] {
          getName(), url });

    } catch (UnsatisfiedLinkError e) {
      log.debug("Failure: library load of library: {}; url: {}; error: {}",
          new Object[] { getName(), url, e });
      throw e;
    } catch (SecurityException e) {
      log.warn("Failure: library load of library: {}; url: {}; error: {}",
          new Object[] { getName(), url, e });
      throw e;
    }
  }

  /** Looks for a URL in a classpath, and if found, unpacks it */
  private boolean unpackLibrary(String path) {
    boolean retval = false;

    try {
      final Enumeration<URL> c = JNILibrary.class.getClassLoader()
          .getResources(path);

      while (c.hasMoreElements()) {
        final URL url = c.nextElement();
        log.trace("path: {}; url: {}", path, url);
        if (url == null)
          return false;

        boolean unpacked = false;
        File lib;
        if (url.getProtocol().toLowerCase().equals("file")) {
          // it SHOULD already exist on the disk. let's look for it.
          try {
            lib = new File(new URI(url.toString()));
          } catch (URISyntaxException e) {
            lib = new File(url.getPath());
          }
          if (!lib.exists()) {
            log.error("Unpacked library not unpacked correctedly;  url: {}",
                url);
            continue;
          }
        } else if (url.getProtocol().toLowerCase().equals("jar")){
          // sucktastic -- we cannot in a JVM load a shared library
          // directly from a JAR, so we need to unpack to a temp
          // directory and load from there.
          InputStream stream = url.openStream();
          if (stream == null) {
            log.error("could not get stream for resource: {}", url.getPath());
            continue;
          }
          FileOutputStream out = null;
          try {
            File dir = getTmpDir();
            // did you know windows REQUIRES .dll. Sigh.
            lib = File
                .createTempFile(
                    "humble",
                    JNIEnv.getEnv().getOSFamily() == JNIEnv.OSFamily.WINDOWS ? ".dll"
                        : null, dir);
            lib.deleteOnExit();
            out = new FileOutputStream(lib);
            int bytesRead = 0;
            final byte[] buffer = new byte[2048];
            while ((bytesRead = stream.read(buffer, 0, buffer.length)) > 0) {
              out.write(buffer, 0, bytesRead);
            }
            unpacked = true;
          } catch (IOException e) {
            log.error("could not create temp file: {}", e);
            continue;
          } finally {
            try {
              stream.close();
            } catch (IOException e) {
            }
            if (out != null)
              try {
                out.close();
              } catch (IOException e) {
              }
          }
          try {
            doJNILoad(lib.getAbsolutePath());
            retval = true;
            break;
          } catch (UnsatisfiedLinkError e) {
            // expected in some cases, try the next case.
          } finally {
            if (unpacked) {
              // Well let's try to clean up after ourselves since
              // we had ot unpack.
              deleteUnpackedFile(lib.getAbsolutePath());
            }
          }
        }
      }
    } catch (IOException e1) {
      retval = false;
    }
    return retval;
  }

  private void deleteUnpackedFile(String absolutePath) {
    final File file = new File(absolutePath);
    if (file.delete())
      return;
    // sigh -- we could not delete it. so we put a marker
    // file along side and delete it the next time the library starts
    // up.
    final String markerName = file.getName() + HUMBLE_TEMP_EXTENSION;
    try {
      File marker = new File(file.getParentFile(), markerName);
      marker.createNewFile();
    } catch (IOException e) {
      log.error("could not create marker file: {}; error: {}", markerName, e);
      // and swallow it.
    }
  }

  private static File getTmpDir() {
    File tmpdir = new File(System.getProperty("java.io.tmpdir"));
    File humbledir = new File(tmpdir, "humble");
    humbledir.mkdirs();
    return humbledir.exists() ? humbledir : tmpdir;
  }

  /**
   * Finds all ".humble" temp files in the temp directory and nukes them.
   */
  private static void deleteTemporaryFiles() {
    final File dir = getTmpDir();
    final FilenameFilter filter = new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.endsWith(HUMBLE_TEMP_EXTENSION);
      }
    };
    final File markers[] = dir.listFiles(filter);
    for (File marker : markers) {
      final String markerName = marker.getName();
      final String libName = markerName.substring(0, markerName.length()
          - HUMBLE_TEMP_EXTENSION.length());
      final File lib = new File(marker.getParentFile(), libName);
      if (!lib.exists() || lib.delete())
        marker.delete();
    }
  }

  private List<String> generateCandidateLibraryURLs(String appName,
      String libname) {
    final List<String> retval = new LinkedList<String>();
    final List<JNIManifest> manifests = getNativeManifests(appName);

    // for each manifest, generate URLs
    for (final JNIManifest manifest : manifests) {
      generateLibnames(retval, manifest.getPath(), libname);
    }
    // and finally we also test the top of the classpath in the event
    // that this is an applet or Web-Start app.
    generateLibnames(retval, "/", libname);

    return retval;
  }

  private static JNIEnv.OSFamily mOSFamily = JNIEnv.getEnv().getOSFamily();

  // These methods are at package visibility to allow testing only
  // They are not meant for use
  void setOSFamily(JNIEnv.OSFamily os) {
    mOSFamily = os;
  }

  JNIEnv.OSFamily getOSFamily() {
    return mOSFamily;
  }

  private void generateLibnames(List<String> list, String path, String libname) {
    final String[] prefixes;
    final String[] suffixes;
    switch (getOSFamily()) {
    case UNKNOWN:
    case LINUX:
      prefixes = new String[] { "lib", "" };
      suffixes = new String[] { ".so" };
      break;
    case WINDOWS:
      prefixes = new String[] { "lib", "", "cyg" };
      suffixes = new String[] {
      "-"+Ferry.getMajorVersion()+".dll", ".dll"
      };
      break;
    case MAC:
      prefixes = new String[] { "lib", "" };
      suffixes = new String[] { ".dylib" };
      break;
    default:
      // really no cases should get here
      prefixes = null;
      suffixes = null;
      break;
    }
    // can assume URL separators
    final String dirSeparator = "/";
    if (path.length() > 0 && !path.endsWith(dirSeparator))
      path = path + dirSeparator;
    for (String suffix : suffixes)
      for (String prefix : prefixes)
        list.add(path + prefix + libname + suffix);
  }

  public int compareTo(JNILibrary o) {
    if (o == null)
      return -1;

    int retval = mName.compareTo(o.mName);
    if (retval == 0) {
      if (mVersion == null) {
        if (o.mVersion != null) {
          retval = 1;
        }
      } else {
        if (o.mVersion == null)
          retval = -1;
        else
          retval = mVersion.compareTo(o.mVersion);
      }
    }
    return retval;
  }
}
