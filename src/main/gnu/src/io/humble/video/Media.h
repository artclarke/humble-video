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

#ifndef MEDIADATA_H_
#define MEDIADATA_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Global.h>
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/IBuffer.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/KeyValueBag.h>

namespace io { namespace humble { namespace video {

/**
 * The parent class of all media objects than can be gotten from or added to a {@link Stream}.
 */
class VS_API_HUMBLEVIDEO MediaData: public io::humble::ferry::RefCounted
{
public:
  /**
   * Get the time stamp of this object in getTimeBase() units.
   * 
   * @return the time stamp
   */
  virtual int64_t getTimeStamp()=0;
  
  /**
   * Set the time stamp for this object in getTimeBase() units.
   * 
   * @param aTimeStamp The time stamp
   */
  virtual void setTimeStamp(int64_t aTimeStamp)=0;
  
  /**
   * Get the time base that time stamps of this object are represented in.
   * 
   * Caller must release the returned value.
   * 
   * @return the time base.
   */
  virtual Rational* getTimeBase()=0;
  
  /**
   * Is this object a key object?  i.e. it can be interpreted without needing any other media objects
   * 
   * @return true if it's a key, false if not
   */
  virtual bool isKey()=0;

  /**
   * Returns whether or not we think this buffer has been filled
   * with data.
   * 
   * 
   * @return Has setComplete() been called and the buffer populated.
   */
  virtual bool isComplete()=0;

  
protected:
  MediaData();
  virtual ~MediaData();
};

/**
 * The parent class for all Encoded media data.
 */
class VS_API_HUMBLEVIDEO MediaEncodedData: public io::humble::video::MediaData
{
public:
  /**
   * Set the time base that time stamps of this object are represented in.
   *
   * @param aBase the new time base.  If null an exception is thrown.
   */
  virtual void setTimeBase(Rational *aBase)=0;
protected:
  MediaEncodedData() {}
  virtual ~MediaEncodedData() {}
};
}}}

#endif /* MEDIADATA_H_ */
