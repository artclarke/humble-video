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

#ifndef MEDIAPACKET_H_
#define MEDIAPACKET_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Media.h>
#include <io/humble/video/Coder.h>

namespace io {
namespace humble {
namespace video {

/**
 * A packet of encoded data that was read from a Demuxer or
 * will be written to a Muxer.
 */
class VS_API_HUMBLEVIDEO MediaPacket : public io::humble::video::MediaEncoded
{
  /*
   * Note: This class is a pure-virtual interface. Actual
   * implementation should be in PacketImpl
   */
public:

  typedef enum SideDataType {
    DATA_UNKNOWN = -1,

    DATA_PALETTE = AV_PKT_DATA_PALETTE,
    DATA_NEW_EXTRADATA = AV_PKT_DATA_NEW_EXTRADATA,

    /**
     * An AV_PKT_DATA_PARAM_CHANGE side data packet is laid out as follows:
     * @code
     * u32le param_flags
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT)
     *     s32le channel_count
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT)
     *     u64le channel_layout
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE)
     *     s32le sample_rate
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS)
     *     s32le width
     *     s32le height
     * @endcode
     */
    DATA_PARAM_CHANGE = AV_PKT_DATA_PARAM_CHANGE,

    /**
     * An AV_PKT_DATA_H263_MB_INFO side data packet contains a number of
     * structures with info about macroblocks relevant to splitting the
     * packet into smaller packets on macroblock edges (e.g. as for RFC 2190).
     * That is, it does not necessarily contain info about all macroblocks,
     * as long as the distance between macroblocks in the info is smaller
     * than the target payload size.
     * Each MB info structure is 12 bytes, and is laid out as follows:
     * @code
     * u32le bit offset from the start of the packet
     * u8    current quantizer at the start of the macroblock
     * u8    GOB number
     * u16le macroblock address within the GOB
     * u8    horizontal MV predictor
     * u8    vertical MV predictor
     * u8    horizontal MV predictor for block number 3
     * u8    vertical MV predictor for block number 3
     * @endcode
     */
    DATA_H263_MB_INFO = AV_PKT_DATA_H263_MB_INFO,

    /**
     * Recommmends skipping the specified number of samples
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    DATA_SKIP_SAMPLES = AV_PKT_DATA_SKIP_SAMPLES,

    /**
     * An AV_PKT_DATA_JP_DUALMONO side data packet indicates that
     * the packet may contain "dual mono" audio specific to Japanese DTV
     * and if it is true, recommends only the selected channel to be used.
     * @code
     * u8    selected channels (0=mail/left, 1=sub/right, 2=both)
     * @endcode
     */
    DATA_JP_DUALMONO = AV_PKT_DATA_JP_DUALMONO,

    /**
     * A list of zero terminated key/value strings. There is no end marker for
     * the list, so it is required to rely on the side data size to stop.
     */
    DATA_STRINGS_METADATA = AV_PKT_DATA_STRINGS_METADATA,

    /**
     * Subtitle event position
     * @code
     * u32le x1
     * u32le y1
     * u32le x2
     * u32le y2
     * @endcode
     */
    DATA_SUBTITLE_POSITION = AV_PKT_DATA_SUBTITLE_POSITION,

    /**
     * Data found in BlockAdditional element of matroska container. There is
     * no end marker for the data, so it is required to rely on the side data
     * size to recognize the end. 8 byte id (as found in BlockAddId) followed
     * by data.
     */
    DATA_MATROSKA_BLOCKADDITIONAL = AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,

    /**
     * The optional first identifier line of a WebVTT cue.
     */
    DATA_WEBVTT_IDENTIFIER = AV_PKT_DATA_WEBVTT_IDENTIFIER,

    /**
     * The optional settings (rendering instructions) that immediately
     * follow the timestamp specifier of a WebVTT cue.
     */
    DATA_WEBVTT_SETTINGS = AV_PKT_DATA_WEBVTT_SETTINGS,

  } SideDataType;

  /**
   * Create a new Packet
   */
  static MediaPacket*
  make();

