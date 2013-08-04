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
 * FilterGraph.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#include "FilterGraph.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/VideoExceptions.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

FilterGraph::FilterGraph() {
  mCtx = avfilter_graph_alloc();
}

FilterGraph::~FilterGraph() {
  avfilter_graph_free(&mCtx);
}

void
FilterGraph::addFilter(FilterType* type, const char* name) {
  if (!name || !*name)
    VS_THROW(HumbleInvalidArgument("name must not be empty"));
  AVFilterContext* fc = avfilter_graph_alloc_filter(mCtx, type->getCtx(), name);
  if (!fc)
    VS_THROW(HumbleRuntimeError::make("was not able to add filter named: %s", name));
}

Filter*
FilterGraph::getFilter(const char* name) {
  if (!name || !*name)
    VS_THROW(HumbleInvalidArgument("name must not be empty"));

  AVFilterContext* fc = avfilter_graph_get_filter(mCtx, (char*)name);
  if (!fc)
    VS_THROW(PropertyNotFoundException(name));

//  return Filter::make(this, fc);
  return 0;
}

void
FilterGraph::setAutoConvert(AutoConvertFlag value) {
   mCtx->disable_auto_convert = value;
}

FilterGraph::AutoConvertFlag
FilterGraph::getAutoConvert() {
  return (FilterGraph::AutoConvertFlag) mCtx->disable_auto_convert;
}

void
FilterGraph::open() {
  int e = avfilter_graph_config(mCtx, 0);
  FfmpegException::check(e, "Could not open filtergraph");
}

char*
FilterGraph::sendCommand(const char* target, const char* command,
    const char* arguments, int flags) {
  if (!target || !*target)
    VS_THROW(HumbleInvalidArgument("target must not be empty"));
  if (!command || !*command)
    VS_THROW(HumbleInvalidArgument("command must not be empty"));
  if (!arguments || !*arguments)
    VS_THROW(HumbleInvalidArgument("arguments must not be empty"));
  const int32_t responseLen = 2048;
  char response[responseLen];
  int e = avfilter_graph_send_command(mCtx, target, command, arguments, response,
      responseLen, flags);
  FfmpegException::check(e, "error sending command to target. target: %s; command: %s; arguments: %s; ",
      target, command, arguments);
  // create a copy
  return av_strdup(response);
}
void
FilterGraph::queueCommand(const char* target, const char* command,
    const char* arguments, int flags, double ts) {
  if (!target || !*target)
    VS_THROW(HumbleInvalidArgument("target must not be empty"));
  if (!command || !*command)
    VS_THROW(HumbleInvalidArgument("command must not be empty"));
  if (!arguments || !*arguments)
    VS_THROW(HumbleInvalidArgument("arguments must not be empty"));
  int e = avfilter_graph_queue_command(mCtx, target, command, arguments, flags, ts);
  FfmpegException::check(e, "error queuing command to target. target: %s; command: %s; arguments: %s; ",
      target, command, arguments);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
