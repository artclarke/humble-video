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
 * FilterSink.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include "FilterSink.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/FilterGraph.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.FilterSink);

namespace io {
namespace humble {
namespace video {

FilterSink::FilterSink(FilterGraph* graph, AVFilterContext* ctx) :
    FilterEndPoint(graph, ctx) {
}

FilterSink::~FilterSink() {
}

void
FilterSink::setFrameSize(int32_t size) {
  AVFilterContext* ctx = getFilterCtx();
  if (!ctx->inputs[0])
    VS_THROW(HumbleInvalidArgument("Cannot setFrameSize until graph this is added to is opened"));

  av_buffersink_set_frame_size(ctx, size);
}

int32_t
FilterSink::getFrameSize() {
  AVFilterContext* ctx = getFilterCtx();

  // NOTE: This is peaking into the structure, and should
  // be replaced if when a av_buffersink_get_frame_size method is
  // implemented.
  return ctx->inputs[0] ? ctx->inputs[0]->min_samples : 0;
}

int32_t
FilterSink::get(MediaRaw* media)
{
  if (!media) {
    VS_THROW(HumbleInvalidArgument("no media passed in"));
  }
  // 'empty' media before filling.
  media->setComplete(false);
  // ok, let's get to work
  AVFilterContext* ctx = getFilterCtx();
  AVFrame* mFrame = media->getCtx();
  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    VS_THROW(HumbleBadAlloc());
  }

  int e = av_buffersink_get_frame(ctx, frame);
  if (e != AVERROR_EOF && e != AVERROR(EAGAIN)) {
    FfmpegException::check(e, "could not get frame from audio sink:");
    // now, copy this into our frame

    // release any memory we have
    av_frame_unref(mFrame);
    // and copy any data in.
    av_frame_ref(mFrame, frame);

    // if we get here, we're complete
    media->setComplete(true);
  }
  if (e == AVERROR(EAGAIN))
    // set to 0 as the media incomplete status signals to caller they should retry.
    e = 0;

  // and free the frame we made
  av_frame_unref(frame);
  av_frame_free(&frame);
  return e;
}
} /* namespace video */
} /* namespace humble */
} /* namespace io */
