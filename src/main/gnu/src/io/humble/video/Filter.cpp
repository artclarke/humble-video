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
 * Filter.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include "Filter.h"
#include "FilterLink.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Filter::Filter(FilterGraph* graph, AVFilterContext* ctx) {
  if (!graph)
    VS_THROW(HumbleInvalidArgument("no graph passed in"));
  if (!ctx)
    VS_THROW(HumbleInvalidArgument("no ctx passed in"));

  // acquire the graph because it owns the ctx
  mGraph.reset(graph, true);
  mCtx = ctx;
}

Filter*
Filter::make(FilterGraph* graph, AVFilterContext* mCtx) {
  Global::init();
  RefPointer<Filter> r;
  r.reset(new Filter(graph, mCtx), true);
  return r.get();
}

Filter::~Filter() {
  // nothing to do since graph owns ctx
  mCtx = 0;
}

int32_t
Filter::getNumInputs() {
  return mCtx->nb_inputs;
}

int32_t
Filter::getNumOutputs() {
  return mCtx->nb_outputs;
}

const char*
Filter::getInputName(int32_t i) {
  if (i < 0 || i >= getNumInputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return avfilter_pad_get_name(mCtx->input_pads, i);
}

const char*
Filter::getOutputName(int32_t i) {
  if (i < 0 || i >= getNumOutputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return avfilter_pad_get_name(mCtx->output_pads, i);
}

MediaDescriptor::Type
Filter::getInputType(int32_t i) {
  if (i < 0 || i >= getNumInputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return (MediaDescriptor::Type) avfilter_pad_get_type(mCtx->input_pads, i);
}

MediaDescriptor::Type
Filter::getOutputType(int32_t i) {
  if (i < 0 || i >= getNumOutputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return (MediaDescriptor::Type) avfilter_pad_get_type(mCtx->output_pads, i);
}
FilterLink*
Filter::getInputLink(int32_t i) {
  if (i < 0 || i >= getNumInputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return FilterLink::make(mGraph.value(), mCtx->inputs[i]);
}

FilterLink*
Filter::getOutputLink(int32_t i) {
  if (i < 0 || i >= getNumOutputs())
    VS_THROW(HumbleInvalidArgument("index out of range"));
  return FilterLink::make(mGraph.value(), mCtx->outputs[i]);
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
