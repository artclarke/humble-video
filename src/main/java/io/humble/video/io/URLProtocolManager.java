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

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


import io.humble.video.io.FfmpegIO;
import io.humble.video.io.FileProtocolHandlerFactory;
import io.humble.video.io.IURLProtocolHandler;
import io.humble.video.io.IURLProtocolHandlerFactory;
import io.humble.video.io.NullProtocolHandlerFactory;
import io.humble.video.io.URLProtocolManager;

/**
 * This class manages URL protocols that we have handlers for.  It is used
 * (and called from) the Native code that integrates with FFMPEG.
 * 
 * To register a handler you would use the singleton manager as follows:
 * <code>
 * <pre>
 *  URLProtocolManager mgr = URLProtocolManager.getManager();
 *  mgr.registerFactory("myprotocol", myProtocolHandlerFactory);
 * </pre>
 * </code>
 */
public class URLProtocolManager
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  private final ConcurrentMap<String, IURLProtocolHandlerFactory> mProtocols;

  public static final String DEFAULT_PROTOCOL = "humblefile";
  public static final String NULL_PROTOCOL = "humblenull";
  
  private static final URLProtocolManager sManager = new URLProtocolManager();

  /**
   * Factory method to get the global protocol manager.
   */
  public static URLProtocolManager getManager()
  {
    return sManager;
  }

  /**
   * Method to be called to initialize this library.
   */
  public static void init()
  {
    // just calling this method causes sManager to be initialized
    // in the static constructor, which is the only initialization
    // we currently need.
  }

  // Don't let people create one of these; instead they must
  // use the Singleton factory method.
  private URLProtocolManager()
  {
    mProtocols = new ConcurrentHashMap<String, IURLProtocolHandlerFactory>();
    // Always register the DEFAULT
    registerFactory(DEFAULT_PROTOCOL, new FileProtocolHandlerFactory());
    // And the NULL protocols
    registerFactory(NULL_PROTOCOL, new NullProtocolHandlerFactory());        
  }

  /**
   * Register a new factory for IURLProtocolHandlers for a given protocol.
   * <p>
   * FFMPEG is very picky; protocols must be only alpha characters (no numbers).
   * </p>
   * @param protocol The protocol, without colon, this factory is to be used for.
   * @param factory The factory for the manager to use whenever a handler is requested for a registered protocol;
   *   null means disable handling for this factory.
   * @return The factory that was previously used for that protocol (or null if none).
   */
  public IURLProtocolHandlerFactory registerFactory(String protocol,
      IURLProtocolHandlerFactory factory)
  {
    if (protocol == null)
      throw new IllegalArgumentException("protocol required");
    
    IURLProtocolHandlerFactory oldFactory;
    if (factory == null)
      oldFactory = mProtocols.remove(protocol);
    else
      oldFactory = mProtocols.put(protocol, factory);
    
    log.trace("Registering factory for URLProtocol: {}", protocol);

    if (oldFactory == null)
    {
      // we previously didn't have something registered for this factory
      // so tell FFMPEG we're now the protocol manager for this protocol.
      log.trace("Letting FFMPEG know about an additional protocol: {}",
          protocol);
      FfmpegIO.registerProtocolHandler(protocol, this);
    }
    return oldFactory;
  }

  /*
   * Get a IURLProtocolHandler for this url.
   * <p>
   * IMPORTANT: This function is called from native code, and so the name and signature cannot
   * change without changing the native code.
   * </p><p>
   * This function is eventually invoked whenever someone tries to call url_open("yourprotocol:...", flags)
   * from FFMPEG native code.  It returns a protocol handler which will then have open(...) called on it.
   * </p>
   * @param url The URL we want to handle.
   * @param flags   Any flags that the url_open() function will want to pass.
   * 
   * @returns A protocol handler to use.
   */
  public IURLProtocolHandler getHandler(String url, int flags)
  {
    IURLProtocolHandler result = null;

    log.trace("looking for protocol handler for: {}", url);
    if (url == null || url.length() == 0)
      throw new IllegalArgumentException("expected valid URL");
    int colonIndex = url.indexOf(":");
    String protocol = null;
    if (colonIndex > 0)
    {
      protocol = url.substring(0, colonIndex);
    }
    else
    {
      protocol = DEFAULT_PROTOCOL;
    }

    IURLProtocolHandlerFactory factory = mProtocols.get(protocol);
    if (factory != null)
    {
      result = factory.getHandler(protocol, url, flags);
    }
    else
    {
      log.error("asked to get handler for unsupported protocol: {}",
          protocol);
    }

    return result;
  }
  
  /**
   * Get the resource portion of a url.  For example for the URL
   * <pre>
   * http://www.humble.io/video
   * </pre>
   * The protocol string is <code>http</code> and the resource
   * string is <code>www.humble.io/video</code>
   * 
   * @param url The url to parse
   * @return The resource
   */
  
  public static String getResourceFromURL(String url)
  {
    String retval = url;
    if (url != null && url.length() > 0)
    {
      int colonIndex;
      colonIndex = url.indexOf("://");
      if (colonIndex > 0)
        retval = url.substring(colonIndex+3);
      else {
        colonIndex = url.indexOf(":");
        if (colonIndex > 0)
        {
          // remove the URL prefix
          retval = url.substring(colonIndex + 1);
        }
      }
    }
    return retval;
  }

  /**
   * Get the protocol portion of a url.  For example for the URL
   * <pre>
   * http://www.humble.io/video
   * </pre>
   * The protocol string is <code>http</code> and the resource
   * string is <code>//www.humble.io/video</code>
   * 
   * @param url The url to parse
   * @return The protocol
   */
  
  public static String getProtocolFromURL(String url)
  {
    String retval = null;
    if (url != null && url.length() > 0)
    {
      int colonIndex = url.indexOf(":");
      if (colonIndex > 0)
      {
        // remove the URL suffix
        retval = url.substring(0, colonIndex);
      }
    }
    return retval;
  }

}
