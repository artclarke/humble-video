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
 * FilterType.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#include "FilterType.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/VideoExceptions.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.FilterType);

namespace io {
namespace humble {
namespace video {

FilterType*
FilterType::make(const AVFilter* f) {
  RefPointer<FilterType> retval;
  if (!f) throw HumbleInvalidArgument("no filter passed in");
  retval = make();
  retval->mCtx = f;

  return retval.get();
}
int32_t
FilterType::getNumFilterTypes() {
  Global::init();
  int i = 0;
  for (const AVFilter* f = 0; (f = avfilter_next(f)) != 0; ++i)
    ;
  return i;
}

FilterType*
FilterType::getFilterType(int32_t index) {
  Global::init();
  int i = 0;
  if (index < 0)
    VS_THROW(HumbleInvalidArgument("index must be >= 0"));

  for (const AVFilter* f = 0; (f = avfilter_next(f)) != 0; ++i)
    if (i == index) {
      FilterType * retval = FilterType::make(f);
      return retval;
    }
  // if we get here, index was too large.
  VS_THROW(HumbleInvalidArgument("index must be < getNumFilterTypes()"));
  // removes an Eclipse CDT warning, although this code is unreachable.
  return 0;
}

FilterType*
FilterType::findFilterType(const char* name) {
  if (!name || !*name)
    VS_THROW(HumbleInvalidArgument("name must be specified"));

  const AVFilter* f = avfilter_get_by_name(name);
  if (!f)
    VS_THROW(PropertyNotFoundException(name));
  return make(f);
}

int32_t
FilterType::getNumInputs() {
  return avfilter_pad_count(mCtx->inputs);
}

int32_t
FilterType::getNumOutputs() {
  return avfilter_pad_count(mCtx->outputs);
}

const char*
FilterType::getInputName(int32_t i) {
  if (i < 0 || i >= getNumInputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return avfilter_pad_get_name(mCtx->inputs, i);
}

const char*
FilterType::getOutputName(int32_t i) {
  if (i < 0 || i >= getNumOutputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return avfilter_pad_get_name(mCtx->outputs, i);
}

MediaDescriptor::Type
FilterType::getInputType(int32_t i) {
  if (i < 0 || i >= getNumInputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return (MediaDescriptor::Type) avfilter_pad_get_type(mCtx->inputs, i);
}

MediaDescriptor::Type
FilterType::getOutputType(int32_t i) {
  if (i < 0 || i >= getNumOutputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return (MediaDescriptor::Type) avfilter_pad_get_type(mCtx->outputs, i);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
