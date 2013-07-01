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
import java.io.DataInput;
import java.io.DataInputStream;
import java.io.DataOutput;
import java.io.EOFException;
import java.io.IOException;
import java.io.RandomAccessFile;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Implementation of URLProtocolHandler that can read from
 * {@link DataInput}
 * objects or write to {@link DataOutput} objects.
 * 
 * <p>
 * 
 * The {@link IURLProtocolHandler#URL_RDWR} mode is not supported
 * unless both input and output objects are the same
 * {@link RandomAccessFile}
 * 
 * </p>
 * <p>
 * 
 * {@link IURLProtocolHandler#isStreamed(String, int)} will return
 * true, unless this class is reading from or writing to
 * a {@link RandomAccessFile};
 * 
 * </p>
 * 
 * @author aclarke
 * 
 */

public class DataInputOutputHandler implements IURLProtocolHandler
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());

  /**
   * Whether or not this class will default calling
   * {@link Closeable#close()} on a stream when
   * {@link #close()} is called on this method.  Can be
   * overriden in constructors.
   */
  public final static boolean CLOSE_STREAM_ON_CLOSE=true;
  
  private final DataInput mDataInput;
  private final DataOutput mDataOutput;
  private final boolean mCloseStreamOnClose;
  private Object mOpenStream = null;
  
  /**
   * Create a handler that can only be opened for reading.
   * @param in an input object.
   */
  public DataInputOutputHandler(DataInput in)
  {
    this(in, null, CLOSE_STREAM_ON_CLOSE);
  }
  
  /**
   * Create a handler that can only be opened for writing.
   * @param out an output object
   */
  public DataInputOutputHandler(DataOutput out)
  {
    this(null, out, CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Create a handler that can only be opened for reading,
   * writing and seeking.
   * @param file a file to use.
   */
  public DataInputOutputHandler(RandomAccessFile file)
  {
    this(file, file, CLOSE_STREAM_ON_CLOSE);
  }
  
  /**
   * Creates a new handler.  If you pass in non null arguments
   * for both in and out, the handler may be opened in either
   * {@link IURLProtocolHandler#URL_RDONLY_MODE} or
   * {@link IURLProtocolHandler#URL_WRONLY_MODE}, but not at
   * the same time.
   * <p>
   * {@link IURLProtocolHandler#URL_RDWR} mode is supported
   * by this handler if both in and out are instances of
   * {@link RandomAccessFile}.
   * </p>
   * @param in The object to read from if asked to.
   * @param out The object to write from if asked to.
   * @param closeStreamOnClose Whether or not to automatically
   *   call {@link Closeable#close()} on the given channel
   *   when {@link #close()} is called.
   */
  public DataInputOutputHandler(
      DataInput in, DataOutput out,
      boolean closeStreamOnClose)
  {
    if (in == null && out == null)
      throw new IllegalArgumentException("must pass one non null stream");
    mDataInput = in;
    mDataOutput = out;
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
      if (mOpenStream != null && mCloseStreamOnClose &&
          mOpenStream instanceof Closeable)
      {
        ((Closeable)mOpenStream).close();
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
        if (mDataInput != null && mDataOutput != null && 
            mDataInput == mDataOutput &&
            mDataInput instanceof RandomAccessFile) {
          mOpenStream = mDataInput;
        } else {
          log.debug("do not support read/write mode for Java IO Handlers");
          return -1;
        }
        break;
      case URL_WRONLY_MODE:
        mOpenStream = mDataOutput;
        if (mOpenStream == null)
        {
          log.error("No OutputStream specified for writing: {}",url);
          return -1;
        }
        break;
      case URL_RDONLY_MODE:
        mOpenStream = mDataInput;
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
    if (mOpenStream == null || !(mOpenStream instanceof DataInput))
      return -1;

    try
    {
      if (mOpenStream instanceof RandomAccessFile) {
        RandomAccessFile file = (RandomAccessFile) mOpenStream;
        return file.read(buf, 0, size);
      } else if (mOpenStream instanceof DataInputStream) {
        DataInputStream stream = (DataInputStream) mOpenStream;
        return stream.read(buf, 0, size);
      } else {
        DataInput input = (DataInput) mOpenStream;
        try {
          input.readFully(buf, 0, size);
          ret = size;
        } catch (EOFException e) {
          // man; we have no idea how many bytes were actually
          // read now... so we truncate the data
          // what a sucky interface!
          ret = -1;
        }
        return ret;
      }
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
    if (mOpenStream == null)
      return -1;
    if (!(mOpenStream instanceof RandomAccessFile))
      return -1;
    final RandomAccessFile file = (RandomAccessFile) mOpenStream;
    try
    {
      final long seek;
      if (whence == SEEK_SET)
        seek = offset;
      else if (whence == SEEK_CUR)
        seek = file.getFilePointer() + offset;
      else if (whence == SEEK_END)
        seek = file.length() + offset;
      else if (whence == SEEK_SIZE)
        // odd feature of the protocol handler; this request
        // just returns the file size without actually seeking
        return (int) file.length();
      else
      {
        log.error("invalid seek value \"{}\" for file: {}", whence, file);
        return -1;
      }

      file.seek(seek);
      return seek;
    }
    catch (IOException e)
    {
      log.debug("got io exception \"{}\" while seeking in: {}", e
          .getMessage(), file);
      return -1;
    }
  }

  /**
   * {@inheritDoc}
   */

  public int write(byte[] buf, int size)
  {
    if (mOpenStream == null ||
        !(mOpenStream instanceof DataOutput))
      return -1;

    try
    {
      DataOutput output = (DataOutput) mOpenStream;
      output.write(buf, 0, size);
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
   * Always true, unless we're inputting or outputting to a
   * {@link RandomAccessFile}. 
   */

  public boolean isStreamed(String url, int flags)
  {
    if (mDataInput != null && mDataInput instanceof RandomAccessFile)
      return false;
    if (mDataOutput != null && mDataOutput instanceof RandomAccessFile)
      return false;
    return true;
  }
  /**
   * Returns the {@link DataInput} we'd input from if asked.
   * @return the {@link DataInput}
   */
  public DataInput getDataInput()
  {
    return mDataInput;
  }
  /**
   * Returns the {@link DataOutput} we'd output to if asked.
   * @return the {@link DataOutput}
   */
  public DataOutput getDataOutput()
  {
    return mDataOutput;
  }
  /**
   * Returns the object currently {@link #open(String, int)}.
   * @return the open object
   */
  public Object getOpen()
  {
    return mOpenStream;
  }

  /**
   * Will this handler call {@link Closeable#close()} automatically
   * when its {@link #close()} method is called by Humble Video?
   * 
   * We will check if the object we're using supports {@link Closeable}.
   * 
   * @return the closeStreamOnClose setting
   */
  public boolean isCloseStreamOnClose()
  {
    return mCloseStreamOnClose;
  }

}
