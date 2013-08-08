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

import io.humble.video.customio.FileProtocolHandler;
import io.humble.video.customio.URLProtocolManager;

/**
 * Interface that Humble Video native code calls back to for each
 * URL.  It is assumed a new Protocol object is made
 * per URL being read or written to.
 * <p> 
 * You must implement this interface if you want to register a
 * new type of URLProtocolHandler with the {@link URLProtocolManager}.
 * </p>
 * <p>
 * If you throw an exception in your implementation of this handler during
 * a callback from within Humble Video, we will assume your method returned -1 while
 * still in native code.  Once the stack unwinds back into Java we will
 * re-raise your exception.
 * </p>
 * @see FileProtocolHandler
 * @see NullProtocolHandler
 * 
 * @author aclarke
 *
 */
public interface IURLProtocolHandler
{
  // These are the flags that can be passed to open
  // IMPORTANT: These values must match the corresponding
  // flags in the avio.h header file in libavformat
  
  /**
   * A flag for {@link #seek(long, int)}.  Denotes positions relative to start of file.
   */
  public static final int SEEK_SET = 0;
  /**
   * A flag for {@link #seek(long, int)}.  Denotes positions relative to where the current file pointer is.
   */
  public static final int SEEK_CUR = 1;
  /**
   * A flag for {@link #seek(long, int)}.  Denotes positions relative to the end of file.
   */
  public static final int SEEK_END = 2;
  /**
   * A flag for {@link #seek(long, int)}.
   * A special hack of FFMPEG, denotes you want to find the total size of the file.
   */
  public static final int SEEK_SIZE = 0x10000;
  /**
   * Open the file in Read Only mode.
   */
  public static final int URL_RDONLY_MODE = 0;
  
  /**
   * Open the file in Write Only mode.
   */
  public static final int URL_WRONLY_MODE = 1;
  
  /**
   * Implement the file in Read/Write mode.
   */
  public static final int URL_RDWR = 2;

  /**
   * This method gets called by FFMPEG when it opens a file.
   * 
   * @param url The URL to open
   * @param flags The flags (e.g. {@link #URL_RDONLY_MODE})
   * @return >= 0 for success; -1 for error.
   * 
   */
  public int open(String url, int flags);

  /**
   * This method gets called by FFMPEG when it tries to read data.
   * <p>
   * For non-blocking IO, return:
   * {@link io.humble.video.Error#typeToErrorNumber(io.humble.video.Error.Type)}
   * pass in {@link io.humble.video.Error.Type#ERROR_AGAIN} for the error type.  This
   * returns the platform specific number for EAGAIN on your platform
   * signaling that callers should try again later.
   * </p>
   * <p>
   * Alternately implementors may block until data is returning, but they should then
   * respect the {@link Thread#isInterrupted()} setting.
   * </p>
   * 
   * @param buf The buffer to write your data to.
   * @param size The number of bytes in buf data available for you to write the data that FFMPEG will read.
   * @return 0 for end of file, else number of bytes you wrote to the buffer, or -1 if error.
   */
  public int read(byte[] buf, int size);

  /**
   * This method gets called by FFMPEG when it tries to write data.
   * <p>
   * For non-blocking IO, return:
   * {@link io.humble.video.Error#typeToErrorNumber(io.humble.video.Error.Type)}
   * pass in {@link io.humble.video.Error.Type#ERROR_AGAIN} for the error type.  This
   * returns the platform specific number for EAGAIN on your platform
   * signaling that callers should try again later.
   * </p>
   * <p>
   * Alternately implementators may block until data is returning, but they should then
   * respect the {@link Thread#isInterrupted()} setting.
   * </p>
   * 
   * @param buf The data you should write.
   * @param size The number of bytes in buf.
   * @return 0 for end of file, else number of bytes you read from buf, or -1 if error.
   */
  public int write(byte[] buf, int size);

  /**
   * A request from FFMPEG to seek to a position in the stream.
   * 
   * @param offset The offset in bytes.
   * @param whence Where that offset is relative to.  Follow the C stdlib fseek() conventions EXCEPT
   *   {@link #SEEK_SIZE} should return back the size of the stream in bytes if known without adjusting
   *   the seek pointer.
   * @return -1 if not supported, else the position relative to whence
   */
  public long seek(long offset, int whence);

  /**
   * A request to close() from FFMPEG
   * 
   * @return -1 on error; else >= 0
   */
  public int close();

  /**
   * Special callback made by Humble Video in order to determine if your
   * stream supports streaming.
   *
   * <p>
   * 
   * If this method returns true, Humble Video will assume it cannot seek backwards
   * in this container.
   * </p>
   * <p>
   * 
   * This has one main practical consequence.  When writing it means certain container formats (e.g. the MOV
   * container) will not be usable as it requires seeking
   * back to the start of a file to write MOV required header information once
   * the entire file has been encoded.
   * 
   * </p>
   * <p>
   * 
   * But if your medium is streaming, you'll want to return true for this,
   * and then FFMPEG will not attempt to seek back in time.
   * 
   * </p>
   *  
   * @param url The URL that would be passed to {@link #open(String, int)}
   * @param flags The flags that would be passed to {@link #open(String, int)}
   * @return true if you can stream that URL; false if not.
   */
  public boolean isStreamed(String url, int flags);
}
