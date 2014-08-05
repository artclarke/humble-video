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
 * FilterLink.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/Logger.h>
#include <io/humble/video/VideoExceptions.h>
#include "Filter.h"
#include "FilterGraph.h"
#include "FilterLink.h"

VS_LOG_SETUP(VS_CPP_PACKAGE.FilterLink);

using namespace io::humble::ferry;
namespace io {
namespace humble {
namespace video {

FilterLink::FilterLink(FilterGraph* graph, AVFilterLink* link) {
  if (!graph)
    VS_THROW(HumbleInvalidArgument("no graph passed in"));
  if (!link)
    VS_THROW(HumbleInvalidArgument("no link passed in"));

  // acquire the graph because it owns the link
  mGraph.reset(graph, true);
  mCtx = link;
}

void
FilterLink::insertFilter(Filter* filter, int32_t srcPadIndex,
    int32_t dstPadIndex) {
  if (!filter)
    VS_THROW(HumbleInvalidArgument("no filter passed in"));
  if (srcPadIndex < 0 || srcPadIndex >= filter->getNumInputs())
    VS_THROW(HumbleInvalidArgument("invalid src pad index"));
  if (dstPadIndex >= filter->getNumOutputs())
    VS_THROW(HumbleInvalidArgument("invalid dst pad index"));

  avfilter_insert_filter(mCtx, filter->getFilterCtx(), srcPadIndex, dstPadIndex);
  // reset cached pointers
  mInputFilter = 0;
  mOutputFilter = 0;
}

FilterLink::~FilterLink() {
  // graph owns the link
  mCtx = 0;
}

FilterLink*
FilterLink::make(FilterGraph* graph, AVFilterLink* link)
{
  Global::init();
  RefPointer<FilterLink> retval;
  retval.reset(new FilterLink(graph, link), true);
  return retval.get();
}

FilterGraph*
FilterLink::getFilterGraph() { return mGraph.get(); }

Filter*
FilterLink::getInputFilter() {
  if (!mInputFilter || mInputFilter->getFilterCtx() != mCtx->src)
    mInputFilter = Filter::make(mGraph.value(), mCtx->src);
  return mInputFilter.get();
}

const char*
FilterLink::getInputPadName() {
  return avfilter_pad_get_name(mCtx->srcpad, 0);
}

MediaDescriptor::Type
FilterLink::getInputPadType() {
  return (MediaDescriptor::Type)avfilter_pad_get_type(mCtx->srcpad, 0);
}

Filter*
FilterLink::getOutputFilter() {
  if (!mOutputFilter || mOutputFilter->getFilterCtx() != mCtx->dst)
    mOutputFilter = Filter::make(mGraph.value(), mCtx->dst);
  return mOutputFilter.get();
}

const char*
FilterLink::getOutputPadName() {
  return avfilter_pad_get_name(mCtx->dstpad, 0);
}

MediaDescriptor::Type
FilterLink::getOutputPadType() {
  return (MediaDescriptor::Type)avfilter_pad_get_type(mCtx->dstpad, 0);
}

MediaDescriptor::Type
FilterLink::getMediaType() {
  return (MediaDescriptor::Type)mCtx->type;
}

Rational*
FilterLink::getPixelAspectRatio() {
  if (!mPixelAspectRatio ||
      mPixelAspectRatio->getNumerator() != mCtx->sample_aspect_ratio.num ||
      mPixelAspectRatio->getDenominator() != mCtx->sample_aspect_ratio.den)
    mPixelAspectRatio = Rational::make(mCtx->sample_aspect_ratio.num, mCtx->sample_aspect_ratio.den);
  return mPixelAspectRatio.get();
}

Rational*
FilterLink::getTimeBase() {
  if (!mTimeBase ||
      mTimeBase->getNumerator() != mCtx->time_base.num ||
      mTimeBase->getDenominator() != mCtx->time_base.den)
    mTimeBase = Rational::make(mCtx->time_base.num, mCtx->time_base.den);
  return mTimeBase.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
