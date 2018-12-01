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

#ifndef STREAM_H_
#define STREAM_H_
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>
namespace io { namespace humble { namespace video
{
  class Container;
  class Rational;
  class KeyValueBag;
  class MediaPacket;
  class IndexEntry;
  class MediaParameters;
  
  /**
   * Represents a stream of similar data (eg video) in a Container.
   * <p>
   * Streams are really virtual concepts; Container objects really just contain
   * a bunch of Packets.  But each Packet usually has a stream
   * id associated with it, and all Packets with that stream id represent
   * the same type of (usually time-based) data.  For example in many FLV
   * video files, there is a stream with id "0" that contains all video data, and
   * a stream with id "1" that contains all audio data.
   * </p><p>
   * You use an Stream object to get properly configured Decoders
   * for decoding, and to tell Encoders how to encode Packets when
   * decoding.
   * </p>
   */
  class VS_API_HUMBLEVIDEO ContainerStream : public io::humble::ferry::RefCounted
  {
  public:
    /** The disposition of this stream. Some streams can have special
     * meanings in some Containers.
     */
    typedef enum Disposition {
      DISPOSITION_NONE = 0,
      DISPOSITION_DEFAULT= AV_DISPOSITION_DEFAULT,
      DISPOSITION_DUB = AV_DISPOSITION_DUB,
      DISPOSITION_ORIGINAL = AV_DISPOSITION_ORIGINAL,
      DISPOSITION_COMMENT = AV_DISPOSITION_COMMENT,
      DISPOSITION_LYRICS = AV_DISPOSITION_LYRICS,
      DISPOSITION_KARAOKE =  AV_DISPOSITION_KARAOKE,

      /**
       * Track should be used during playback by default.
       * Useful for subtitle track that should be displayed
       * even when user did not explicitly ask for subtitles.
       */
      DISPOSITION_FORCED = AV_DISPOSITION_FORCED,

      /** stream for hearing impaired audiences */
      DISPOSITION_HEARING_IMPAIRED = AV_DISPOSITION_HEARING_IMPAIRED,
      /** stream for visual impaired audiences */
      DISPOSITION_VISUAL_IMPAIRED =  AV_DISPOSITION_VISUAL_IMPAIRED,
      /**< stream without voice */
      DISPOSITION_CLEAN_EFFECTS = AV_DISPOSITION_CLEAN_EFFECTS,
      /**
       * The stream is stored in the file as an attached picture/"cover art" (e.g.
       * APIC frame in ID3v2). The single packet associated with it will be returned
       * among the first few packets read from the file unless seeking takes place.
       * It can also be accessed at any time in #getAttachedPic().
       */
      DISPOSITION_ATTACHED_PIC =  AV_DISPOSITION_ATTACHED_PIC,
    } Disposition;
    /**
     * What types of parsing can we do on a call to
     * Source#read(Packet)
     */
    typedef enum ParseType {
      /** No special instructions */
      PARSE_NONE=AVSTREAM_PARSE_NONE,
      /** full parsing and repack */
      PARSE_FULL=AVSTREAM_PARSE_FULL,
      /** Only parse headers, do not repack. */
      PARSE_HEADERS=AVSTREAM_PARSE_HEADERS,
      /** full parsing and interpolation of timestamps for frames not starting on a packet boundary */
      PARSE_TIMESTAMPS=AVSTREAM_PARSE_TIMESTAMPS,
      /** full parsing and repack of the first frame only, only implemented for H.264 currently */
      PARSE_FULL_ONCE=AVSTREAM_PARSE_FULL_ONCE,
        /** full parsing and repack with timestamp and position generation by parser for raw
           this assumes that each packet in the file contains no demuxer level headers and
           just codec level data, otherwise position generation would fail */
      PARSE_FULL_RAW=AVSTREAM_PARSE_FULL_RAW
    } ParseType;

    /**
     * Get the relative position this stream has in the hosting
     * Container object.
     * @return The Index within the Container of this stream.
     */
    virtual int32_t getIndex();

