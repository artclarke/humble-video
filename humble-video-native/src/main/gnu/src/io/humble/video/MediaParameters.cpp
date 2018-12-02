/*******************************************************************************
 * Copyright (c) 2018, Andrew "Art" Clarke.  All rights reserved.
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
 * MediaParameters.cpp
 *
 *  Created on: Nov 30, 2018
 *      Author: aclarke
 */

#include "MediaParameters.h"
#include "FfmpegIncludes.h"
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/Logger.h>

VS_LOG_SETUP(VS_CPP_PACKAGE.MediaParameters);

using namespace io::humble::ferry;

namespace io { namespace humble { namespace video {
  MediaParameters::MediaParameters() {
    mCtx = avcodec_parameters_alloc();
    if (!mCtx)
      VS_THROW(HumbleBadAlloc());
  }

  MediaParameters*
  MediaParameters::make(AVCodecParameters *ctx, Rational* tb) {
    if (!ctx)
      VS_THROW(HumbleInvalidArgument("null context not allowed"));

    RefPointer<MediaParameters> retval= MediaParameters::make();

    if (avcodec_parameters_copy(retval->mCtx, ctx) < 0) {
      VS_THROW(HumbleBadAlloc());
    }
    retval->mTimeBase.reset(tb, true);

    return retval.get();
  }

  MediaParameters::~MediaParameters() {
    if (mCtx)
      avcodec_parameters_free(&mCtx);
  }

}}}
