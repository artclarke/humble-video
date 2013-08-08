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
#include <io/humble/ferry/RefPointer.h>

namespace io {
namespace humble {
namespace video {

/**
 * The parent class for all Raw media data.
 */
class VS_API_HUMBLEVIDEO MediaRaw: public io::humble::video::Media
{
public:
  /**
   * Get the time stamp of this object in getTimeBase() units.
   *
   * @return the time stamp
   */
  virtual int64_t getTimeStamp() { return getCtx()->pts; }

  /**
   * Set the time stamp for this object in getTimeBase() units.
   *
   * @param aTimeStamp The time stamp
   */
  virtual void setTimeStamp(int64_t aTimeStamp) { getCtx()->pts = aTimeStamp; }

  /**
   * Get the time base that time stamps of this object are represented in.
   *
   * @return the time base.
   */
  virtual Rational* getTimeBase() { return mTimeBase.get(); }

  /**
   * Is this object a key object?  i.e. it can be interpreted without needing any other media objects
   *
   * @return true if it's a key, false if not
   */
  virtual bool isKey() { return getCtx()->key_frame; }

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
  virtual int32_t getPacketSize() { return av_frame_get_pkt_size(getCtx()); };

  /**
   * duration of the corresponding packet, expressed in
   * ContainerStream.getTimeBase() units, 0 if unknown.
   * - encoding: unused
   * - decoding: Read by user.
   */
  virtual int64_t getPacketDuration() { return av_frame_get_pkt_duration(getCtx()); }

  /**
    * frame timestamp estimated using various heuristics, in stream time base
    * - encoding: unused
    * - decoding: set by libavcodec, read by user.
    */
   virtual int64_t getBestEffortTimeStamp() { return av_frame_get_best_effort_timestamp(getCtx()); }

   /**
    * @param value is the object complete or not.
    * @see #isComplete()
    */
   virtual void setComplete(bool value)=0;

#ifndef SWIG
   virtual AVFrame *getCtx()=0;
#endif
protected:
    MediaRaw() { }
    virtual ~MediaRaw() {}
private:
    io::humble::ferry::RefPointer<Rational> mTimeBase;
};

/**
 * Media that represents samples of some continugous stream.
 * Examples of Sampled data are MediaAudio (digital samples of continuous audio)
 * or MediaPicture (digital samples of a continuous visual stream).
 */
class VS_API_HUMBLEVIDEO MediaSampled : public MediaRaw {
public:
  /**
   * The number of samples in this frame of sampled data.
   */
  virtual int32_t getNumSamples()=0;
protected:
  MediaSampled() {}
  virtual ~MediaSampled() {}
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIARAW_H_ */
