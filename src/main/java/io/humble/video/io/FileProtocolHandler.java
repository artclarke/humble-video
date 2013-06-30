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

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.video.io.IURLProtocolHandler;

/**
 * Implementation of URLProtocolHandler that can read and write files.
 * 
 * This just duplicates all the functionality in the default "file:" protocol
 * that FFMPEG implemements, but demonstrates how you can have FFMPEG
 * call back into Java.
 * 
 * @author aclarke
 *
 */
public class FileProtocolHandler implements IURLProtocolHandler
{
  File file = null;
  RandomAccessFile stream = null;

  private final Logger log = LoggerFactory.getLogger(this.getClass());

  public FileProtocolHandler()
  {
    log.debug("Initializing file protocol handler without file");
    this.file = null;
  }

  public FileProtocolHandler(File file)
  {
    log.debug("Initializing file protocol handler: {}", file);
    this.file = file;
  }

  public FileProtocolHandler(String filename)
  {
    log.debug("Initializing file protocol handler: {}", filename);
    filename = getFilename(filename);
    if (filename != null)
    {
      this.file = new File(filename);
    }
    else
    {
      this.file = null;
    }
  }

  public int close()
  {
    log.debug("Closing file: {}", file);
    try
    {
      stream.close();
    }
    catch (IOException e)
    {
      log.error("Error closing file: {}", file);
      e.printStackTrace();
      return -1;
    }

    log.debug("Succesfully closed file: {}", file);
    return 0;
  }

  public int open(String url, int flags)
  {
    int retval = -1;
    log.debug("attempting to open {} with flags {}", url == null ? file
        : url, flags);
    if (stream != null)
      this.close();

    if (this.file == null)
    {
      url = getFilename(url);
      if (url != null)
      {
        file = new File(url);
      }
    }
    log.debug("Opening file: {}", file);

    final String mode;
    switch (flags)
    {
    case URL_RDWR:
      mode = "rw";
      break;
    case URL_WRONLY_MODE:
      // RandomAccessFile doesn't support write-only
      mode = "rw";
      break;
    case URL_RDONLY_MODE:
      mode = "r";
      break;
    default:
      log.error("Invalid flag passed to open: {}", file);
      return retval;
    }

    log.debug("read mode \"{}\" for file: {}", mode, file);

    try
    {
      stream = new RandomAccessFile(file, mode);
      retval = 0;
    }
    catch (Exception e)
    {
      log.error("Could not find file: {}; ex: {}", file, e);
      return retval;
    }
    log.debug("Opened file: {}", file);
    return retval;
  }

  public int read(byte[] buf, int size)
  {
    //log.debug("Attempting to read {} bytes from: {}", size, file);
    try
    {
      int ret = -1;
      ret = stream.read(buf, 0, size);
      //log.debug("Got result for read: {}", ret);
      return ret;
    }
    catch (IOException e)
    {
      log.error("Got IO exception reading from file: {}", file);
      e.printStackTrace();
      return -1;
    }
  }

  public long seek(long offset, int whence)
  {
    try
    {
      final long seek;
      if (whence == SEEK_SET)
        seek = offset;
      else if (whence == SEEK_CUR)
        seek = stream.getFilePointer() + offset;
      else if (whence == SEEK_END)
        seek = stream.length() + offset;
      else if (whence == SEEK_SIZE)
        // odd feature of the protocol handler; this request
        // just returns the file size without actually seeking
        return (int) stream.length();
      else
      {
        log.error("invalid seek value \"{}\" for file: {}", whence, file);
        return -1;
      }

      stream.seek(seek);
      log.debug("seeking to \"{}\" in: {}", seek, file);
      return seek;
    }
    catch (IOException e)
    {
      log.error("got io exception \"{}\" while seeking in: {}", e
          .getMessage(), file);
      e.printStackTrace();
      return -1;
    }
  }

  public int write(byte[] buf, int size)
  {
    //log.debug("writing {} bytes to: {}", size, file);
    try
    {
      stream.write(buf, 0, size);
      return size;
    }
    catch (IOException e)
    {
      log.error("Got error writing to file: {}", file);
      e.printStackTrace();
      return -1;
    }
  }

  private String getFilename(String url)
  {
    String retval = url;
    if (url != null && url.length() > 0)
    {
      int colonIndex = url.indexOf(":");
      if (colonIndex > 0)
      {
        // remove the URL prefix
        retval = url.substring(colonIndex + 1);
      }
    }
    log.debug("url->filename: {}->{}", url, retval);
    return retval;
  }

  public boolean isStreamed(String url, int flags)
  {
    return false;
  }
}
