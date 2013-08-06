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
 * FilterSource.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include "FilterSource.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/MediaAudioImpl.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

FilterSource::FilterSource(FilterGraph* graph, AVFilterContext* ctx) :
    FilterEndPoint(graph, ctx) {
}

FilterSource::~FilterSource() {
}

void
FilterSource::add(MediaRaw* media) {
  if (!media) {
    VS_THROW(HumbleInvalidArgument("no media passed in"));
  }
  if (!media->isComplete()) {
    VS_THROW(HumbleInvalidArgument("incomplete media passed in"));
  }
  // ok, let's get to work
  AVFilterContext* ctx = getFilterCtx();
  AVFrame* frame = media->getCtx();

  int e = av_buffersrc_write_frame(ctx, frame);
  FfmpegException::check(e, "could not add frame to audio source:");
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
