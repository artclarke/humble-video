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

import static org.junit.Assert.assertEquals;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import io.humble.video.io.FfmpegIO;
import io.humble.video.io.FfmpegIOHandle;
import io.humble.video.io.IURLProtocolHandler;
import io.humble.video.io.IURLProtocolHandlerFactory;
import io.humble.video.io.URLProtocolManager;

public class URLProtocolHandlerTest
{
  private static final URLProtocolManager mMgr = URLProtocolManager.getManager();
  private static final IURLProtocolHandler mHandler = new IURLProtocolHandler()
  {
    public int close()
    {
      throw new IllegalStateException("fails on purpose");
    }

    public boolean isStreamed(String aUrl, int aFlags)
    {
      return true;
    }

    public int open(String aUrl, int aFlags)
    {
      if (aUrl.equals("test:succeed"))
        return 0;
      else
        throw new IllegalStateException("fails on purpose");
    }

    public int read(byte[] aBuf, int aSize)
    {
      throw new IllegalStateException("fails on purpose");
    }

    public long seek(long aOffset, int aWhence)
    {
      throw new IllegalStateException("fails on purpose");
    }

    public int write(byte[] aBuf, int aSize)
    {
      throw new IllegalStateException("fails on purpose");
    }          
  };
  private static final IURLProtocolHandlerFactory mFactory = new IURLProtocolHandlerFactory()
  {
    public IURLProtocolHandler getHandler(String aProtocol, String aUrl,
        int aFlags)
    {
      return mHandler;
    }
  };
  private final byte[] mBuffer = new byte[10];
  private FfmpegIOHandle mHandle;
  
  @BeforeClass
  public static void beforeClass()
  {
    mMgr.registerFactory("test", mFactory);
  }
  
  @Before
  public void setUp()
  {
    mHandle = new FfmpegIOHandle();
  }

  
  @Test
  public void testOpenException()
  {
    assertEquals("should fail", -1, FfmpegIO.url_open(mHandle, "test:fail", IURLProtocolHandler.URL_RDWR));
  }
  
  @Test
  public void testReadException()
  {
    int retval = -1;
    retval = FfmpegIO.url_open(mHandle, "test:succeed", IURLProtocolHandler.URL_RDWR);
    assertEquals("should succeed", 0, retval);
    retval = FfmpegIO.url_read(mHandle, mBuffer, mBuffer.length);
    assertEquals("should fail", -1, retval);
  }
  
  @Test
  public void testWriteException()
  {
    int retval = -1;
    retval = FfmpegIO.url_open(mHandle, "test:succeed", IURLProtocolHandler.URL_RDWR);
    assertEquals("should succeed", 0, retval);
    retval = FfmpegIO.url_write(mHandle, mBuffer, mBuffer.length);
    assertEquals("should fail", -1, retval);
  }
  
  @Test
  public void testSeekException()
  {
    int retval = -1;
    retval = FfmpegIO.url_open(mHandle, "test:succeed", IURLProtocolHandler.URL_RDWR);
    assertEquals("should succeed", 0, retval);
    retval = (int)FfmpegIO.url_seek(mHandle, 0, IURLProtocolHandler.SEEK_END);
    assertEquals("should fail", -1, retval);
  }
  
  @Test
  public void testCloseException()
  {
    int retval = -1;
    retval = FfmpegIO.url_open(mHandle, "test:succeed", IURLProtocolHandler.URL_RDWR);
    assertEquals("should succeed", 0, retval);
    retval = FfmpegIO.url_close(mHandle);
    assertEquals("should fail", -1, retval);
  }

  @Test
  public void testURLProtocolManagerGetResource()
  {
    String url;
    String resource;
    String protocol;
    url = "http://www.humble.io/video";
    resource = URLProtocolManager.getResourceFromURL(url);
    protocol = URLProtocolManager.getProtocolFromURL(url);
    assertEquals("http", protocol);
    assertEquals("www.humble.io/video", resource);
    url = "file:/foo/bar";
    resource = URLProtocolManager.getResourceFromURL(url);
    protocol = URLProtocolManager.getProtocolFromURL(url);
    assertEquals("file", protocol);
    assertEquals("/foo/bar", resource);
  }
}
