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
 * FilterAudioSource.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include "FilterAudioSource.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/MediaAudioImpl.h>

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

FilterAudioSource::FilterAudioSource(FilterGraph* graph, AVFilterContext* ctx) :
    FilterSource(graph, ctx) {
}

void
FilterAudioSource::addAudio(MediaAudio* audio) {
  FilterSource::add(audio);
}

FilterAudioSource*
FilterAudioSource::make(FilterGraph* graph,
    AVFilterContext* ctx) {
  Global::init();
  RefPointer<FilterAudioSource> r;
  r.reset(new FilterAudioSource(graph, ctx), true);
  return r.get();
}

FilterAudioSource::~FilterAudioSource() {
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
