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

import java.io.Closeable;
import java.io.DataInput;
import java.io.DataOutput;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.channels.ByteChannel;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.video.Container;

/**
 * Allows Humble Video to read from and write to many different types of Java I/O
 * objects, plus custom {@link IURLProtocolHandler} objects.
 * 
 * <p>
 * Most of the time, Container hides this away from you, but in case you're
 * interested, here's the underlying class doing the IO magic.
 * </p>
 * 
 * To use for reading (assuming an InputStream object called inputStream): </p>
 * 
 * <pre>
 * Container container = Container.make();
 * container.open(inputStream, null);
 * </pre>
 * <p>
 * or for writing:
 * </p>
 * 
 * <pre>
 * Container container = Container.make();
 * container.open(outputStream, null);
 * </pre>
 * <p>
 * Really. That's it.
 * </p>
 * <p>
 * 
 * All streams that are mapped in this factory share the same name space, even
 * if registered under different protocols. So, if "exampleone" and "exampletwo"
 * were both registered as protocols for this factory, then
 * "exampleone:filename" is the same as "exampletwo:filename" and will map to
 * the same input and output streams. In reality, they are all mapped to the
 * {@link #DEFAULT_PROTOCOL} protocol.
 * 
 * </p>
 */

public class HumbleIO implements IURLProtocolHandlerFactory
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  /**
   * The default protocol string that this factory uses (
   * {@value #DEFAULT_PROTOCOL}).
   */

  public final static String DEFAULT_PROTOCOL = "humble";

  /**
   * Do we undo mappings on open by default?
   */
  private final static boolean DEFAULT_UNMAP_URL_ON_OPEN = true;

  /**
   * Do we call {@link Closeable#close()} by default when closing?
   */
  private final static boolean DEFAULT_CLOSE_STREAM_ON_CLOSE = true;

  /**
   * A thread-safe mapping between URLs and registration information
   */
  private final ConcurrentMap<String, RegistrationInformation> mURLs = new ConcurrentHashMap<String, RegistrationInformation>();

  /**
   * The singleton Factory object for this class loader.
   */
  private final static HumbleIO mFactory = new HumbleIO();

  /**
   * The static constructor just registered the singleton factory under the
   * DEFAULT_PROTOCOL protocol.
   */
  static
  {
    registerFactory(DEFAULT_PROTOCOL);
  }

  /**
   * Package level constructor. We don't allow people to create their own
   * version of this factory
   */
  HumbleIO()
  {

  }

  /**
   * Register a new protocol name for this factory that Humble.IO will use for
   * the given protocol.
   * 
   * <p>
   * 
   * A default factory for the protocol {@value #DEFAULT_PROTOCOL} will already
   * be registered. This just allows you to register the same factory under
   * different strings if you want.
   * 
   * </p>
   * <p>
   * 
   * <strong>NOTE: Protocol can only contain alpha characters.</strong>
   * 
   * </p>
   * 
   * @param protocolPrefix The protocol (e.g. "yourapphandler").
   * @return The factory registered
   */

  static HumbleIO registerFactory(String protocolPrefix)
  {
    URLProtocolManager manager = URLProtocolManager.getManager();
    manager.registerFactory(protocolPrefix, mFactory);
    return mFactory;
  }

  /**
   * Get the singleton factory object for this class.
   * 
   * @return the factory
   */

  static public HumbleIO getFactory()
  {
    return mFactory;
  }

  /**
   * Generates a unique name suitable for using in the map methods for the URL
   * parameter.
   * 
   * @param src The object you want to generate a unique name for, or null if
   *        you don't have one.
   * @return A unique name (will be unique across time and space).
   */
  static public String generateUniqueName(Object src)
  {
    return generateUniqueName(src, null);
  }

  /**
   * Generates a unique name suitable for using in the map methods for the URL
   * parameter.
   * 
   * @param src The object you want to generate a unique name for, or null if
   *        you don't have one.
   * @param extension an option extension to append to the generated URL.
   * @return A unique name (will be unique across time and space).
   */
  static public String generateUniqueName(Object src, String extension)
  {
    StringBuilder builder = new StringBuilder();
    builder.append(UUID.randomUUID().toString());
    if (src != null)
    {
      builder.append("-");
      builder.append(src.getClass().getName());
      builder.append("-");
      builder.append(Integer.toHexString(src.hashCode()));
    }
    if (extension != null)
    {
      builder.append(extension);
    }
    return builder.toString();
  }


  /**
   * Maps a {@link IURLProtocolHandler} to a url that Humble can open.
   * 
   * {@link #unmap(String)} will be called automatically after this URL is
   * opened.
   * 
   * @param handler the handler
   * @return a string that is suitable for passing to {@link Container}'s open
   *         methods.
   */
  public static String map(IURLProtocolHandler handler)
  {
    return map(generateUniqueName(handler), handler, DEFAULT_UNMAP_URL_ON_OPEN);
  }

  /**
   * Maps a {@link IURLProtocolHandler} to a url that Humble can open.
   * 
   * {@link #unmap(String)} will be called automatically after this URL is
   * opened.
   * 
   * @param url the unique string to use for the mapping.
   * @param handler the handler
   * @return a string that is suitable for passing to {@link Container}'s open
   *         methods.
   */
  public static String map(String url, IURLProtocolHandler handler)
  {
    return map(url, handler, DEFAULT_UNMAP_URL_ON_OPEN);
  }

  /**
   * Maps a {@link DataInput} object to a URL for use by Humble.
   * 
   * @param input the {@link DataInput}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(DataInput input)
  {
    return map(generateUniqueName(input), input, null,
        DEFAULT_UNMAP_URL_ON_OPEN, DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link DataInput} object to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param input the {@link DataInput}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(String url, DataInput input)
  {
    return map(url, input, null, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link DataOutput} object to a URL for use by Humble.
   * 
   * @param output the {@link DataOutput}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(DataOutput output)
  {
    return map(generateUniqueName(output), null, output,
        DEFAULT_UNMAP_URL_ON_OPEN, DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link DataOutput} object to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param output the {@link DataOutput}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(String url, DataOutput output)
  {
    return map(url, null, output, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link RandomAccessFile} object to a URL for use by Humble.
   * 
   * @param file the {@link RandomAccessFile}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(RandomAccessFile file)
  {
    return map(generateUniqueName(file), file, file, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link RandomAccessFile} object to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param file the {@link RandomAccessFile}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(String url, RandomAccessFile file)
  {
    return map(url, file, file, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link ReadableByteChannel} to a URL for use by Humble.
   * 
   * @param channel the {@link ReadableByteChannel}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(ReadableByteChannel channel)
  {
    return map(generateUniqueName(channel), channel, null,
        DEFAULT_UNMAP_URL_ON_OPEN, DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link ReadableByteChannel} to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param channel the {@link ReadableByteChannel}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(String url, ReadableByteChannel channel)
  {
    return map(url, channel, null, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link WritableByteChannel} to a URL for use by Humble.
   * 
   * @param channel the {@link WritableByteChannel}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(WritableByteChannel channel)
  {
    return map(generateUniqueName(channel), null, channel,
        DEFAULT_UNMAP_URL_ON_OPEN, DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link WritableByteChannel} to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param channel the {@link WritableByteChannel}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(String url, WritableByteChannel channel)
  {
    return map(url, null, channel, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link ByteChannel} to a URL for use by Humble.
   * 
   * @param channel the {@link ByteChannel}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(ByteChannel channel)
  {
    return map(generateUniqueName(channel), channel, channel,
        DEFAULT_UNMAP_URL_ON_OPEN, DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link ByteChannel} to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param channel the {@link ByteChannel}
   * @return a string that can be passed to {@link Container}'s open methods.
   */
  public static String map(String url, ByteChannel channel)
  {
    return map(url, channel, channel, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps an {@link InputStream} to a URL for use by Humble.
   * 
   * @param in the stream
   * 
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(InputStream in)
  {
    return map(generateUniqueName(in), in, null, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps an {@link InputStream} to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param in the stream
   * 
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(String url, InputStream in)
  {
    return map(url, in, null, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps an {@link OutputStream} to a URL for use by Humble.
   * 
   * @param out the stream
   * 
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(OutputStream out)
  {
    return map(generateUniqueName(out), null, out, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps an {@link OutputStream} to a URL for use by Humble.
   * 
   * @param url the URL to use.
   * @param out the stream
   * 
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(String url, OutputStream out)
  {
    return map(url, null, out, DEFAULT_UNMAP_URL_ON_OPEN,
        DEFAULT_CLOSE_STREAM_ON_CLOSE);
  }

  /**
   * Maps a {@link DataInput} or {@link DataOutput} object to a URL for use by
   * Humble.
   * 
   * @param url the url
   * @param in the input to use
   * @param out the output to use
   * @param unmapOnOpen should we remove the mapping as soon as the resulting
   *        handler is opened?
   * @param closeOnClose should we call {@link Closeable#close()} on the
   *        underlying input or output objects when
   *        {@link IURLProtocolHandler#close()} is called (if supported by the
   *        underlying object)?
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(String url, DataInput in, DataOutput out,
      boolean unmapOnOpen, boolean closeOnClose)
  {
    return map(url, new DataInputOutputHandler(in, out, closeOnClose));
  }

  /**
   * Maps an {@link ReadableByteChannel} or {@link WritableByteChannel} to a URL
   * for use by Humble.
   * 
   * @param url the url
   * @param in the input to use
   * @param out the output to use
   * @param unmapOnOpen should we remove the mapping as soon as the resulting
   *        handler is opened?
   * @param closeOnClose should we call {@link Closeable#close()} on the
   *        underlying input or output objects when
   *        {@link IURLProtocolHandler#close()} is called (if supported by the
   *        underlying object)?
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(String url, ReadableByteChannel in,
      WritableByteChannel out, boolean unmapOnOpen, boolean closeOnClose)
  {
    return map(url, new ReadableWritableChannelHandler(in, out, closeOnClose));
  }

  /**
   * Maps an {@link InputStream} or {@link OutputStream} to a URL for use by
   * Humble.
   * 
   * @param url the url
   * @param in the input to use
   * @param out the output to use
   * @param unmapOnOpen should we remove the mapping as soon as the resulting
   *        handler is opened?
   * @param closeOnClose should we call {@link Closeable#close()} on the
   *        underlying input or output objects when
   *        {@link IURLProtocolHandler#close()} is called?
   * 
   * @return Returns a URL that can be passed to Humble's {@link Container}'s
   *         open method, and will result in IO being performed on the passed in
   *         streams.
   */

  public static String map(String url, InputStream in, OutputStream out,
      boolean unmapOnOpen, boolean closeOnClose)
  {
    return map(url, new InputOutputStreamHandler(in, out, closeOnClose));
  }

  /**
   * Maps a {@link IURLProtocolHandler} to a url that Humble can open.
   * 
   * @param url the unique string to use for the mapping.
   * @param handler the handler
   * @param unmapUrlOnOpen if true, when Humble opens the
   *        {@link IURLProtocolHandler}, {@link #unmap(String)} will be called
   *        automatically.
   * @return a string that is suitable for passing to {@link Container}'s open
   *         methods.
   */
  public static String map(String url, IURLProtocolHandler handler,
      boolean unmapUrlOnOpen)
  {
    if (mFactory.mapIO(url, handler, unmapUrlOnOpen) != null)
      throw new RuntimeException("url is already mapped: " + url);

    return DEFAULT_PROTOCOL + ":" + URLProtocolManager.getResourceFromURL(url);
  }

  /**
   * Undoes a URL to {@link InputStream} or {@link OutputStream} mapping.
   * Forwards to {@link #getFactory()}.{@link #unmapIO(String)}
   */
  public static IURLProtocolHandler unmap(String url)
  {
    return mFactory.unmapIO(url);
  }

  /**
   * Maps the given url or file name to the given {@link IURLProtocolHandler} or
   * so that Humble calls to open the URL it will call back to the handler.
   * 
   * <p>
   * 
   * If you set unmapOnOpen to false, or you never actually open this mapping,
   * then you must ensure that you call {@link #unmapIO(String)} at some point
   * in time to remove the mapping, or we will hold onto references to the
   * handler you passed in.
   * 
   * </p>
   * 
   * @param url A file or URL. If a URL, the protocol will be stripped off and
   *        replaced with {@link #DEFAULT_PROTOCOL} when registering.
   * @param handler An {@link IURLProtocolHandler} for the url
   * @param unmapUrlOnOpen If true, the handler will unmap itself after an
   *        {@link Container} opens the registered URL. If true, you do not
   *        need to call {@link #unmapIO(String)} for this url.
   * @return The previous handler for this url, or null if none.
   * 
   * 
   * @throws IllegalArgumentException if url is null or zero length
   * @throws IllegalArgumentException if handler is null
   */

  public IURLProtocolHandler mapIO(String url, IURLProtocolHandler handler,
      boolean unmapUrlOnOpen)
  {
    {
      if (url == null || url.length() <= 0)
        throw new IllegalArgumentException("must pass in non-zero url");
      if (handler == null)
      {
        throw new IllegalArgumentException("must pass in a non null handler");
      }
      String streamName = URLProtocolManager.getResourceFromURL(url);
      RegistrationInformation tuple = new RegistrationInformation(streamName,
          handler, unmapUrlOnOpen);
      RegistrationInformation oldTuple = mURLs.putIfAbsent(streamName, tuple);
      return oldTuple == null ? null : oldTuple.getHandler();
    }
  }

  /**
   * Unmaps a registration between a URL and the underlying i/o objects.
   * <p>
   * If URL contains a protocol it is ignored when trying to find the matching
   * IO stream.
   * </p>
   * 
   * @param url The stream name to unmap
   * @return the {@link IURLProtocolHandler} that had been registered for that
   *         url, or null if none.
   */
  public IURLProtocolHandler unmapIO(String url)
  {
    if (url == null || url.length() <= 0)
      throw new IllegalArgumentException("must pass in non-zero url");
    String streamName = URLProtocolManager.getResourceFromURL(url);
    RegistrationInformation oldTuple = mURLs.remove(streamName);
    return oldTuple == null ? null : oldTuple.getHandler();
  }

  /**
   * {@inheritDoc}
   */

  public IURLProtocolHandler getHandler(String protocol, String url, int flags)
  {
    // Note: We need to remove any protocol markers from the url
    String streamName = URLProtocolManager.getResourceFromURL(url);

    RegistrationInformation tuple = mURLs.get(streamName);
    if (tuple != null)
    {
      IURLProtocolHandler handler = tuple.getHandler();
      if (tuple.isUnmappingOnOpen())
      {
        IURLProtocolHandler oldHandler = unmapIO(tuple.getName());

        // the unmapIO is an atomic operation
        if (handler != null && !handler.equals(oldHandler))
        {
          // someone already unmapped this stream
          log.error("stream {} already unmapped; it was likely already opened",

          tuple.getName());
          return null;
        }
      }
      return handler;
    }
    return null;
  }

  /**
   * A set of information about a registered handler.
   * 
   * @author aclarke
   * 
   */

  private static class RegistrationInformation
  {
    private final String mName;
    private final boolean mIsUnmappingOnOpen;
    private final IURLProtocolHandler mHandler;

    public RegistrationInformation(String streamName,
        IURLProtocolHandler handler, boolean unmapUrlOnOpen)
    {
      mName = streamName;
      mHandler = handler;
      mIsUnmappingOnOpen = unmapUrlOnOpen;
    }

    /**
     * The name of this handler registration, without any protocol.
     * 
     * @return the name
     */
    public String getName()
    {
      return mName;
    }

    /**
     * Should the handler unmap the stream after it is opened?
     * 
     * @return the decision
     */
    public boolean isUnmappingOnOpen()
    {
      return mIsUnmappingOnOpen;
    }

    public IURLProtocolHandler getHandler()
    {
      return mHandler;
    }
  }
}
