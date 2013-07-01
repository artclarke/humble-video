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

import io.humble.video.customio.DataInputOutputHandler;
import io.humble.video.customio.FfmpegIO;
import io.humble.video.customio.FfmpegIOHandle;
import io.humble.video.customio.FileProtocolHandler;
import io.humble.video.customio.HumbleIO;
import io.humble.video.customio.IURLProtocolHandler;
import io.humble.video.customio.InputOutputStreamHandler;
import io.humble.video.customio.ReadableWritableChannelHandler;
import io.humble.video.customio.URLProtocolManager;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

import junit.framework.TestCase;

import org.junit.*;

public class HumbleIOTest extends TestCase
{

  private HumbleIO mFactory = null;
  private IURLProtocolHandler mHandler = null;
  private final String mSampleFile = "target/test-classes/testfile.flv";
  private final String mProtocolString = HumbleIO.DEFAULT_PROTOCOL;

  @Before
  public void setUp()
  {
    mFactory = new HumbleIO();
    URLProtocolManager.getManager().registerFactory(mProtocolString, mFactory);
    mHandler = null;
  }

  @After
  public void tearDown()
  {
    mHandler = null;
  }

  @Test
  public void testValidFFMPEGURLFileOpenForReading()
      throws FileNotFoundException
  {
    int flags = IURLProtocolHandler.URL_RDONLY_MODE;

    FileInputStream stream = new FileInputStream(mSampleFile);
    String url = mProtocolString + ":" + mSampleFile;
    mFactory
        .mapIO(url, new InputOutputStreamHandler(stream, null, true), false);
    int retval = -1;

    // Test all the different ways to open a valid file.
    mHandler = mFactory.getHandler(HumbleIO.DEFAULT_PROTOCOL, url, flags);
    assertTrue("could not find a mHandler using the mFactory", mHandler != null);

    // the mFactory should pass the URL to the mHandler
    retval = mHandler.open(null, flags);
    assertEquals(0, retval);

    retval = mHandler.close();
    assertEquals(0, retval);

    retval = mHandler.open(url, flags);
    assertTrue(retval == 0);

    retval = mHandler.close();
    assertEquals(0, retval);

    // now, try opening using FFMPEG
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, url, flags);
    assertEquals(0, retval);

    retval = FfmpegIO.url_close(handle);
    assertTrue(retval == 0);

