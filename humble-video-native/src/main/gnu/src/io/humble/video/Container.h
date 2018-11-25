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

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Property.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/ContainerFormat.h>
#include <io/humble/video/Coder.h>

namespace io
{
namespace humble
{
namespace video
{

  class ContainerStream;
/**
 * A Container for Media data. This is an abstract class and
 * cannot be instantiated on its own.
 */
class VS_API_HUMBLEVIDEO Container : public ::io::humble::video::Configurable
{
public:
  /**
   * Do not set these flags -- several are used by the internals of Humble Video.
   */
  typedef enum Flag
  {
    /**  Generate missing pts even if it requires parsing future frames. **/
    FLAG_GENPTS = AVFMT_FLAG_GENPTS,
    /**  Ignore index. **/
    FLAG_IGNIDX = AVFMT_FLAG_IGNIDX,
    /**  Do not block when reading packets from input. **/
    FLAG_NONBLOCK = AVFMT_FLAG_NONBLOCK,
    /**  Ignore DTS on frames that contain both DTS & PTS **/
    FLAG_IGNDTS = AVFMT_FLAG_IGNDTS,
    /**  Do not infer any values from other values, just return what is stored in the container **/
    FLAG_NOFILLIN = AVFMT_FLAG_NOFILLIN,
    /**  Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled **/
    FLAG_NOPARSE = AVFMT_FLAG_NOPARSE,
    /**  Do not buffer frames when possible **/
    FLAG_NOBUFFER = AVFMT_FLAG_NOBUFFER,
    /**  The caller has supplied a custom AVIOContext, don't avio_close() it. **/
    FLAG_CUSTOM_IO = AVFMT_FLAG_CUSTOM_IO,
    /**  Discard frames marked corrupted **/
    FLAG_DISCARD_CORRUPT = AVFMT_FLAG_DISCARD_CORRUPT,
    /** Flush the Container every packet. */
    FLAG_FLUSH_PACKETS = AVFMT_FLAG_FLUSH_PACKETS,
    /**
     * When muxing, try to avoid writing any random/volatile data to the output.
     * This includes any random IDs, real-time timestamps/dates, muxer version, etc.
     *
     * This flag is mainly intended for testing.
     */
    FLAG_BITEXACT = AVFMT_FLAG_BITEXACT,
#if FF_API_LAVF_MP4A_LATM
    /**  Enable RTP MP4A-LATM payload **/
    FLAG_MP4A_LATM = AVFMT_FLAG_MP4A_LATM,
#endif
    /**  try to interleave outputted packets by dts (using this flag can slow demuxing down) **/
    FLAG_SORT_DTS = AVFMT_FLAG_SORT_DTS,
    /**  Enable use of private options by delaying codec open (this could be made default once all code is converted) **/
    FLAG_PRIV_OPT = AVFMT_FLAG_PRIV_OPT,
#if FF_API_LAVF_KEEPSIDE_FLAG
    /**  Don't merge side data but keep it separate. **/
    FLAG_KEEP_SIDE_DATA = AVFMT_FLAG_KEEP_SIDE_DATA,
#endif
    /** Enable fast, but inaccurate seeks for some formats */
    FLAG_FAST_SEEK= AVFMT_FLAG_FAST_SEEK,
    /** Stop muxing when the shortest stream stops. */
    FLAG_SHORTEST = AVFMT_FLAG_SHORTEST,
    /** Add bitstream filters as requested by the muxer */
    FLAG_AUTO_BSF = AVFMT_FLAG_AUTO_BSF,
  } Flag;

  /**
   * The number of streams in this container.
   * <p>If this container is a Source this will query the stream and find out
   * how many streams are in it.</p><p>If the current thread is interrupted while this blocking method
   * is running the method will return with a negative value.
   * To check if the method exited because of an interruption
   * pass the return value to Error#make(int) and then
   * check Error#getType() to see if it is
   * Error.Type#ERROR_INTERRUPTED.
   * </p>
   *
   * @return The number of streams in this container.
   */
  virtual int32_t
  getNumStreams();

#ifndef SWIG
  virtual void* getCtx() { return getFormatCtx(); }
  virtual AVFormatContext* getFormatCtx()=0;

  /*
   *  This method contains classes that the Container shares internally inside Humble Video,
   *  but which Swig should not care about.
   *
   *  Mostly it is the meta-data associated with Streams. The big reason to have it here
   *  rather than in the ContainerStream and sub-class objects is that in FFmpeg
   *  the AVFormatContext object owns all the memory, and since we're a referencing-counting
   *  system we need to put our meta-data for streams in the same object that mirrors
   *  the AVFormatContext
   */
  class Stream {
  public:
    Stream(Container* container, int32_t index);
    ~Stream();

    Container*
    getContainer() {
      VS_REF_ACQUIRE(mContainer);
      return mContainer;
    }

    int32_t
    getIndex() const {
      return mIndex;
    }

    int64_t
    getLastDts() const {
      return mLastDts;
    }

    void
    setLastDts(int64_t lastDts) {
      mLastDts = lastDts;
    }

    KeyValueBag*
    getMetaData() {
      return mMetaData.get();
    }

    AVStream*
    getCtx() { return mCtx; }

    Coder*
    getCoder();

    void setCoder(Coder* coder) {
      mCoder.reset(coder, true);
    }

  private:
    int32_t mIndex;
    int64_t mLastDts;
    io::humble::ferry::RefPointer<KeyValueBag> mMetaData;
    io::humble::ferry::RefPointer<Coder> mCoder;
    Container* mContainer;
    AVStream* mCtx;
  };
  Stream* getStream(int32_t i);
#endif // ! SWIG

protected:
  Container();
  virtual
  ~Container();
#ifndef SWIG
  // static methods for custom IO
  static int url_read(void*h, unsigned char* buf, int size);
  static int url_write(void*h, unsigned char* buf, int size);
  static int64_t url_seek(void*h, int64_t position, int whence);
#endif // ! SWIG
protected:
  void doSetupStreams();
private:
  std::vector<Stream*> mStreams;

};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CONTAINER_H_ */
