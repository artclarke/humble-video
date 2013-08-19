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
/*
 * Muxer.h
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#ifndef MUXER_H_
#define MUXER_H_

#include <io/humble/video/Container.h>
#include <io/humble/video/MuxerFormat.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/MuxerStream.h>
#include <io/humble/video/Encoder.h>

#ifndef SWIG
#ifdef MUXER_H_

#include <io/humble/video/customio/URLProtocolHandler.h>

#endif
#endif // ! SWIG

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO Muxer : public io::humble::video::Container
{
public:

  /**
   * Creates a new muxer.
   *
   * One of the three passed in parameter must be non-null. If the muxer requires a URL to write to,
   * then that must be specified.
   *
   * @param format If non null, this will be the format this muxer assumes it is writting packets in.
   * @param filename The filename/url to open. If format is null, this will also be examined to guess actual format.
   * @param formatName The formatname of the muxer to use. This will only be examined if format is null.
   *
   * @return a Muxer
   *
   * @throws InvalidArgument if all parameters are null.
   */
  static Muxer*
  make(MuxerFormat* format, const char* filename, const char* formatName);

  /**
   * Get the URL the Muxer was opened with.
   * May return null if unknown.
   * @return the URL opened, or null.
   */
  virtual const char*
  getURL();

  /**
   * Get the MuxerFormat associated with this Muxer
   * or null if unknown.
   */
  virtual MuxerFormat *
  getFormat() { return mFormat.get(); }

  /**
   * Muxers can only be in one of these states.
   */
  typedef enum State
  {
    /**
     * Initialized but not yet opened. Transitions to STATE_OPENED or STATE_ERROR.
     * New streams can be added.
     */
    STATE_INITED,
    /**
     * File is opened, and header is written. For most formats,
     * you can no longer add new streams. Check flags to find out if you can.
     */
    STATE_OPENED,

    /**
     * Trailer is written, file is closed and all file-resources have been released. The Muxer
     * should be discarded.
     */
    STATE_CLOSED,
    /**
     * An error has occured.
     */
    STATE_ERROR
  } State;

  /**
   * Get the current state of the Muxer.
   */
  virtual State
  getState() {
    return mState;
  }

  virtual void
  open(KeyValueBag* inputOptions, KeyValueBag* outputOptions);

  virtual void
  close();

  /**
   * Get the number of streams in this container.
   */
  int32_t getNumStreams() { return getFormatCtx()->nb_streams; }

  /**
   * Set the buffer length Humble Video will suggest to FFMPEG for writing output data.
   *
   * If called when a Container is open, the call is ignored and -1 is returned.
   *
   * @param size The suggested buffer size.
   * @throws InvalidArgument if size <= 0
   */
  virtual void
  setOutputBufferLength(int32_t size);

  /**
   * Return the output buffer length.
   *
   * @return The input buffer length Humble Video told FFMPEG to assume.
   *   0 means FFMPEG should choose it's own
   *   size (and it'll probably be 32768).
   */
  virtual int32_t
  getOutputBufferLength();

  /**
   * Adds a new stream that will have packets written to it that are encoded
   * by the given Encoder.
   *
   * Note on thread safety: Callers must ensure that the encoder is not encoding
   * packets at the same time that Muxer#open or Muxer#close is being called. It should
   * be safe to call Encoder#encodeVideo or Encoder#encodeAudio whem Muxer#writePacket
   * is being called. But if possible, we recommend serializing Encoder and Muxer calls
   * as not all FFmpeg encoder implements can be thread-safe outside the container they
   * write to.
   *
   * @param encoder The encoder that will be used for packets written to this stream.
   *
   * @throws InvalidArgument if encoder is null.
   * @throws InvalidArgument if encoder is not open.
   *
   */
  virtual MuxerStream*
  addNewStream(Encoder* encoder);

  /**
   * Writes the given packet to the Muxer.
   *
   * @param packet The packet to write. If null, it tells the muxer to flush any data queued up to
   *   the underlying storage (disk, network, etc).
   *
   * @returns true if all data has been flushed, false if data remains to be flushed.
   *
   * @throw InvalidArgument if packet is null.
   * @throw InvalidArgument if packet is not complete.
   * @throw RuntimeException for other errors.
   */
  virtual bool
  writePacket(MediaPacket* packet);

//
//  /**
//   * Takes the packet given (in whatever timebase it was encoded with) and resets all timestamps
//   * to align with the stream in this container that it will be added to.
//   *
//   * @param packet The packet to stamp. Packet#setStreamIndex must have been called to avoid an error,
//   *  Packet#getStreamIndex must point to a stream number in this muxer.
//   */
//  virtual void stampOutputPacket(MediaPacket* packet);

protected:
  virtual AVFormatContext* getFormatCtx() { return mCtx; }
  Muxer(MuxerFormat* format, const char* filename, const char* formatName);
  virtual
  ~Muxer();
private:
  State mState;
  AVFormatContext* mCtx;
  io::humble::video::customio::URLProtocolHandler* mIOHandler;

  io::humble::ferry::RefPointer<MuxerFormat> mFormat;
  int32_t mBufferLength;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MUXER_H_ */
