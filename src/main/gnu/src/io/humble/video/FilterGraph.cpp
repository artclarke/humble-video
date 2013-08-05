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
#include <io/humble/video/FilterAudioSource.h>
#include <io/humble/video/FilterPictureSource.h>
#include <io/humble/video/FilterAudioSink.h>
#include <io/humble/video/FilterPictureSink.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

FilterGraph::FilterGraph() {
  mCtx = avfilter_graph_alloc();
  if (!mCtx) {
    VS_THROW(HumbleBadAlloc());
  }
  mState = STATE_INITED;
}

FilterAudioSource*
FilterGraph::addAudioSource(const char* name,
    Rational* aTimeBase, int32_t sampleRate,
    AudioChannel::Layout channelLayout, AudioFormat::Type format) {
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("no name specified"));
  }
  if (sampleRate <= 0) {
    VS_THROW(HumbleInvalidArgument("no sample rate specified"));
  }
  if (channelLayout == AudioChannel::CH_LAYOUT_UNKNOWN) {
    VS_THROW(HumbleInvalidArgument("no channel layout specified"));
  }
  if (format == AudioFormat::SAMPLE_FMT_NONE) {
    VS_THROW(HumbleInvalidArgument("no sample format specified"));
  }
  // hold in ref pointer to avoid leak
  RefPointer<Rational> timeBase = aTimeBase;
  if (!timeBase)
    timeBase = Rational::make(1, sampleRate);

  // get a buffer source
  AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
  AVFilterContext* ctx = 0;
  char args[512] = "";

  snprintf(args, sizeof(args),
          "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
          timeBase->getNumerator(),
          timeBase->getDenominator(),
          sampleRate,
          AudioFormat::getName(format),
          (int64_t)channelLayout);

  int e = avfilter_graph_create_filter(&ctx, abuffersrc, name,
                                     args, NULL, mCtx);
  FfmpegException::check(e, "could not add FilterAudioSource ");

  RefPointer<FilterAudioSource> s = FilterAudioSource::make(this, ctx);
  // now, add it to the graph sources
  this->addSource(s.value(), name);
  return s.get();
}

FilterPictureSource*
FilterGraph::addPictureSource(const char* name, int32_t width, int32_t height,
    PixelFormat::Type format) {
  (void) name;
  (void) width;
  (void) height;
  (void) format;
  return 0;
}

FilterAudioSink*
FilterGraph::addAudioSink(const char* name, int32_t sampleRate,
    AudioChannel::Layout channelLayout, AudioFormat::Type format) {
  (void) sampleRate;
  (void) channelLayout;
  (void) format;
  AVFilterContext* ctx = 0;
  RefPointer<FilterAudioSink> s = FilterAudioSink::make(this, ctx);
  // now, add it to the graph sources
  this->addSink(s.value(), name);
  return s.get();
}

FilterPictureSink*
FilterGraph::addPictureSink(const char* name, int32_t width, int32_t height,
    PixelFormat::Type format) {
  (void) name;
  (void) width;
  (void) height;
  (void) format;
  return 0;
}

FilterGraph::~FilterGraph() {
  avfilter_graph_free(&mCtx);
}

FilterGraph*
FilterGraph::make()
{
  Global::init();
  RefPointer<FilterGraph> r;
  r.reset(new FilterGraph(), true);
  return r.get();
}

void
FilterGraph::addSource(FilterSource* aSource, const char* name) {
  RefPointer<Configurable> source;
  source.reset((Configurable*) aSource, true);

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
  if (i < 0 || (size_t) i >= mSourceNames.size())
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
  sink.reset((Configurable*) aSink, true);

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
  if (i < 0 || (size_t) i >= mSinkNames.size())
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
FilterGraph::loadGraph(const char* f) {
  if (mState != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("Attempt to set property on opened graph"));
  }
  if (!f || !*f) {
    VS_THROW(HumbleInvalidArgument("name must not be empty"));
  }
  AVFilterInOut* inputs = 0;
  AVFilterInOut* outputs = 0;
  char* filterDebugString = 0;

  try {

    // let's iterate through all our inputs, then outputs
    std::map<std::string, RefPointer<Configurable> >::iterator iter;

    for(iter = mSources.begin(); iter != mSources.end(); ++iter) {
      AVFilterInOut* io = avfilter_inout_alloc();
      if (!io)
        VS_THROW(HumbleBadAlloc());
      // do not force a ref count increment since the iter will remain throughout
      FilterSource* source = (FilterSource*)((*iter).second.value());
      io->name = av_strdup(source->getName());
      io->filter_ctx = source->getFilterCtx();
      io->pad_idx = 0;
      io->next = inputs;
      inputs = io;
    }
    for(iter = mSinks.begin(); iter != mSinks.end(); ++iter) {
      AVFilterInOut* io = avfilter_inout_alloc();
      if (!io)
        VS_THROW(HumbleBadAlloc());
      // do not force a ref count increment since the iter will remain throughout
      FilterSink* sink = (FilterSink*)((*iter).second.value());
      io->name = av_strdup(sink->getName());
      io->filter_ctx = sink->getFilterCtx();
      io->pad_idx = 0;
      io->next = inputs;
      inputs = io;
    }
    // now, let's try parsing
    int e = avfilter_graph_parse_ptr(mCtx, f, &inputs, &outputs, 0);
    FfmpegException::check(e, "failure to parse FilterGraph description ");

    // now, check to make sure that the number of inputs matches the number of outputs
    // and if not, throw an exception.
    size_t numUnclosedInputs=0;
    for(const AVFilterInOut* i = inputs; i; i = i->next)
      ++numUnclosedInputs;
    size_t numUnclosedOutputs = 0;
    for(const AVFilterInOut* i = outputs; i; i = i->next)
      ++numUnclosedOutputs;
    if (numUnclosedInputs > mSources.size() || numUnclosedOutputs > mSinks.size()) {
      filterDebugString = getHumanReadableString();
      VS_THROW(HumbleRuntimeError::make("filterDescription had unclosed Sinks or Sources. Sinks: [expected=%d,actual=%d]; Sources: [expected=%d, actual=%d]; graphStr=%s;",
          mSinks.size(),
          numUnclosedInputs,
          mSources.size(),
          numUnclosedOutputs,
          filterDebugString));
    }
  } catch (std::exception & e) {
    // free any memory before rethrowing
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    av_freep(&filterDebugString);
  }

}
void
FilterGraph::addFilter(FilterType* type, const char* name) {
  if (mState != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("Attempt to set property on opened graph"));
  }
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("name must not be empty"));
  }
  AVFilterContext* fc = avfilter_graph_alloc_filter(mCtx, type->getCtx(), name);
  if (!fc)
  VS_THROW(
      HumbleRuntimeError::make("was not able to add filter named: %s", name));
}

Filter*
FilterGraph::getFilter(const char* name) {
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("name must not be empty"));
  }

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
