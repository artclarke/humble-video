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
#include <io/humble/video/VideoExceptions.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

FilterGraph::FilterGraph() {
  mCtx = avfilter_graph_alloc();
  mState = STATE_INITED;
}

FilterGraph::~FilterGraph() {
  avfilter_graph_free(&mCtx);
}

void
FilterGraph::addSource(FilterSource* aSource, const char* name) {
  RefPointer<Configurable> source;
  source.reset((Configurable*)aSource, true);

  if (getState() != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("cannot add sources after opening graph"));
  }
  if (!source) {
    VS_THROW(HumbleInvalidArgument("no source specified"));
  }
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("no name specified"));
  }
  if (mSources.find(name) != mSources.end()) mSources.erase(name);
  else mSourceNames.push_back(name);
  mSources[name] = source;
}

int32_t
FilterGraph::getNumSources() {
  return mSources.size();
}

FilterSource*
FilterGraph::getSource(int32_t i) {
  if (i < 0 || (size_t)i >= mSourceNames.size())
  VS_THROW(HumbleInvalidArgument("index out of range"));

  std::string name = mSourceNames[i];
  return (FilterSource*) (mSources[name].get());
}

FilterSource*
FilterGraph::getSource(const char* name) {
  RefPointer<Configurable> r;
  try {
    r = mSources[name];
  } catch (std::out_of_range & e) {
    VS_THROW(PropertyNotFoundException(name));
  }
  return (FilterSource*) (r.get());
}

void
FilterGraph::addSink(FilterSink* aSink, const char*name) {
  RefPointer<Configurable> sink;
  sink.reset((Configurable*)aSink, true);

  if (getState() != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("cannot add sinks after opening graph"));
  }
  if (!sink) {
    VS_THROW(HumbleInvalidArgument("no sink specified"));
  }
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("no name specified"));
  }
  if (mSinks.find(name) != mSources.end()) mSinks.erase(name);
  else mSinkNames.push_back(name);
  mSinks[name] = sink;
}

int32_t
FilterGraph::getNumSinks() {
  return mSinks.size();
}

FilterSink*
FilterGraph::getSink(int32_t i) {
  if (i < 0 || (size_t)i >= mSinkNames.size())
  VS_THROW(HumbleInvalidArgument("index out of range"));

  std::string name = mSinkNames[i];
  return (FilterSink*) (mSinks[name].get());
}

FilterSink*
FilterGraph::getSink(const char*name) {
  RefPointer<Configurable> r;
  try {
    r = mSinks[name];
  } catch (std::out_of_range & e) {
    VS_THROW(PropertyNotFoundException(name));
  }
  return (FilterSink*) (r.get());
}

void
FilterGraph::addFilter(FilterType* type, const char* name) {
  if (mState != STATE_INITED)
  VS_THROW(HumbleRuntimeError("Attempt to set property on opened graph"));
  if (!name || !*name)
  VS_THROW(HumbleInvalidArgument("name must not be empty"));
  AVFilterContext* fc = avfilter_graph_alloc_filter(mCtx, type->getCtx(), name);
  if (!fc)
  VS_THROW(
      HumbleRuntimeError::make("was not able to add filter named: %s", name));
}

Filter*
FilterGraph::getFilter(const char* name) {
  if (!name || !*name)
  VS_THROW(HumbleInvalidArgument("name must not be empty"));

  AVFilterContext* fc = avfilter_graph_get_filter(mCtx, (char*) name);
  if (!fc)
  VS_THROW(PropertyNotFoundException(name));

//  return Filter::make(this, fc);
  return 0;
}

void
FilterGraph::setAutoConvert(AutoConvertFlag value) {
  if (mState != STATE_INITED)
  VS_THROW(HumbleRuntimeError("Attempt to set property on opened graph"));
  mCtx->disable_auto_convert = value;
}

FilterGraph::AutoConvertFlag
FilterGraph::getAutoConvert() {
  return (FilterGraph::AutoConvertFlag) mCtx->disable_auto_convert;
}

void
FilterGraph::open() {
  int e = avfilter_graph_config(mCtx, 0);
  if (e < 0) {
    mState = STATE_ERROR;
    FfmpegException::check(e, "Could not open filtergraph ");
  }
  mState = STATE_OPENED;
}

char*
FilterGraph::sendCommand(const char* target, const char* command,
    const char* arguments, int flags) {
  if (!target || !*target)
  VS_THROW(HumbleInvalidArgument("target must not be empty"));
  if (!command || !*command)
  VS_THROW(HumbleInvalidArgument("command must not be empty"));
  const int32_t responseLen = 2048;
  char response[responseLen];
  int e = avfilter_graph_send_command(mCtx, target, command, arguments,
      response, responseLen, flags);
  FfmpegException::check(e,
      "error sending command to target. target: %s; command: %s; arguments: %s; ",
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
  int e = avfilter_graph_queue_command(mCtx, target, command, arguments, flags,
      ts);
  FfmpegException::check(e,
      "error queuing command to target. target: %s; command: %s; arguments: %s; ",
      target, command, arguments);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
