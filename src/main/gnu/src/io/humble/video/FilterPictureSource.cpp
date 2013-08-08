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
 * FilterPictureSource.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include "FilterPictureSource.h"
#include <io/humble/ferry/RefPointer.h>

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

FilterPictureSource::FilterPictureSource(FilterGraph* graph,
    AVFilterContext* ctx) :
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

void
FilterPictureSource::addPicture(MediaPicture* picture) {
  FilterSource::add(picture);
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