  /**
   * Allocate a new packet that wraps an existing Buffer.
   *
   * NOTE: At least 16 bytes of the passed in buffer will be used
   * for header information, so the resulting Packet.getSize() 
   * will be smaller than Buffer.getBufferSize() .
   *
   * @param buffer The Buffer to wrap.
   * @return a new packet or null on error.
   */
  static MediaPacket*
  make(io::humble::ferry::Buffer* buffer);

  /**
   * Allocate a new packet wrapping the existing contents of
   * a passed in packet.  Callers can then modify
   * #getPts(),
   * #getDts() and other get/set methods without
   * modifying the original packet.
   *
   * @param packet Packet to reuse buffer from and to
   *   copy settings from.
   * @param copyData if true copy data from packet
   *   into our own buffer.  If false, share the same
   *   data buffer that packet uses
   *
   * @return a new packet or null on error.
   */
  static MediaPacket*
  make(MediaPacket *packet, bool copyData);

  /**
   * Allocate a new packet.
   * <p>
   * Note that any buffers this packet needs will be
   * lazily allocated (i.e. we won't actually grab all
   * the memory until we need it).
   * </p>
   * @param size The maximum size, in bytes, of data you
   *   want to put in this packet.
   *
   * @return a new packet, or null on error.
   */
  static MediaPacket*
  make(int32_t size);


  /**
   * Get any underlying raw data available for this packet.
   *
   * @return The raw data, or null if not accessible.
   */
  virtual io::humble::ferry::Buffer* getData()=0;

  /**
   * Get the number of side data elements in this packet.
   */
  virtual int32_t getNumSideDataElems()=0;

  /**
   * Get the n'th item of SideData.
   * <p>
   * WARNING: Callers must ensure that the the packet object
   * this is called form is NOT reset or destroyed while using this buffer,
   * as unfortunately we cannot ensure this buffer survives the
   * underlying packet data.
   * </p>
   *
   * @param n The n'th item to get.
   * @return the data, or null if none found
   * @throws InvalidArgument if n < 0 || n >= #getNumSideDataElems()
   */
  virtual io::humble::ferry::Buffer* getSideData(int32_t n)=0;
  /**
   * Get the n'th item of SideData.
   *
   * @param n The n'th item to get.
   * @return the data, or SideDataType.DATA_UNKNOWN if none found
   * @throws InvalidArgument if n < 0 || n >= #getNumSideDataElems()
   */
  virtual SideDataType getSideDataType(int32_t n) = 0;

  /**
   * Get the Presentation Time Stamp (PTS) for this packet.
   *
   * This is the time at which the payload for this packet should
   * be <strong>presented</strong> to the user, in units of
   * #getTimeBase(), relative to the start of stream.
   *
   * @return Get the Presentation Timestamp for this packet.
   */
  virtual int64_t
  getPts()=0;

  /**
   * Set a new Presentation Time Stamp (PTS) for this packet.
   *
   * @param aPts a new PTS for this packet.
   *
   * @see #getPts()
   */
  virtual void
  setPts(int64_t aPts)=0;

  /**
   * Get the Decompression Time Stamp (DTS) for this packet.
   * <p>
   * This is the time at which the payload for this packet should
   * be <strong>decompressed</strong>, in units of
   * #getTimeBase(), relative to the start of stream.
   * </p>
   * <p>
   * Some media codecs can require packets from the &quot;future&quot; to
   * be decompressed before earliest packets as an additional way to compress
   * data.  In general you don't need to worry about this, but if you're
   * curious start reading about the difference between I-Frames, P-Frames
   * and B-Frames (or Bi-Directional Frames).  B-Frames can use information
   * from future frames when compressed.
   * </p>
   * @return Get the Decompression Timestamp (i.e. when this was read relative
   * to the start of reading packets).
   */
  virtual int64_t
  getDts()=0;

  /**
   * Set a new Decompression Time Stamp (DTS) for this packet.
   * @param aDts a new DTS for this packet.
   * @see #getDts()
   */
  virtual void
  setDts(int64_t aDts)=0;

  /**
   * Get the size in bytes of the payload currently in this packet.
   * @return Size (in bytes) of payload currently in packet.
   */
  virtual int32_t
  getSize()=0;

