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
 * AudioSamples.h
 *
 *  Created on: Jul 9, 2013
 *      Author: aclarke
 */

#ifndef AUDIOSAMPLES_H_
#define AUDIOSAMPLES_H_

#include <io/humble/video/MediaData.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO AudioSamples : public io::humble::video::MediaRawData
{
public:
  /**
   * Different type of audio sample formats.
   *
   * From libavutil/samplefmt.h
   */
  typedef enum Format
  {
    /** No sample format */
    SAMPLE_FMT_NONE = AV_SAMPLE_FMT_NONE,
    /**  unsigned 8 bits */
    SAMPLE_FMT_U8 = AV_SAMPLE_FMT_U8,
    /**  signed 16 bits */
    SAMPLE_FMT_S16 = AV_SAMPLE_FMT_S16,
    /**  signed 32 bits */
    SAMPLE_FMT_S32 = AV_SAMPLE_FMT_S32,
    /**  float */
    SAMPLE_FMT_FLT = AV_SAMPLE_FMT_FLT,
    /**  double */
    SAMPLE_FMT_DBL = AV_SAMPLE_FMT_DBL,

    /**  unsigned 8 bits, planar */
    SAMPLE_FMT_U8P = AV_SAMPLE_FMT_U8P,
    /**  signed 16 bits, planar */
    SAMPLE_FMT_S16P = AV_SAMPLE_FMT_S16P,
    /**  signed 32 bits, planar */
    SAMPLE_FMT_S32P = AV_SAMPLE_FMT_S32P,
    /**  float, planar */
    SAMPLE_FMT_FLTP = AV_SAMPLE_FMT_FLTP,
    /**  double, planar */
    SAMPLE_FMT_DBLP = AV_SAMPLE_FMT_DBLP,
  } Format;


protected:
  AudioSamples();
  virtual
  ~AudioSamples();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* AUDIOSAMPLES_H_ */