    /**
     * Return a container format specific id for this stream.
     * @return The (container format specific) id of this stream.
     */
    virtual int32_t getId();

    /**
     * Get the (sometimes estimated) average frame rate of this container.
     * For variable frame-rate containers (they do exist) this is just
     * an approximation.  Better to use getTimeBase().
     *
     * For contant frame-rate containers, this will be 1 / ( getTimeBase() )
     *
     * @return The frame-rate of this container.
     */
    virtual Rational * getFrameRate();

    /**
     * The time base in which all timestamps (e.g. Presentation Time Stamp (PTS)
     * and Decompression Time Stamp (DTS)) are represented.  For example
     * if the time base is 1/1000, then the difference between a PTS of 1 and
     * a PTS of 2 is 1 millisecond.  If the timebase is 1/1, then the difference
     * between a PTS of 1 and a PTS of 2 is 1 second.
     *
     * @return The time base of this stream.
     */
    virtual Rational * getTimeBase();

    /**
     * Return the start time, in #getTimeBase() units, when this stream
     * started.
     * @return The start time.
     */
    virtual int64_t getStartTime();

    /**
     * Return the duration, in #getTimeBase() units, of this stream,
     * or Global#NO_PTS if unknown.
     * @return The duration (in getTimeBase units) of this stream, if known.
     */
    virtual int64_t getDuration();

    /**
     * The current Decompression Time Stamp that will be used on this stream,
     * in #getTimeBase() units.
     * @return The current Decompression Time Stamp that will be used on this stream.
     */
    virtual int64_t getCurrentDts();

    /**
     * Get the number of index entries in this stream.
     * @return The number of index entries in this stream.
     * @see #getIndexEntry(int)
     */
    virtual int getNumIndexEntries();

    /**
     * Returns the number of encoded frames if known.  Note that frames here means
     * encoded frames, which can consist of many encoded audio samples, or
     * an encoded video frame.
     *
     * @return The number of frames (encoded) in this stream.
     */
    virtual int64_t getNumFrames();
    
    /**
     * Gets the sample aspect ratio.
     *
     * @return The sample aspect ratio.
     */
    virtual Rational* getSampleAspectRatio();

    /**
     * Get the underlying container for this stream, or null if Humble Video
     * doesn't know.
     * 
     * @return the container, or null if we don't know.
     */
    virtual Container* getContainer();
    
    /**
     * Get how the decoding codec should parse data from this stream.
     * @return the parse type.
     */
    virtual ContainerStream::ParseType getParseType();
    
    /**
     * Set the parse type the decoding codec should use.  Set to
     * ParseType#PARSE_NONE if you don't want any parsing
     * to be done.
     * <p>
     * Warning: do not set this flag unless you know what you're doing,
     * and do not set after you've started reading packets.
     * </p>
     * 
     * @param type The type to set.
     */
    virtual void setParseType(ParseType type);

    /**
     * Get the KeyValueBag for this object,
     * or null if none.
     * <p>
     * If the Container or Stream object
     * that this KeyValueBag came from was opened
     * for reading, then changes via KeyValueBag#setValue(String, String)
     * will have no effect on the underlying media.
     * </p>
     * <p>
     * If the Container or Stream object
     * that this KeyValueBag came from was opened
     * for writing, then changes via KeyValueBag#setValue(String, String)
     * will have no effect after Container#writeHeader()
     * is called.
     * </p>
     * @return the KeyValueBag.
     */
   virtual KeyValueBag* getMetaData();

