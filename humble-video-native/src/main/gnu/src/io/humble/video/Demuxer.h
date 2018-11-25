/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef DEMUXER_H_
#define DEMUXER_H_

#include <io/humble/video/MediaPacket.h>
#include <io/humble/video/Container.h>
#include <io/humble/video/DemuxerStream.h>
#include <io/humble/video/DemuxerFormat.h>

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/HumbleException.h>

namespace io {
namespace humble {
namespace video {

/**
 * A Container that MediaPacket objects can be read from.
 */
class VS_API_HUMBLEVIDEO Demuxer : public io::humble::video::Container
{
  /**
   * Note: This class is a pure-virtual interface. Actual
   * implementation should be in DemuxerImpl.
   */
public:

  /**
   * Create a new Demuxer
   */
  static Demuxer*
  make();

  /**
   * Demuxers can only be in one of these states:
   */
  typedef enum State
  {
    /**
     * STATE_INITED: Allocated but open has not been called yet. Transitions to STATE_OPENED when ::open(...)
     *   is successfully called, or STATE_ERROR if ::open(...) has an error.
     */
    STATE_INITED,
    /**
     * STATE_OPENED: Opened and read to read or write data. Transitions to STATE_CLOSED on successful ::close(...)
     *   or STATE_ERROR if ::close(...) has an error.
     */
    STATE_OPENED,
    /**
     * Some Sources can be live (i.e. in Play mode)
     */
    STATE_PLAYING,
    /**
     * Some containers (e.g. networks) can be paused.
     */
    STATE_PAUSED,
    /**
     * STATE_CLOSED: Container is closed and should be discarded.
     */
    STATE_CLOSED,
    /**
     * STATE_ERROR: Container had an error and should be discarded.
     */
    STATE_ERROR,
  } State;

  /**
   * Get the current state of the container.
   */
  virtual State getState()=0;

  /**
   * Get the DemuxerFormat associated with this Demuxer
   * or null if unknown.
   */
  virtual DemuxerFormat *
  getFormat() = 0;

  /**
   * Set the buffer length Humble Video will suggest to FFMPEG for reading inputs.
   *
   * If called when a Container is open, the call is ignored and -1 is returned.
   *
   * @param size The suggested buffer size.
   * @throws InvalidArgument if size <= 0
   */
  virtual void
  setInputBufferLength(int32_t size)=0;

  /**
   * Return the input buffer length.
   *
   * @return The input buffer length Humble Video told FFMPEG to assume.
   *   0 means FFMPEG should choose it's own
   *   size (and it'll probably be 32768).
   */
  virtual int32_t
  getInputBufferLength()=0;

  /**
   * Open this container and make it ready for reading, optionally
   * reading as far into the container as necessary to find all streams.
   * <p>The caller must call #close() when done, but if not, the
   * Demuxer will eventually close
   * them later but warn to the logging system.
   *
   * @param url The resource to open; The format of this string is any
   *   url that FFMPEG supports (including additional protocols if added
   *   through the video.customio library).
   * @param type The type of this container.
   * @param format A pointer to a InputFormat object specifying
   *   the format of this container, or null if you want us to guess.
   * @param streamsCanBeAddedDynamically If true, open() will expect that new
   *   streams can be added at any time, even after the format header has been read.
   * @param queryStreamMetaData If true, open() will call #queryStreamMetaData()
   *   on this container, which will potentially block until it has ready
   *   enough data to find all streams in a container.  If false, it will only
   *   block to read a minimal header for this container format.
   * @param options If not null, a set of key-value pairs that will be set on the container immediately
   *   the format is determined.  Some options cannot be set on input formats until
   *   the system has had a chance to determine what the format is, so this
   *   is the only way to set InputFormat-specific options.
   * @param optionsNotSet If not null, on return this KeyValueBag object will be cleared out, and
   *   replaced with any key/value pairs that were in <code>options</code> but could not be set on this
   *   Demuxer.
   *
   */
  virtual void
  open(const char *url, DemuxerFormat* format, bool streamsCanBeAddedDynamically,
      bool queryStreamMetaData, KeyValueBag* options,
      KeyValueBag* optionsNotSet)=0;

  /**
   * Close the container.  open() must have been called first, or
   * else an error is returned.
   * <p>
   * If this method exits because of an interruption,
   * all resources will be closed anyway.
   * </p>
   *
   */
  virtual void
  close()=0;

