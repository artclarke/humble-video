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
 * MediaRaw.h
 *
 *  Created on: Jul 11, 2013
 *      Author: aclarke
 */

#ifndef MEDIARAW_H_
#define MEDIARAW_H_

#include <io/humble/video/Media.h>

namespace io {
namespace humble {
namespace video {

/**
 * The parent class for all Raw media data.
 */
class VS_API_HUMBLEVIDEO MediaRaw: public io::humble::video::MediaData
{
public:
  /** Get the presentation time stamp */
  virtual int64_t getPts() { return getCtx()->pts; }

  /** Get any meta-data associated with this media item */
  virtual KeyValueBag* getMetaData();

  /**
   * pts copied from the Packet that was decoded to produce this frame
   * - encoding: unused
   * - decoding: Read by user.
   */
  virtual int64_t getPacketPts() { return getCtx()->pkt_pts; }

  /**
   * dts copied from the Packet that triggered returning this frame
   * - encoding: unused
   * - decoding: Read by user.
   */
  virtual int64_t getPacketDts() { return getCtx()->pkt_dts; }

  /**
   * size of the corresponding packet containing the compressed
   * frame.
   * It is set to a negative value if unknown.
   * - encoding: unused
   * - decoding: set by libavcodec, read by user.
   */
  virtual int32_t getPacketSize() { return getCtx()->pkt_size; };

  /**
   * duration of the corresponding packet, expressed in
   * ContainerStream.getTimeBase() units, 0 if unknown.
   * - encoding: unused
   * - decoding: Read by user.
   */
  virtual int64_t getPacketDuration() { return getCtx()->pkt_duration; }

  /**
    * frame timestamp estimated using various heuristics, in stream time base
    * - encoding: unused
    * - decoding: set by libavcodec, read by user.
    */
   virtual int64_t getBestEffortTimeStamp() { return getCtx()->best_effort_timestamp; }

   /**
    * Total size in bytes of the decoded media.
    *
    * @return number of bytes of decoded media
    */
   virtual int32_t getSize()=0;

protected:
    MediaRaw() {}
    virtual ~MediaRaw() {}
    virtual AVFrame *getCtx()=0;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIARAW_H_ */