   /**
    * Search for the given time stamp in the key-frame index for this Stream.
    * <p>
    * Not all ContainerFormat implementations
    * maintain key frame indexes, but if they have one,
    * then this method searches in the Stream index
    * to quickly find the byte-offset of the nearest key-frame to
    * the given time stamp.
    * </p>
    * @param wantedTimeStamp the time stamp wanted, in the stream's
    *                        time base units.
    * @param flags A bitmask of the <code>SEEK_FLAG_*</code> flags, or 0 to turn
    *              all flags off.  If Container#SEEK_FLAG_BACKWARDS then the returned
    *              index will correspond to the time stamp which is <=
    *              the requested one (not supported by all demuxers).
    *              If Container#SEEK_FLAG_BACKWARDS is not set then it will be >=.
    *              if Container#SEEK_FLAG_ANY seek to any frame, only
    *              keyframes otherwise (not supported by all demuxers).
    * @return The IndexEntry for the nearest appropriate timestamp
    *   in the index, or null if it can't be found.
    */
   virtual IndexEntry* findTimeStampEntryInIndex(
       int64_t wantedTimeStamp, int32_t flags);

   /**
    * Search for the given time stamp in the key-frame index for this Stream.
    * <p>
    * Not all ContainerFormat implementations
    * maintain key frame indexes, but if they have one,
    * then this method searches in the Stream index
    * to quickly find the index entry position of the nearest key-frame to
    * the given time stamp.
    * </p>
    * @param wantedTimeStamp the time stamp wanted, in the stream's
    *                        time base units.
    * @param flags A bitmask of the <code>SEEK_FLAG_*</code> flags, or 0 to turn
    *              all flags off.  If Container#SEEK_FLAG_BACKWARDS then the returned
    *              index will correspond to the time stamp which is <=
    *              the requested one (not supported by all demuxers).
    *              If Container#SEEK_FLAG_BACKWARDS is not set then it will be >=.
    *              if Container#SEEK_FLAG_ANY seek to any frame, only
    *              keyframes otherwise (not supported by all demuxers).
    * @return The position in this Stream index, or -1 if it cannot
    *   be found or an index is not maintained.
    * @see #getIndexEntry(int)
    */
   virtual int32_t findTimeStampPositionInIndex(
       int64_t wantedTimeStamp, int32_t flags);

   /**
    * Get the IndexEntry at the given position in this
    * Stream object's index.
    * <p>
    * Not all ContainerFormat types maintain
    * Stream indexes, but if they do,
    * this method can return those entries.
    * </p>
    * <p>
    * Do not modify the Container this stream
    * is from between calls to this method and
    * #getNumIndexEntries() as indexes may
    * be compacted while processing.
    * </p>
    * @param position The position in the index table.
    */
   virtual IndexEntry* getIndexEntry(int32_t position);

   /**
    * Get the Stream.Disposition of this stream.
    */
   virtual ContainerStream::Disposition getDisposition();

   /**
    * For containers with Stream.Disposition.DISPOSITION_ATTACHED_PIC,
    * this returns a read-only copy of the packet containing the
    * picture (needs to be decoded separately).
    */
   virtual MediaPacket* getAttachedPic();

   /**
    * Check if the stream is matched by the stream specifier.
    *
    * See the "stream specifiers" chapter in the FFmpeg documentation for the syntax
    * of specifier: https://www.ffmpeg.org/ffmpeg.html#Stream-selection
    *
    * @param specifier the specifier string
    * @return  true if this stream is matched by specifier; false if this stream is not
    *   matched by specifier;
    *
    * @throw InvalidArgument if the specifier is invalid.
    *
    * @note  A stream specifier can match several streams in a container.
    */
   virtual bool matchSpecifier(const char* specifier);

   /**
    * Get the codec parameters. This structure can be used
    * with other Processors, sometimes.
    *
    */
   virtual MediaParameters* getMediaParameters();


#ifndef SWIG

   virtual void setTimeBase(Rational *);
   virtual void setFrameRate(Rational *);

   virtual void setSampleAspectRatio(Rational* newRatio);

   virtual void setMetaData(KeyValueBag* metaData);

   virtual int32_t addIndexEntry(IndexEntry* entry);
   virtual void setId(int32_t id);

   AVStream* getCtx();

#endif
  protected:
    virtual ~ContainerStream();
    ContainerStream(Container*, int32_t);
  private:
    int32_t mIndex;
    io::humble::ferry::RefPointer<Container> mContainer;
  };
}}}

#endif /*STREAM_H_*/