  /**
   * Get the maximum size (in bytes) of payload this packet can hold.
   * @return Get maximum size (in bytes) of payload this packet can hold.
   */
  virtual int32_t
  getMaxSize()=0;

  /**
   * Get the container-specific index for the stream this packet is
   * part of.
   * @return Stream in container that this packet has data for, or <0 if unsure.
   */
  virtual int32_t
  getStreamIndex()=0;

  /**
   * Get any flags set on this packet, as a 4-byte binary-ORed bit-mask.
   * This is access to raw FFMPEG
   * flags, but it is easier to use the is* methods below.
   * @return Any flags on the packet.
   */
  virtual int32_t
  getFlags()=0;

  /**
   * Does this packet contain Key data? i.e. data that needs no other
   * frames or samples to decode.
   * @return true if key; false otherwise.
   */
  virtual bool
  isKeyPacket()=0;

  /**
   * Return the duration of this packet, in units of #getTimeBase()
   * @return Duration of this packet, in same time-base as the PTS.
   */
  virtual int64_t
  getDuration()=0;

  /**
   * Return the position (in bytes) of this packet in the stream.
   * @return The position of this packet in the stream, or -1 if
   *   unknown.
   */
  virtual int64_t
  getPosition()=0;

  /**
   * Set if this is a key packet.
   *
   * @param keyPacket true for yes, false for no.
   */
  virtual void
  setKeyPacket(bool keyPacket)=0;

  /**
   * Set any internal flags.
   *
   * @param flags Flags to set
   */
  virtual void
  setFlags(int32_t flags)=0;

  /**
   * Set the stream index for this packet.
   *
   * @param streamIndex The stream index, as determined from the IContainer this packet will be written to.
   */
  virtual void
  setStreamIndex(int32_t streamIndex)=0;

  /**
   * Set the duration.
   * @param duration new duration
   * @see #getDuration()
   */
  virtual void
  setDuration(int64_t duration)=0;

  /**
   * Set the position.
   * @param position new position
   * @see #getPosition()
   */
  virtual void
  setPosition(int64_t position)=0;

  /**
   * Time difference in IStream#getTimeBase() units
   * from the presentation time stamp of this
   * packet to the point at which the output from the decoder has converged
   * independent from the availability of previous frames. That is, the
   * frames are virtually identical no matter if decoding started from
   * the very first frame or from this keyframe.
   * Is Global#NO_PTS if unknown.
   * This field is not the display duration of the current packet.
   * <p>
   * The purpose of this field is to allow seeking in streams that have no
   * keyframes in the conventional sense. It corresponds to the
   * recovery point SEI in H.264 and match_time_delta in NUT. It is also
   * essential for some types of subtitle streams to ensure that all
   * subtitles are correctly displayed after seeking.
   * </p>
   * <p>
   * If you didn't follow that, try drinking one to two glasses
   * of Absinthe.  It won't help, but it'll be more fun.
   * </p>
   *
   * @return the convergence duration
   */
  virtual int64_t
  getConvergenceDuration()=0;

  /**
   * Set the convergence duration.
   * @param duration the new duration
   */
  virtual void
  setConvergenceDuration(int64_t duration)=0;

  /**
   * Discard the current payload and allocate a new payload.
   * <p>
   * Note that if any people have access to the old payload using
   * getData(), the memory will continue to be available to them
   * until they release their hold of the Buffer.
   * </p>
   * <p>
   * When requesting a packet size, the system
   *   may allocate a larger payloadSize.
   * </p>
   * @param payloadSize The (minimum) payloadSize of this packet in bytes. It is ok to
   *   pass in 0 here, in which case the packet will later allocate memory if needed.
   */
  virtual void
  reset(int32_t payloadSize)=0;

#ifndef SWIG

  // do not return this via SWIG. Instead we will need to make a helper
  // method that casts this to a Decoder or an Encoder, and then casts
  // it back up from Java.

  /**
   * Gets the Coder* that made this packet.
   *
   * @return the coder if known, or null if not.
   */
  virtual Coder*
  getCoder()=0;
#endif

protected:
  MediaPacket();
  virtual
  ~MediaPacket();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAPACKET_H_ */