  /**
   * Get the stream at the given position.
   *
   * @param streamIndex the index of this stream in the container
   * @return The stream at that position in the container, or null if none there.
   */
  virtual DemuxerStream*
  getStream(int32_t streamIndex)=0;

  /**
   * Reads the next packet in the Demuxer into the Packet.  This method will
   * release any buffers currently held by this packet and allocate
   * new ones.
   * <p>
   * For non-blocking IO data sources, it is possible for this method
   * to return as successful but with no complete packet. In that case
   * the caller should retry again later (think EAGAIN) semantics.
   * </p>
   *
   * @param  packet [In/Out] The packet the Demuxer will read into.
   *
   * @return 0 if successful, or <0 if not.
   * @throws RuntimeException if an error occurs except for EOF (in which case <0 returned)
   *         or EAGAIN (in which case 0 returned with an incomplete packet).
   */
  virtual int32_t
  read(MediaPacket *packet)=0;

  /**
   * Attempts to read all the meta data in this stream, potentially by reading ahead
   * and decoding packets.
   * <p>
   * Any packets this method reads ahead will be cached and correctly returned when you
   * read packets, but this method can be non-blocking potentially until end of container
   * to get all meta data.  Take care when you call it.
   * </p><p>After this method is called, other meta data methods like #getDuration() should
   * work.</p>
   *
   */
  virtual void
  queryStreamMetaData()=0;

  /**
   * Gets the duration, if known, of this container.
   *
   * This will only work for non-streamable containers where Demuxer
   * can calculate the container size.
   *
   * @return The duration, or Global#NO_PTS if not known.
   */
  virtual int64_t
  getDuration()=0;

  /**
   * Get the starting timestamp in microseconds of the first packet of the earliest stream in this container.
   * <p>
   * This will only return value values either either (a) for non-streamable
   * containers where Demuxer can calculate the container size or
   * (b) after Demuxer has actually read the
   * first packet from a streamable Demuxer.
   * </p>
   *
   * @return The starting timestamp in microseconds, or Global#NO_PTS if not known.
   */
  virtual int64_t
  getStartTime()=0;

  /**
   * Get the file size in bytes of this container.
   *
   * This will only return a valid value if the container is non-streamed and supports seek.
   *
   * @return The file size in bytes, or <0 on error.
   */
  virtual int64_t
  getFileSize()=0;

  /**
   * Get the calculated overall bit rate of this file.
   * <p>
   * This will only return a valid value if the container is non-streamed and supports seek.
   * </p>
   * @return The overall bit rate in bytes per second, or <0 on error.
   */
  virtual int32_t
  getBitRate()=0;

  /**
   * Get the flags associated with this object.
   *
   * @return The (compacted) value of all flags set.
   */
  virtual int32_t
  getFlags()=0;

  /**
   * Set the flags to use with this object.  All values
   * must be ORed (|) together.
   *
   * @see Flags
   *
   * @param newFlags The new set flags for this codec.
   */
  virtual void
  setFlags(int32_t newFlags) = 0;

  /**
   * Get the setting for the specified flag
   *
   * @param flag The flag you want to find the setting for
   *
   * @return 0 for false; non-zero for true
   */
  virtual bool
  getFlag(Flag flag) = 0;

  /**
   * Set the flag.
   *
   * @param flag The flag to set
   * @param value The value to set it to (true or false)
   *
   */
  virtual void
  setFlag(Flag flag, bool value) = 0;

  /**
   * Get the URL the Demuxer was opened with.
   * May return null if unknown.
   * @return the URL opened, or null.
   */
  virtual const char*
  getURL()=0;

  /**
   * Get the number of times Demuxer#readNextPacket(Packet)
   * will retry a read if it gets a Error.Type#ERROR_AGAIN
   * value back.
   *
   * Defaults to 1 times.  <0 means it will keep retrying indefinitely.
   *
   * @return the read retry count
   */
  virtual int32_t
  getReadRetryCount()=0;

  /**
   * Sets the read retry count.
   *
   * @see #getReadRetryCount()
   *
   * @param count The read retry count.  <0 means keep trying.
   */
  virtual void
  setReadRetryCount(int32_t count)=0;

  /**
   * Can streams be added dynamically to this container?
   *
   * @return true if streams can be added dynamically
   */
  virtual bool
  canStreamsBeAddedDynamically()=0;

