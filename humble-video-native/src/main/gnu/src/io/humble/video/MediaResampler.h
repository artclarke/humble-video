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
/*
 * MediaResampler.h
 *
 *  Created on: Jul 26, 2014
 *      Author: aclarke
 */

#ifndef MEDIARESAMPLER_H_
#define MEDIARESAMPLER_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/MediaRaw.h>

namespace io {
namespace humble {
namespace video {

/**
 * Super class for Media Resampler objects.
 *
 * A resampler converts a type of media (e.g. Picture) to another
 * type of the same media with some parameters change (e.g. color).
 */
class VS_API_HUMBLEVIDEO MediaResampler : public io::humble::video::Configurable
{
public:
  typedef enum State {
    STATE_INITED,
    STATE_OPENED,
    STATE_ERROR
  } State;
  virtual State getState()=0;

  /**
   * Resample in to out based on the resampler parameters.
   *
   * Resamples the in media based on the parameters set when
   * this resampler was constructed.
   *
   * @param out The media we'll resample to.  Check
   *     MediaSampled#isComplete() after the call.
   * @param in The media we'll resample from.
   *
   * @throws InvalidArgument if in our out does not match the parameters this
   *         resampler was set with.
   * @throws InvalidArgument if the sub-class of in our out is of an invalid
   *         type (e.g. passsing audio to a picture resampler).
   */
  virtual int32_t resample(MediaSampled* out, MediaSampled* in)=0;

protected:
  MediaResampler();
  virtual
  ~MediaResampler();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIARESAMPLER_H_ */
