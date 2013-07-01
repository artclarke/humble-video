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

import junit.framework.TestCase;

import io.humble.video.customio.FfmpegIO;
import io.humble.video.customio.FfmpegIOHandle;
import io.humble.video.customio.IURLProtocolHandler;
import io.humble.video.customio.NullProtocolHandler;
import io.humble.video.customio.URLProtocolManager;

import org.junit.*;

public class NullProtocolHandlerTest extends TestCase
{
  private IURLProtocolHandler writeHandle = null;
  private final String sampleFile = "target/test-classes/testfile.flv";
  private final String nullProtocolString = URLProtocolManager.NULL_PROTOCOL;
  
  @Before
  public void setUp()
  {
    writeHandle = null;
  }

  @After
  public void tearDown()
  {
    writeHandle = null;
  }

  @Test
  public void testCreate()
  {
    assertNotNull(new NullProtocolHandler());
  }
  
  @Test
  public void testCanOpenAnyFile()
  {
    // Test all ways to create a protocol writeHandle
    String badReadFile = "/tmp/garbage-file-k200cski2loiudjhsl2";
    int flags = IURLProtocolHandler.URL_RDONLY_MODE;

    int retval = 0;

    writeHandle = new NullProtocolHandler();
    assertTrue(writeHandle != null);

    retval = writeHandle.open(null, flags);
    assertTrue(retval == 0);

    retval = writeHandle.open(badReadFile, flags);
    assertTrue(retval == 0);
  }

  @Test
  public void testValidFFMPEGURLFileOpenForReading()
  {
    int flags = IURLProtocolHandler.URL_RDONLY_MODE;
    final String url = nullProtocolString + ":" + sampleFile;
    int retval = -1;

    // now, try opening using FFMPEG
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, url, flags);
    assertTrue(retval == 0);

    retval = FfmpegIO.url_close(handle);
    assertTrue(retval == 0);
  }

  @Test
  public void testInvalidFileOpenForWriting()
  {
    int flags = IURLProtocolHandler.URL_RDWR;
    final String url = nullProtocolString + ":foo";
    int retval = -1;

    // now, try opening using FFMPEG
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, url, flags);
    assertTrue(retval == 0);

    retval = FfmpegIO.url_close(handle);
    assertTrue(retval == 0);
  }

  @Test
  public void testFileRead()
  {
    // open our file
    writeHandle = new NullProtocolHandler();

    String filename = nullProtocolString +":" + sampleFile;
    int retval = 0;
    retval = writeHandle.open(filename,
        IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue("Could not open file: " + filename, retval >= 0);

    long bytesRead = 0;
    long totalBytes = writeHandle.seek(0, IURLProtocolHandler.SEEK_SIZE);

    byte[] buffer = new byte[1024];
    while ((retval = writeHandle.read(buffer, buffer.length)) > 0)
    {
      bytesRead += retval;
    }
    assertEquals(0, totalBytes);
    assertEquals(totalBytes, bytesRead);
    // and close
    retval = writeHandle.close();
    assertTrue(retval >= 0);
  }

  @Test
  public void testFileWrite()
  {
    String copyFile = nullProtocolString+":"+this.getClass().getName()+"_"+this.getName()+".flv";

    FfmpegIOHandle writeHandle = new FfmpegIOHandle();
    int retval = -1;
    
    // First, open the write writeHandle.
    retval = FfmpegIO.url_open(writeHandle, copyFile, IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue("could not open url: " + copyFile, retval >= 0);

    // Now, create and open a read writeHandle.
    // note that without a protocol string, should default to file:
    
    FfmpegIOHandle handle = new FfmpegIOHandle();
    retval = FfmpegIO.url_open(handle, URLProtocolManager.DEFAULT_PROTOCOL + ":" + sampleFile, IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue("could not open url: " + sampleFile, retval == 0);

    long bytesWritten = 0;
    long bytesRead = 0;
    byte[] buffer = new byte[1024];
    while ((retval = FfmpegIO.url_read(handle, buffer, buffer.length)) > 0)
    {
      bytesRead += retval;
      // Write the output.
      retval = FfmpegIO.url_write(writeHandle, buffer, retval);
      assertTrue(retval >= 0);
      bytesWritten += retval;
    }
    assertTrue(bytesRead > 0);
    assertEquals(bytesRead, bytesWritten);

    // close each file
    retval = FfmpegIO.url_close(handle);
    assertTrue(retval == 0);

    retval = FfmpegIO.url_close(writeHandle);
    assertTrue(retval >= 0);

  }
  

}
