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
 * Coder.h
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#ifndef CODER_H_
#define CODER_H_

#include <io/humble/video/Configurable.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO Coder : public io::humble::video::Configurable
{
  typedef enum Flag {
    FLAG_UNALIGNED = CODEC_FLAG_UNALIGNED,
    /** Use fixed qscale. */
    FLAG_QSCALE = CODEC_FLAG_QSCALE,
    /** 4 MV per MB allowed / advanced prediction for H.263. */
    FLAG_4MV = CODEC_FLAG_4MV,
    /** Use qpel MC. */
    FLAG_QPEL = CODEC_FLAG_QPEL,
    /** Use GMC. */
    FLAG_GMC = CODEC_FLAG_GMC,
    /** Always try a MB with MV=<0,0>. */
    FLAG_MV0 = CODEC_FLAG_MV0,
    /**
     * The parent program guarantees that the input for B-frames containing
     * streams is not written to for at least s->max_b_frames+1 frames, if
     * this is not set the input will be copied.
     */
    FLAG_INPUT_PRESERVED = CODEC_FLAG_INPUT_PRESERVED,
    /** Use internal 2pass ratecontrol in first pass mode. */
    FLAG_PASS1 = CODEC_FLAG_PASS1,
    /** Use internal 2pass ratecontrol in second pass mode. */
    FLAG_PASS2 = CODEC_FLAG_PASS2,
    /** Only decode/encode grayscale. */
    FLAG_GRAY = CODEC_FLAG_GRAY,
    /** Don't draw edges. */
    FLAG_EMU_EDGE = CODEC_FLAG_EMU_EDGE,
    /** error[?] variables will be set during encoding. */
    FLAG_PSNR = CODEC_FLAG_PSNR,
              /** Input bitstream might be truncated at a random
                location instead of only at frame boundaries. */
    FLAG_TRUNCATED = CODEC_FLAG_TRUNCATED,
    /** Normalize adaptive quantization. */
    FLAG_NORMALIZE_AQP = CODEC_FLAG_NORMALIZE_AQP,
    /** Use interlaced DCT. */
    FLAG_INTERLACED_DCT = CODEC_FLAG_INTERLACED_DCT,
    /** Force low delay. */
    FLAG_LOW_DELAY = CODEC_FLAG_LOW_DELAY,
    /** Place global headers in extradata instead of every keyframe. */
    FLAG_GLOBAL_HEADER = CODEC_FLAG_GLOBAL_HEADER,
    /** Use only bitexact stuff (except (I)DCT). */
    FLAG_BITEXACT = CODEC_FLAG_BITEXACT,
    /* Fx : Flag for h263+ extra options */
    /** H.263 advanced intra coding / MPEG-4 AC prediction */
    FLAG_AC_PRED = CODEC_FLAG_AC_PRED,
    /** loop filter */
    FLAG_LOOP_FILTER = CODEC_FLAG_LOOP_FILTER,
    /** interlaced motion estimation */
    FLAG_INTERLACED_ME = CODEC_FLAG_INTERLACED_ME,
    FLAG_CLOSED_GOP = CODEC_FLAG_CLOSED_GOP,
  } Flag;

  typedef enum Flag2 {
    /** Allow non spec compliant speedup tricks. */
    FLAG2_FAST = CODEC_FLAG2_FAST,
    /** Skip bitstream encoding. */
    FLAG2_NO_OUTPUT = CODEC_FLAG2_NO_OUTPUT,
    /** Place global headers at every keyframe instead of in extradata. */
    FLAG2_LOCAL_HEADER = CODEC_FLAG2_LOCAL_HEADER,
    /** timecode is in drop frame format. DEPRECATED!!!! */
    FLAG2_DROP_FRAME_TIMECODE = CODEC_FLAG2_DROP_FRAME_TIMECODE,
    /** Discard cropping information from SPS. */
    FLAG2_IGNORE_CROP = CODEC_FLAG2_IGNORE_CROP,

    /** Input bitstream might be truncated at a packet boundaries instead of only at frame boundaries. */
    FLAG2_CHUNKS = CODEC_FLAG2_CHUNKS,
    /** Show all frames before the first keyframe */
    FLAG2_SHOW_ALL = CODEC_FLAG2_SHOW_ALL,
  } Flag2;
protected:
  Coder();
  virtual
  ~Coder();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CODER_H_ */
