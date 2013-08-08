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

import io.humble.video.customio.IURLProtocolHandler;

/**
 * The NullProtocolHandler implements {@link IURLProtocolHandler}, but discards
 * any data written and always returns 0 for reading.
 * 
 * <p>
 * This can be useful if your operating system doesn't have the unix
 * equivalent of <code>/dev/null</code> or you want to cheaply
 * discard data.
 * </p>
 * 
 * @author aclarke
 *
 */
public class NullProtocolHandler implements IURLProtocolHandler
{

  // package level so other folks can't create it.
  NullProtocolHandler()
  {
  }
  
  public int close()
  {
    // Always succeed
    return 0;
  }

  public boolean isStreamed(String aUrl, int aFlags)
  {
    // We're not streamed because, well, we do nothing.
    return false;
  }

  public int open(String aUrl, int aFlags)
  {
    // Always succeed
    return 0;
  }

  public int read(byte[] aBuf, int aSize)
  {
    // always read zero bytes
    return 0;
  }

  public long seek(long aOffset, int aWhence)
  {
    // always seek to where we're asked to seek to
    return aOffset;
  }

  public int write(byte[] aBuf, int aSize)
  {
    // always write all bytes
    return aSize;
  }

}
