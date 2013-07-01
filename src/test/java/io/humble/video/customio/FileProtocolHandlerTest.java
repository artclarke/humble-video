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
import io.humble.video.customio.FileProtocolHandler;
import io.humble.video.customio.FileProtocolHandlerFactory;
import io.humble.video.customio.IURLProtocolHandler;
import io.humble.video.customio.IURLProtocolHandlerFactory;
import io.humble.video.customio.URLProtocolManager;

import org.junit.*;

public class FileProtocolHandlerTest extends TestCase
{

  private IURLProtocolHandlerFactory factory = null;
  private IURLProtocolHandler handler = null;
  private final String sampleFile = "target/test-classes/testfile.flv";
  private final String fileProtocolString = "file";
  private final String ffmpegioProtocolString = URLProtocolManager.DEFAULT_PROTOCOL;

  @Before
  public void setUp()
  {
    factory = new FileProtocolHandlerFactory();
    handler = null;
  }

  @After
  public void tearDown()
  {
    handler = null;
  }

  @Test
  public void testInvalidFileOpenForReading()
  {
    // Test all ways to create a protocol handler
    String badReadFile = "/tmp/garbage-file-k200cski2loiudjhsl2";
    int flags = IURLProtocolHandler.URL_RDONLY_MODE;

    int retval = 0;

    handler = new FileProtocolHandler(badReadFile);
    assertTrue(handler != null);

    retval = handler.open(null, flags);
    assertTrue(retval == -1);

    retval = handler.open(badReadFile, flags);
    assertTrue(retval == -1);
  }

