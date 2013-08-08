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
package io.humble.video.customio;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Implementation of URLProtocolHandler that can read from {@link InputStream}
 * objects or write to {@link OutputStream} objects.
 * 
 * <p>
 * 
 * The {@link IURLProtocolHandler#URL_RDWR} mode is not supported, and
 * {@link #isStreamed(String, int)} will always return true.
 * 
 * </p>
 * 
 * @author aclarke
 * 
 */

public class InputOutputStreamHandler implements IURLProtocolHandler
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());

  /**
   * Whether or not this class will default calling
   * {@link Closeable#close()} on a stream when
   * {@link #close()} is called on this method.  Can be
   * overriden in constructors.
   */
  public final static boolean CLOSE_STREAM_ON_CLOSE=true;
  
  private final InputStream mInputStream;
  private final OutputStream mOutputStream;
  private final boolean mCloseStreamOnClose;
  private Closeable mOpenStream = null;
  
  /**
   * Create a handler that can only be opened for reading.
   * @param stream an input stream
   * 
   * @throws IllegalArgumentException if stream is null
   */
  public InputOutputStreamHandler(InputStream stream)
  {
    this(stream, null, CLOSE_STREAM_ON_CLOSE);
  }
  
  /**
   * Create a handler that can only be opened for writing.
   * @param stream an output stream
   * @throws IllegalArgumentException if stream is null
   */
  public InputOutputStreamHandler(OutputStream stream)
  {
    this(null, stream, CLOSE_STREAM_ON_CLOSE);
  }
  
  /**
   * Creates a new handler.  If you pass in non null arguments
   * for both in and out, the handler may be opened in either
   * {@link IURLProtocolHandler#URL_RDONLY_MODE} or
   * {@link IURLProtocolHandler#URL_WRONLY_MODE}, but not at
   * the same time.
   * <p>
   * {@link IURLProtocolHandler#URL_RDWR} mode is not supported
   * by this handler.
   * </p>
   * @param in The stream to read from if asked to.
   * @param out The stream to write from if asked to.
   * @param closeStreamOnClose Whether or not to automatically
   *   call {@link Closeable#close()} on the given stream
   *   when {@link #close()} is called.
   * @throws IllegalArgumentException if both in and out are null
   */
  public InputOutputStreamHandler(InputStream in, OutputStream out,
      boolean closeStreamOnClose)
  {
    if (in == null && out == null)
      throw new IllegalArgumentException("must pass one non null stream");
    mInputStream = in;
    mOutputStream = out;
    mCloseStreamOnClose = closeStreamOnClose;
  }


  /**
   * {@inheritDoc}
   */

  public int close()
  {
    int retval = 0;
    try
    {
      if (mOpenStream != null && mCloseStreamOnClose)
      {
        mOpenStream.close();
      }
    }
    catch (IOException e)
    {
      log.error("could not close stream {}: {}", mOpenStream, e);
      retval = -1;
    }
    mOpenStream = null;
    return retval;
  }

  /**
   * {@inheritDoc}
   */

  public int open(String url, int flags)
  {
    if (mOpenStream != null)
    {
      log.debug("attempting to open already open handler: {}",
          mOpenStream);
      return -1;
    }
    switch (flags)
    {
      case URL_RDWR:
        log.debug("do not support read/write mode for Java IO Handlers");
        return -1;
      case URL_WRONLY_MODE:
        mOpenStream = mOutputStream;
        if (mOpenStream == null)
        {
          log.error("No OutputStream specified for writing: {}",url);
          return -1;
        }
        break;
      case URL_RDONLY_MODE:
        mOpenStream = mInputStream;
        if (mOpenStream == null)
        {
          log.error("No InputStream specified for reading: {}", url);
          return -1;
        }
        break;
      default:
        log.error("Invalid flag passed to open: {}", url);
        return -1;
    }

    return 0;
  }

  /**
   * {@inheritDoc}
   */

  public int read(byte[] buf, int size)
  {
    int ret = -1;
    if (mOpenStream == null || !(mOpenStream instanceof InputStream))
      return -1;

    try
    {
      InputStream stream = (InputStream) mOpenStream;
      ret = stream.read(buf, 0, size);
      return ret;
    }
    catch (IOException e)
    {
      log.error("Got IO exception reading from stream: {}; {}",
          mOpenStream, e);
      return -1;
    }
  }

  /**
   * {@inheritDoc}
   * 
   * This method is not supported on this class and always return -1;
   */

  public long seek(long offset, int whence)
  {
    return -1;
  }

  /**
   * {@inheritDoc}
   */

  public int write(byte[] buf, int size)
  {
    if (mOpenStream == null ||
        !(mOpenStream instanceof OutputStream))
      return -1;

    try
    {
      OutputStream stream = (OutputStream) mOpenStream;
      stream.write(buf, 0, size);
      return size;
    }
    catch (IOException e)
    {
      log.error("Got error writing to file: {}; {}", mOpenStream, e);
      return -1;
    }
  }

  /**
   * {@inheritDoc}
   * Always true for this class. 
   */

  public boolean isStreamed(String url, int flags)
  {
    return true;
  }
  /**
   * Returns the stream we'd input from if asked.
   * @return the inputStream
   */
  public InputStream getInputStream()
  {
    return mInputStream;
  }
  /**
   * Returns the stream we'd output to if asked.
   * @return the outputStream
   */
  public OutputStream getOutputStream()
  {
    return mOutputStream;
  }
  /**
   * Returns the stream currently {@link #open(String, int)}.
   * @return the openStream
   */
  public Closeable getOpenStream()
  {
    return mOpenStream;
  }

  /**
   * Will this handler call {@link Closeable#close()} automatically
   * when its {@link #close()} method is called by Humble Video?
   * 
   * @return the closeStreamOnClose setting
   */
  public boolean isCloseStreamOnClose()
  {
    return mCloseStreamOnClose;
  }

}
