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
 * FilterPictureSource.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include "FilterPictureSource.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/FilterGraph.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.FilterPictureSource);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

FilterPictureSource::FilterPictureSource(FilterGraph* graph, AVFilterContext* ctx) :
    FilterSource(graph, ctx) {

}

FilterPictureSource::~FilterPictureSource() {
}

FilterPictureSource*
FilterPictureSource::make(FilterGraph* graph, AVFilterContext* ctx) {
  Global::init();
  RefPointer<FilterPictureSource> r;
  r.reset(new FilterPictureSource(graph, ctx), true);
  return r.get();
}

int32_t
FilterPictureSource::getPicture(MediaPicture* picture) {
  if (!picture) {
    VS_THROW(HumbleInvalidArgument("no picture passed in"));
  }
  AVFilterContext* ctx = getFilterCtx();

  // sadly, FFmpeg will not auto scale width and height at this
  // time, and so we need to check before we get a frame so we
  // don't destroy something we shouldn't.
  if (!ctx->inputs) {
    VS_THROW(HumbleRuntimeError("unexpect ffmpeg internal error"));
  }

  AVFilterLink* link = ctx->inputs[0];
  if (link) {
    if (link->w != picture->getWidth() || link->h != picture->getHeight()) {
      VS_THROW(HumbleInvalidArgument::make("picture dimensions do not match expected.  Got (%d x %d); Expected (%d x %d)",
          picture->getWidth(),
          picture->getHeight(),
          link->w,
          link->h
          ));
    }
  }
  return FilterSource::get(picture);
}
} /* namespace video */
} /* namespace humble */
} /* namespace io */
