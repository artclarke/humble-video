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
#ifndef INDEXENTRY_H_
#define INDEXENTRY_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/HumbleVideo.h>

namespace io { namespace humble { namespace video
{

/**
 * An index entry for a {@link Stream}.
 * <p>
 * Some ContainerFormats can maintain index of where key-frames
 * (and other interesting frames) can be found in a byte-stream.
 * This is really helpful for implementing efficient seeking (for
 * example, you can find all index entries near a desired timestamp,
 * and you'll find the nearest key-frame).
 * </p>
 * <p>
 * We don't maintain a complete list of all ContainerFormats that support
 * index, but if they do, you can query the {@link Stream#getNumIndexEntries()}
 * method to find how many entires are in the index.  Some ContainerFormats can
 * parse the relevant Container message if an index is embedded in the
 * container (for example, the MOV and MP4 demuxer can do this).  Other
 * ContainerFormats can create an index automatically as they read the file,
 * even if an index is not embedded in the container (for example the FLV
 * demuxer does this).
 * </p>
 *
 * @see Stream#findTimeStampEntryInIndex(long, int)
 * @see Stream#findTimeStampPositionInIndex(long, int)
 * @see Stream#getIndexEntry(int)
 * @see Stream#getNumIndexEntries()
 * @see Stream#getIndexEntries()
 *
 */
class VS_API_HUMBLEVIDEO IndexEntry: public io::humble::ferry::RefCounted
{
public:
  /**
   * A bit mask value that may be set in {@link #getFlags}.
   */
  static const int32_t INDEX_FLAG_KEYFRAME = 0x0001;

  /**
   * Create a new {@link IndexEntry} with the specified
   * values.
   *
   * @param position The value to be returned from {@link #getPosition()}.
   * @param timeStamp The value to be returned from {@link #getTimeStamp()}.
   * @param flags The value to be returned from {@link #getFlags()}.
   * @param size The value to be returned from {@link #getSize()}.
   * @param minDistance The value to be returned from {@link #getMinDistance()}.
   */
  static IndexEntry* make(int64_t position, int64_t timeStamp,
      int32_t flags, int32_t size, int32_t minDistance);

  /**
   * The position in bytes of the frame corresponding to this index entry
   * in the {@link Container}.
   * @return The byte-offset from start of the Container where the
   *   frame for this {@link IndexEntry} can be found.
   */
  virtual int64_t getPosition()=0;
  /**
   * The actual time stamp, in units of {@link Stream#getTimeBase()}, of the frame this entry points to.
   * @return The time stamp for this entry.
   */
  virtual int64_t getTimeStamp()=0;
  /**
   * Flags set for this entry.  See the INDEX_FLAG* constants
   * above.
   * @return the flags.
   */
  virtual int32_t getFlags()=0;

  /**
   * The size of bytes of the frame this index entry points to.
   * @return The size in bytes.
   */
  virtual int32_t getSize()=0;
  /**
   * Minimum number of index entries between this index entry
   * and the last keyframe in the index, used to avoid unneeded searching.
   * @return the minimum distance, in bytes.
   */
  virtual int32_t getMinDistance()=0;
  /**
   * Is this index entry pointing to a key frame.
   * Really shorthand for <code>{@link #getFlags()} &amp; {@link #INDEX_FLAG_KEYFRAME}</code>.
   * @return True if this index entry is for a key frame.
   */
  virtual bool    isKeyFrame()=0;

protected:
  IndexEntry();
  virtual ~IndexEntry();
};

}}}
#endif /* INDEXENTRY_H_ */
