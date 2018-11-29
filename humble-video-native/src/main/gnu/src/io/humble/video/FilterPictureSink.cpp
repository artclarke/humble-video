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
 * FilterPictureSink.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include "FilterPictureSink.h"
#include <io/humble/ferry/RefPointer.h>

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

FilterPictureSink::FilterPictureSink(FilterGraph* graph,
    AVFilterContext* ctx) :
    FilterSink(graph, ctx) {

}

FilterPictureSink::~FilterPictureSink() {
}

FilterPictureSink*
FilterPictureSink::make(FilterGraph* graph, AVFilterContext* ctx) {
  Global::init();
  RefPointer<FilterPictureSink> r;
  r.reset(new FilterPictureSink(graph, ctx), true);
  return r.get();
}

ProcessorResult
FilterPictureSink::sendPicture(MediaPicture* picture) {
  return FilterSink::sendRaw(picture);
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