  /**
   * Get the KeyValueBag of media MetaData for this object,
   * or null if none.
   * <p>
   * If the Demuxer or IStream object
   * that this KeyValueBag came from was opened
   * for reading, then changes via KeyValueBag#setValue(String, String)
   * will have no effect on the underlying media.
   * </p>
   * <p>
   * If the Demuxer or IStream object
   * that this KeyValueBag came from was opened
   * for writing, then changes via KeyValueBag#setValue(String, String)
   * will have no effect after Demuxer#writeHeader()
   * is called.
   * </p>
   * @return the KeyValueBag.
   */
  virtual KeyValueBag*
  getMetaData()=0;

  /**
   * Forces the Demuxer to assume all audio streams are
   * encoded with the given audio codec when demuxing.
   * @param id The codec id
   * @return < 0 on error (e.g. not an audio codec); >= 0 on success.
   */
  virtual int32_t
  setForcedAudioCodec(Codec::ID id)=0;

  /**
   * Forces the Demuxer to assume all video streams are
   * encoded with the given video codec when demuxing.
   * @param id The codec id
   * @return < 0 on error (e.g. not an video codec); >= 0 on success.
   */
  virtual int32_t
  setForcedVideoCodec(Codec::ID id)=0;

  /**
   * Forces the Demuxer to assume all subtitle streams are
   * encoded with the given subtitle codec when demuxing.
   * @param id The codec id
   * @return < 0 on error (e.g. not an subtitle codec); >= 0 on success.
   */
  virtual int32_t
  setForcedSubtitleCodec(Codec::ID id)=0;

  /**
   * Flags that can be bitmasked in the #seek method. If no
   * flag, then key-frame-only/forward seeking is assumed.
   */
  typedef enum SeekFlag
  {
    /**
     * Seek backwards.
     */
    SEEK_BACKWARD = AVSEEK_FLAG_BACKWARD,
    /** Seek based on position in bytes. */
    SEEK_BYTE = AVSEEK_FLAG_BYTE,
    /** Seek to any frame, even non-keyframes */
    SEEK_ANY = AVSEEK_FLAG_ANY,
    /** Seek based on frame number */
    SEEK_FRAME = AVSEEK_FLAG_FRAME,
  } SeekFlag;

  /**
   * Seek to timestamp ts.
   *
   * Seeking will be done so that the point from which all active streams
   * can be presented successfully will be closest to ts and within min/max_ts.
   * Active streams are all streams that have Stream.getDiscardSetting <
   * Codec.DISCARD_ALL.
   *
   * If flags contain SeekFlags.SEEK_BYTE, then all timestamps are in bytes and
   * are the file position (this may not be supported by all demuxers).
   * If flags contain SeekFlags.SEEK_FRAME, then all timestamps are in frames
   * in the stream with stream_index (this may not be supported by all demuxers).
   * Otherwise all timestamps are in units of the stream selected by stream_index
   * or if stream_index is -1, in (1/Global.DEFAULT_PTS_MICROSECONDS} units.
   * If flags contain SeekFlags.SEEK_ANY, then non-keyframes are treated as
   * keyframes (this may not be supported by all demuxers).
   *
   * @param stream_index index of the stream which is used as time base reference
   * @param min_ts smallest acceptable timestamp
   * @param ts target timestamp
   * @param max_ts largest acceptable timestamp
   * @param flags flags
   * @return >=0 on success, error code otherwise
   *
   * @note This is part of the new seek API which is still under construction.
   *       Thus do not use this yet. It may change at any time, do not expect
   *       ABI compatibility yet!
   */
  virtual int32_t
  seek(int32_t stream_index, int64_t min_ts, int64_t ts,
      int64_t max_ts, int32_t flags)=0;

  /**
   * Gets the AVFormatContext.max_delay property if possible.
   * @return The max delay, error code otherwise.
   */
  virtual int32_t
  getMaxDelay()=0;

  /**
   * Start playing a network source. Call #pause() to pause.
   */
  virtual void
  play()=0;

  /**
   * Pause a playing network source. Call #play() to unpause.
   *
   * @return 0 on success; <0 if state is not #State.STATE_PLAYING or error.
   */
  virtual void
  pause()=0;

protected:
  Demuxer();
  virtual
  ~Demuxer();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* DEMUXER_H_ */
