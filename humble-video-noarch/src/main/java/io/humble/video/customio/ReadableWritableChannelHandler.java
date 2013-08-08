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
import java.nio.ByteBuffer;
import java.nio.channels.Channel;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Implementation of URLProtocolHandler that can read from
 * {@link ReadableByteChannel}
 * objects or write to {@link WritableByteChannel} object.
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

public class ReadableWritableChannelHandler implements IURLProtocolHandler
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());

  /**
   * Whether or not this class will default calling
   * {@link Closeable#close()} on a stream when
   * {@link #close()} is called on this method.  Can be
   * overriden in constructors.
   */
  public final static boolean CLOSE_STREAM_ON_CLOSE=true;
  
  private final ReadableByteChannel mReadChannel;
  private final WritableByteChannel mWriteChannel;
  private final boolean mCloseStreamOnClose;
  private Channel mOpenStream = null;
  
  /**
   * Create a handler that can only be opened for reading.
   * @param channel an input channel.
   */
  public ReadableWritableChannelHandler(ReadableByteChannel channel)
  {
    this(channel, null, CLOSE_STREAM_ON_CLOSE);
  }
  
  /**
   * Create a handler that can only be opened for writing.
   * @param channel an output channel
   */
  public ReadableWritableChannelHandler(WritableByteChannel channel)
  {
    this(null, channel, CLOSE_STREAM_ON_CLOSE);
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
   * @param in The channel to read from if asked to.
   * @param out The channel to write from if asked to.
   * @param closeStreamOnClose Whether or not to automatically
   *   call {@link Closeable#close()} on the given channel
   *   when {@link #close()} is called.
   */
  public ReadableWritableChannelHandler(
      ReadableByteChannel in, WritableByteChannel out,
      boolean closeStreamOnClose)
  {
    if (in == null && out == null)
      throw new IllegalArgumentException("must pass one non null stream");
    mReadChannel = in;
    mWriteChannel = out;
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
        mOpenStream = mWriteChannel;
        if (mOpenStream == null)
        {
          log.error("No OutputStream specified for writing: {}",url);
          return -1;
        }
        break;
      case URL_RDONLY_MODE:
        mOpenStream = mReadChannel;
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
    if (mOpenStream == null || !(mOpenStream instanceof ReadableByteChannel))
      return -1;

    try
    {
      ReadableByteChannel channel = (ReadableByteChannel) mOpenStream;
      ByteBuffer buffer = ByteBuffer.allocate(size);
      ret = channel.read(buffer);
      if (ret > 0) {
        buffer.flip();
        buffer.get(buf, 0, ret);
      }
      return ret;
    }
    catch (IOException e)
    {
      log.error("Got IO exception reading from channel: {}; {}",
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
        !(mOpenStream instanceof WritableByteChannel))
      return -1;

    try
    {
      WritableByteChannel channel = (WritableByteChannel) mOpenStream;
      ByteBuffer buffer = ByteBuffer.allocate(size);
      buffer.put(buf, 0, size);
      buffer.flip();
      return channel.write(buffer);
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
   * Returns the channel we'd input from if asked.
   * @return the channel
   */
  public ReadableByteChannel getReadChannel()
  {
    return mReadChannel;
  }
  /**
   * Returns the channel we'd output to if asked.
   * @return the channel
   */
  public WritableByteChannel getWriteChannel()
  {
    return mWriteChannel;
  }
  /**
   * Returns the channel currently {@link #open(String, int)}.
   * @return the openStream
   */
  public Channel getOpenChannel()
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
