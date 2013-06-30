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
package io.humble.video.io;


import io.humble.video.io.FfmpegIOHandle;
import io.humble.video.io.IURLProtocolHandlerFactory;
import io.humble.video.io.URLProtocolManager;

/**
 * For Internal Use Only.
 * This is the global (static) object that implements the Humble Video IO library.
 * <p>
 * The following methods are not meant to be generally
 * called by code (although they can be).
 * </p></p>
 * You should never need to call this method.  Calling {@link URLProtocolManager#registerFactory(String, IURLProtocolHandlerFactory)}
 * should cause it to be instantiated.
 *</p></p>
 * They are generally 'thread-aware' but not 'thread-safe', meaning if you use a handle
 * on a thread, you are a responsible for making sure you don't
 * reuse it on other threads.
 * </p></p>
 * They forward into ffmpeg's URL read/write/seek capabilities
 * and allow our test scripts to make sure our URLProtocolHandlers
 * (and FFMPEG's native handlers) work as expected.
 * </p></p>
 * Lastly this class, unlike other classes in the library, does not use SWIG to generate
 * the Java objects, so you need to be careful not to change method names as the corresponding
 * native code relies on specific naming and parameters.
 * </p>
 * @author aclarke
 *
 */
public class FfmpegIO
{

  static
  {
    // this will load the one shared library
    io.humble.ferry.Ferry.init();
    FfmpegIO.init();
    // And force the URLProtocolManager global
    // object to be created.
    URLProtocolManager.init();
  }

  /** Force a load of all native libraries; not normally needed */
  public static void load() {}
  
  /**
   * Internal Only.  Do not use.
   */
  public native static void init();
  
  /**
   * This method is called by URLProtocolManager to register itself as a 
   * protocol manager for different FFMPEG protocols.
   * 
   * @param protocol The protocol
   * @param manager The manager for that protocol.
   */
  static synchronized void registerProtocolHandler(String protocol,
      URLProtocolManager manager)
  {
    native_registerProtocolHandler(protocol, manager);
  }

  public static int url_open(FfmpegIOHandle handle, String filename, int flags)
  {
    return native_url_open(handle, filename, flags);
  }

  public static int url_read(FfmpegIOHandle handle, byte[] buffer, int length)
  {
    return native_url_read(handle, buffer, length);
  }

  public static int url_close(FfmpegIOHandle handle)
  {
    return native_url_close(handle);
  }

  public static int url_write(FfmpegIOHandle handle, byte[] buffer, int length)
  {
    return native_url_write(handle, buffer, length);
  }

  public static long url_seek(FfmpegIOHandle handle, long position, int whence)
  {
    return native_url_seek(handle, position, whence);
  }

  private static native int native_registerProtocolHandler(
      String urlPrefix, URLProtocolManager proto);

  private static native int native_url_open(FfmpegIOHandle handle,
      String filename, int flags);

  private static native int native_url_read(FfmpegIOHandle handle,
      byte[] buffer, int length);

  private static native int native_url_write(FfmpegIOHandle handle,
      byte[] buffer, int length);

  private static native long native_url_seek(FfmpegIOHandle handle,
      long position, int whence);

  private static native int native_url_close(FfmpegIOHandle handle);

}