  @Test
  public void testValidFFMPEGURLFileOpenForReading()
  {
    int flags = IURLProtocolHandler.URL_RDONLY_MODE;
    final String url = ffmpegioProtocolString + ":" + sampleFile;
    int retval = -1;

    // Test all the different ways to open a valid file.
    handler = factory.getHandler(ffmpegioProtocolString, url, flags);
    assertTrue("could not find a handler using the factory",
        handler != null);

    // the factory should pass the URL to the handler
    retval = handler.open(null, flags);
    assertTrue(retval == 0);

    retval = handler.open(url, flags);
    assertTrue(retval == 0);

    // try creating the handler without the factory
    handler = new FileProtocolHandler();
    retval = handler.open(null, flags);
    // should fail because we have no way of knowing the file
    assertTrue(retval == -1);

    // and should succeed because now we know the url.
    retval = handler.open(url, flags);
    assertTrue(retval == 0);

    // try creating the handler by letting the constructor
    // know the file, but not the open
    handler = new FileProtocolHandler(url);
    retval = handler.open(null, flags);
    assertTrue(retval == 0);

    retval = handler.open(url, flags);
    assertTrue(retval == 0);

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
    String badWriteFile = "/notavalidrootdir/garbage-file-k200cski2loiudjhsl2";
    handler = new FileProtocolHandler(badWriteFile);

    int retval = 0;

    retval = handler.open(null, IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue(retval == -1);

    retval = handler
        .open(badWriteFile, IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue(retval == -1);
  }

  @Test
  public void testFileRead()
  {
    // open our file
    handler = new FileProtocolHandler();

    int retval = 0;
    retval = handler.open(fileProtocolString + ":" + sampleFile,
        IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue(retval >= 0);

    long bytesRead = 0;
    long totalBytes = handler.seek(0, IURLProtocolHandler.SEEK_SIZE);

    byte[] buffer = new byte[1024];
    while ((retval = handler.read(buffer, buffer.length)) > 0)
    {
      bytesRead += retval;
    }
    assertEquals(totalBytes, bytesRead);
    // and close
    retval = handler.close();
    assertTrue(retval >= 0);
  }

  @Test
  public void testFileWrite()
  {
    String copyFile = "file:"+this.getClass().getName()+"_"+this.getName()+".flv";

    handler = new FileProtocolHandler();
    int retval = 0;

    // First, open the write handler.
    retval = handler.open(copyFile, IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue(retval >= 0);

    // Now, create and open a read handler.
    // note that without a protocol string, should default to file:
    IURLProtocolHandler reader = new FileProtocolHandler(sampleFile);
    retval = reader.open(null, IURLProtocolHandler.URL_RDONLY_MODE);

    long bytesWritten = 0;
    long totalBytes = reader.seek(0, IURLProtocolHandler.SEEK_SIZE);

    byte[] buffer = new byte[1024];
    while ((retval = reader.read(buffer, buffer.length)) > 0)
    {
      // Write the output.
      retval = handler.write(buffer, retval);
      assertTrue(retval >= 0);
      bytesWritten += retval;
    }
    assertTrue(bytesWritten == totalBytes);

    // close each file
    retval = reader.close();
    assertTrue(retval >= 0);

    retval = handler.close();
    assertTrue(retval >= 0);

  }

  @Test
  public void testFFMPEGUrlRead()
  {
    try {
      testFFMPEGUrlReadTestFile(sampleFile);
      fail("should raise exception");
    } catch (Exception e) {
      
    }
  }

  @Test
  public void testFFMPEGIOUrlRead()
  {
    testFFMPEGUrlReadTestFile(ffmpegioProtocolString + ":" + sampleFile);
  }

  private void testFFMPEGUrlReadTestFile(String filename)
  {
    long retval = 0;
    // Call url_open wrapper
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, filename,
        IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue("url_open("+filename+") failed: " + retval, retval >= 0);

    // call url_read wrapper
    long bytesRead = 0;
    byte[] buffer = new byte[1024];
    while ((retval = FfmpegIO.url_read(handle, buffer, buffer.length)) > 0)
    {
      bytesRead += retval;
    }
    long fileSize = getFileSize(sampleFile);
    assertTrue("getFileSize failed", bytesRead == fileSize);

    // now test seeking
    long position = 0;
    retval = FfmpegIO.url_seek(handle, position,
        IURLProtocolHandler.SEEK_SIZE);
    // either SEEK_SIZE equals the size of the file, or it returns -1 because this
    // protocol doesn't support it.
    assertTrue("url_seek 1 failed: " + retval, bytesRead == retval
        || retval < 0);
    retval = FfmpegIO.url_seek(handle, position,
        IURLProtocolHandler.SEEK_END);
    assertTrue("url_seek 2 failed: " + retval, retval >= 0);
    // call url_close wrapper
    retval = FfmpegIO.url_close(handle);
    assertTrue("url_close failed: " + retval, retval >= 0);
  }

  @Test
  public void testDefaultUrlWrite()
  {
    try {
      testFFMPEGUrlWriteTestFile("file:"+this.getClass().getName()+"_"+this.getName()+".flv");
      fail("should raise exception");
    } catch (Exception e) {}
  }

  @Test
  public void testFfmpegIoUrlWrite()
  {
    testFFMPEGUrlWriteTestFile(ffmpegioProtocolString + ":"
        + this.getClass().getName()+"_"+this.getName() + ".flv");
  }

  private void testFFMPEGUrlWriteTestFile(String filename)
  {
    int retval = 0;
    // Call url_open wrapper
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, filename,
        IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue("url_open failed: " + retval, retval >= 0);

    // call url_read wrapper
    byte[] buffer = new byte[4];
    buffer[0] = 'F';
    buffer[1] = 'L';
    buffer[2] = 'V';
    buffer[3] = 0;

    retval = FfmpegIO.url_write(handle, buffer, buffer.length);
    assertTrue("url_write failed: " + retval, retval == buffer.length);

    // call url_close wrapper
    retval = FfmpegIO.url_close(handle);
    assertTrue("url_close failed: " + retval, retval >= 0);
  }

  private static long getFileSize(String filename)
  {
    IURLProtocolHandler handler = new FileProtocolHandler();

    int retval = 0;
    retval = handler.open(filename, IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue(retval >= 0);

    long totalBytes = handler.seek(0, IURLProtocolHandler.SEEK_SIZE);
    assertTrue(totalBytes >= 0);

    // and close
    retval = handler.close();
    assertTrue(retval >= 0);
    return totalBytes;
  }
  
}