    assertNotNull(mFactory.unmapIO(url));
  }

  @Test
  public void testAutoUnmapping() throws FileNotFoundException
  {
    int flags = IURLProtocolHandler.URL_RDONLY_MODE;

    FileInputStream stream = new FileInputStream(mSampleFile);

    // map the URL, telling the factory to unmap it
    // when closed
    String url = mProtocolString + ":1";
    mFactory.mapIO(url, new InputOutputStreamHandler(stream, null, true), true);
    int retval = -1;

    // now, try opening using FFMPEG
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, url, flags);
    assertEquals(0, retval);

    assertNull("found handler when we expected it to be unmapped", mFactory
        .unmapIO(url));

    retval = FfmpegIO.url_close(handle);
    assertEquals(0, retval);

  }

  @Test
  public void testFileRead() throws FileNotFoundException
  {
    // open our file
    FileInputStream stream = new FileInputStream(mSampleFile);
    mHandler = new InputOutputStreamHandler(stream, null, true);

    int retval = 0;
    retval = mHandler.open(mSampleFile, IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue(retval >= 0);

    long bytesRead = 0;

    byte[] buffer = new byte[1024];
    while ((retval = mHandler.read(buffer, buffer.length)) > 0)
    {
      bytesRead += retval;
    }
    // and close
    retval = mHandler.close();
    assertTrue(retval >= 0);
    assertEquals(4546420, bytesRead);
  }

  @Test
  public void testFileWrite() throws FileNotFoundException
  {

    String copyFile = this.getClass().getName() + "_" + this.getName() + ".flv";

    FileInputStream inStream = new FileInputStream(mSampleFile);
    FileOutputStream outStream = new FileOutputStream(copyFile);
    mHandler = new InputOutputStreamHandler(null, outStream, true);
    int retval = 0;

    // First, open the write mHandler.
    retval = mHandler.open(copyFile, IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue(retval >= 0);

    // Now, create and open a read mHandler.
    // note that without a protocol string, should default to file:
    IURLProtocolHandler reader = new InputOutputStreamHandler(inStream, null,
        true);
    retval = reader.open(null, IURLProtocolHandler.URL_RDONLY_MODE);

    long bytesWritten = 0;
    long totalBytes = 0;

    byte[] buffer = new byte[1024];
    while ((retval = reader.read(buffer, buffer.length)) > 0)
    {
      totalBytes += retval;
      // Write the output.
      retval = mHandler.write(buffer, retval);
      assertTrue(retval >= 0);
      bytesWritten += retval;
    }
    assertEquals(totalBytes, bytesWritten);
    assertEquals(4546420, totalBytes);

    // close each file
    retval = reader.close();
    assertTrue(retval >= 0);

    retval = mHandler.close();
    assertTrue(retval >= 0);

  }

  @Test
  public void testFFMPEGUrlRead()
  {
    try {
      testFFMPEGUrlReadTestFile(mSampleFile);
      fail("should not allow reading a file without a protocol");
    } catch (Exception e) {
      // pass.
    }
  }

  @Test
  public void testFFMPEGIOUrlRead() throws FileNotFoundException
  {
    FileInputStream stream = new FileInputStream(mSampleFile);
    mFactory.mapIO(mSampleFile,
        new InputOutputStreamHandler(stream, null, true), true);
    testFFMPEGUrlReadTestFile(mProtocolString + ":" + mSampleFile);
  }

  private void testFFMPEGUrlReadTestFile(String filename)
  {

    long retval = 0;
    // Call url_open wrapper
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, filename,
        IURLProtocolHandler.URL_RDONLY_MODE);
    assertTrue("url_open(" + filename + ") failed: " + retval, retval >= 0);

    // call url_read wrapper
    long bytesRead = 0;
    byte[] buffer = new byte[1024];
    while ((retval = FfmpegIO.url_read(handle, buffer, buffer.length)) > 0)
    {
      bytesRead += retval;
    }
    assertEquals(4546420, bytesRead);

    // call url_close wrapper
    retval = FfmpegIO.url_close(handle);
    assertTrue("url_close failed: " + retval, retval >= 0);
  }

  @Test
  public void testFfmpegIoUrlWrite() throws FileNotFoundException
  {
    String outFile = this.getClass().getName() + "_" + this.getName() + ".flv";
    FileOutputStream stream = new FileOutputStream(outFile);
    mFactory.mapIO(outFile, new InputOutputStreamHandler(null, stream, true),
        true);
    testFFMPEGUrlWriteTestFile(mProtocolString + ":" + outFile);
  }

  private void testFFMPEGUrlWriteTestFile(String filename)
  {
    int retval = 0;
    // Call url_open wrapper
    FfmpegIOHandle handle = new FfmpegIOHandle();

    retval = FfmpegIO.url_open(handle, filename,
        IURLProtocolHandler.URL_WRONLY_MODE);
    assertTrue("url_open failed for filename: " + filename + "; : " + retval,
        retval >= 0);

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

  @Test
  public void testFFMPEGIORandomAccesssRead() throws FileNotFoundException
  {
    String inName = mSampleFile;
    RandomAccessFile stream = new RandomAccessFile(inName, "rw");
    mFactory.mapIO(mSampleFile, new DataInputOutputHandler(stream), true);
    testFFMPEGUrlReadTestFile(mProtocolString + ":" + inName);
  }

  @Test
  public void testFFMPEGIORandomAccesssWrite() throws FileNotFoundException
  {
    String outName = this.getClass().getName() + this.getName() + ".flv";
    RandomAccessFile stream = new RandomAccessFile(outName, "rw");
    mFactory.mapIO(outName, new DataInputOutputHandler(stream), true);
    testFFMPEGUrlWriteTestFile(mProtocolString + ":" + outName);
  }

  @Test
  public void testFFMPEGIOFileChannelWrite() throws IOException
  {
    String outName = this.getClass().getName() + this.getName() + ".flv";

    FileOutputStream fileOutputStream = new FileOutputStream(outName);
    WritableByteChannel channel = fileOutputStream.getChannel();
    mFactory.mapIO(outName, new ReadableWritableChannelHandler(null, channel, true), true);
    testFFMPEGUrlWriteTestFile(mProtocolString + ":" + outName);
    channel.close();
    fileOutputStream.close();
  }

  @Test
  public void testFFMPEGIOFileChannelRead() throws IOException
  {
    String inName = mSampleFile;
    FileInputStream fileInputStream = new FileInputStream(inName);
    ReadableByteChannel channel = fileInputStream.getChannel();
    mFactory.mapIO(mSampleFile, new ReadableWritableChannelHandler(channel, null, true), true);
    testFFMPEGUrlReadTestFile(mProtocolString + ":" + mSampleFile);
    channel.close();
    fileInputStream.close();
  }

  @Test
  public void testFFMPEGIOUrlProtocolHandlerRead()
  {
    IURLProtocolHandler handler = new FileProtocolHandler(mSampleFile);
    mFactory.mapIO(mSampleFile, handler, true);
    testFFMPEGUrlReadTestFile(mProtocolString + ":" + mSampleFile);
  }

  @Test
  public void testFFMPEGIOURLProtocolHandlerWrite()
  {
    String outName = this.getClass().getName() + this.getName() + ".flv";

    IURLProtocolHandler handler = new FileProtocolHandler(outName);
    mFactory.mapIO(outName, handler, true);
    testFFMPEGUrlWriteTestFile(mProtocolString + ":" + outName);
  }

}
