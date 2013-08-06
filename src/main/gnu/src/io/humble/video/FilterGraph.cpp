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

#define VS_FILTER_AUDIO_SINK "abuffersink"
#define VS_FILTER_AUDIO_SOURCE "abuffer"
#define VS_FILTER_VIDEO_SINK "buffersink"
#define VS_FILTER_VIDEO_SOURCE "buffer"

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
FilterGraph::~FilterGraph() {
  avfilter_graph_free(&mCtx);
}

FilterAudioSource*
FilterGraph::addAudioSource(const char* name, int32_t sampleRate,
    AudioChannel::Layout channelLayout, AudioFormat::Type format,
    Rational* aTimeBase) {
  if (getState() != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("cannot add sources after opening graph"));
  }
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
  RefPointer<Rational> timeBase;
  timeBase.reset(aTimeBase, true);
  if (!timeBase) timeBase = Rational::make(1, sampleRate);

  // get a buffer source
  AVFilter *abuffersrc = avfilter_get_by_name(VS_FILTER_AUDIO_SOURCE);
  if (!abuffersrc)
    VS_THROW(
        HumbleRuntimeError::make(
            "could not find audio buffer source; bad FFmpeg build?"));
  AVFilterContext* ctx = 0;
  char args[512] = "";

  snprintf(args, sizeof(args),
      "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
      timeBase->getNumerator(), timeBase->getDenominator(), sampleRate,
      AudioFormat::getName(format), (int64_t) channelLayout);

  int e = avfilter_graph_create_filter(&ctx, abuffersrc, name, args, 0, mCtx);
  FfmpegException::check(e, "could not add FilterAudioSource ");

  // now, add it to the graph sources
  this->addSource(ctx);

  // and return a made object (note: we must not ref-count this ourselves)
  return dynamic_cast<FilterAudioSource*>(getFilter(ctx));
}

FilterPictureSource*
FilterGraph::addPictureSource(const char* name, int32_t width, int32_t height,
    PixelFormat::Type format, Rational* aTimeBase,
    Rational* aPixelAspectRatio) {
  if (getState() != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("cannot add sources after opening graph"));
  }

  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("no name specified"));
  }
  if (width <= 0) {
    VS_THROW(HumbleInvalidArgument("no width specified"));
  }
  if (height <= 0) {
    VS_THROW(HumbleInvalidArgument("no height specified"));
  }
  if (format == PixelFormat::PIX_FMT_NONE) {
    VS_THROW(HumbleInvalidArgument("no sample format specified"));
  }
  // hold in ref pointer to avoid leak
  RefPointer<Rational> timeBase;
  timeBase.reset(aTimeBase, true);
  if (!timeBase) timeBase = Rational::make(1, Global::DEFAULT_PTS_PER_SECOND);

  RefPointer<Rational> aspectRatio;
  aspectRatio.reset(aPixelAspectRatio, true);
  if (!aspectRatio) aspectRatio = Rational::make(1, 1);

  AVFilter *buffersrc = avfilter_get_by_name(VS_FILTER_VIDEO_SOURCE);
  if (!buffersrc)
    VS_THROW(
        HumbleRuntimeError::make(
            "could not find video buffer source; bad FFmpeg build?"));
  AVFilterContext* ctx = 0;

  char args[512];
  snprintf(args, sizeof(args),
      "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d", width,
      height, format, timeBase->getNumerator(), timeBase->getDenominator(),
      aspectRatio->getNumerator(), aspectRatio->getDenominator());

  int e = avfilter_graph_create_filter(&ctx, buffersrc, name, args, 0, mCtx);
  FfmpegException::check(e, "could not add FilterPictureSource ");

  // now, add it to the graph sources
  this->addSource(ctx);

  return dynamic_cast<FilterPictureSource*>(getFilter(ctx));
}

FilterAudioSink*
FilterGraph::addAudioSink(const char* name, int32_t sampleRate,
    AudioChannel::Layout channelLayout, AudioFormat::Type format) {
  if (getState() != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("cannot add sinks after opening graph"));
  }

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

  AVFilter *abuffersink = avfilter_get_by_name(VS_FILTER_AUDIO_SINK);
  if (!abuffersink)
    VS_THROW(
        HumbleRuntimeError::make(
            "could not find audio buffer sink; bad FFmpeg build?"));
  const int sampleRates[] =
      { sampleRate, -1 };
  const int64_t channels[] =
      { channelLayout, -1 };
  const enum AVSampleFormat sampleFormats[] =
      { (enum AVSampleFormat) format, (enum AVSampleFormat) -1 };

  AVFilterContext* ctx = 0;
  int e = avfilter_graph_create_filter(&ctx, abuffersink, name, 0, 0, mCtx);
  FfmpegException::check(e, "could not add FilterAudioSink ");

  try {
    e =
        av_opt_set_int_list(ctx, "sample_fmts", sampleFormats, -1, AV_OPT_SEARCH_CHILDREN);
    FfmpegException::check(e, "could not set audio formats ");

    e =
        av_opt_set_int_list(ctx, "channel_layouts", channels, -1, AV_OPT_SEARCH_CHILDREN);
    FfmpegException::check(e, "could not set audio channel layouts ");

    e =
        av_opt_set_int_list(ctx, "sample_rates", sampleRates, -1, AV_OPT_SEARCH_CHILDREN);
    FfmpegException::check(e, "could not set audio sample rates ");
  } catch (std::exception & e0) {
    // if we throw an exception, free the context so we don't leak.
    avfilter_free(ctx);
    ctx = 0;
    throw e0;
  }

  // now, add it to the graph sources
  this->addSink(ctx);

  return dynamic_cast<FilterAudioSink*>(getFilter(ctx));
}

FilterPictureSink*
FilterGraph::addPictureSink(const char* name, int32_t width, int32_t height,
    PixelFormat::Type format) {
  if (getState() != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("cannot add sinks after opening graph"));
  }
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("no name specified"));
  }
  if (width <= 0) {
    VS_THROW(HumbleInvalidArgument("no width specified"));
  }
  if (height <= 0) {
    VS_THROW(HumbleInvalidArgument("no height specified"));
  }
  if (format == PixelFormat::PIX_FMT_NONE) {
    VS_THROW(HumbleInvalidArgument("no sample format specified"));
  }
  AVFilter *buffersink = avfilter_get_by_name(VS_FILTER_VIDEO_SINK);
  if (!buffersink) {
    VS_THROW(
        HumbleRuntimeError::make(
            "could not find video buffer sink; bad FFmpeg build?"));
  }
  enum AVPixelFormat formats[] = { (enum AVPixelFormat)format, AV_PIX_FMT_NONE };
  AVFilterContext* ctx = 0;

  int e = avfilter_graph_create_filter(&ctx, buffersink, name, 0, 0, mCtx);
  FfmpegException::check(e, "could not add FilterPictureSink ");
  try {
    e =
        av_opt_set_int_list(ctx, "pix_fmts", formats, -1, AV_OPT_SEARCH_CHILDREN);
    FfmpegException::check(e, "could not set pixel formats ");
  } catch (std::exception & e0) {
    // if we throw an exception, free the context so we don't leak.
    avfilter_free(ctx);
    ctx = 0;
    throw e0;
  }
  this->addSink(ctx);
  return dynamic_cast<FilterPictureSink*>(getFilter(ctx));
}

FilterGraph*
FilterGraph::make() {
  Global::init();
  RefPointer<FilterGraph> r;
  r.reset(new FilterGraph(), true);
  return r.get();
}

void
FilterGraph::addSource(AVFilterContext* source) {
  if (!source) {
    VS_THROW(HumbleInvalidArgument("no source specified"));
  }
  mSources.push_back(source);
}

int32_t
FilterGraph::getNumSources() {
  return mSources.size();
}

FilterSource*
FilterGraph::getSource(int32_t i) {
  if (i < 0 || (size_t) i >= mSources.size())
    VS_THROW(HumbleInvalidArgument("index out of range"));

  return dynamic_cast<FilterSource*>(getFilter(mSources[i]));
}

FilterSource*
FilterGraph::getSource(const char* name) {
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("missing name"));
  }
  std::vector<AVFilterContext*> contexts = mSources;
  int n = contexts.size();
  for(int i = 0; i < n; i++) {
    AVFilterContext* sourceCtx = contexts[i];
    if (sourceCtx && sourceCtx->name && *sourceCtx->name && strcmp(sourceCtx->name, name)==0)
      return getSource(i);
  }
  // if we get here we did not find the filter.
  VS_THROW(PropertyNotFoundException(name));
  return 0;
}

void
FilterGraph::addSink(AVFilterContext* sink) {
  if (!sink) {
    VS_THROW(HumbleInvalidArgument("no sink specified"));
  }
  mSinks.push_back(sink);
}

int32_t
FilterGraph::getNumSinks() {
  return mSinks.size();
}

FilterSink*
FilterGraph::getSink(int32_t i) {
  if (i < 0 || (size_t) i >= mSinks.size()) {
    VS_THROW(HumbleInvalidArgument("index out of range"));
  }
  return dynamic_cast<FilterSink*>(getFilter(mSinks[i]));
}

FilterSink*
FilterGraph::getSink(const char*name) {
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("no name specified"));
  }
  std::vector<AVFilterContext*> contexts = mSinks;
  int n = contexts.size();
  for(int i = 0; i < n; i++) {
    AVFilterContext* sourceCtx = contexts[i];
    if (sourceCtx && sourceCtx->name && *sourceCtx->name && strcmp(sourceCtx->name, name)==0)
      return getSink(i);
  }
  // if we get here we did not find the filter.
  VS_THROW(PropertyNotFoundException(name));
  return 0;
}

void
FilterGraph::open(const char* f) {
  if (mState != STATE_INITED) {
    VS_THROW(HumbleRuntimeError("Attempt to set property on opened graph"));
  }
  if (!f || !*f) {
    VS_THROW(HumbleInvalidArgument("name must not be empty"));
  }
  AVFilterInOut* inputs = 0;
  AVFilterInOut* outputs = 0;

  try {

    // let's iterate through all our inputs, then outputs
    fillAVFilterInOut(mSources, &inputs);
    fillAVFilterInOut(mSinks, &outputs);

    // now, let's try parsing
    int e = avfilter_graph_parse_ptr(mCtx, f, &inputs, &outputs, 0);
    FfmpegException::check(e, "failure to parse FilterGraph description ");

    e = avfilter_graph_config(mCtx, 0);
    FfmpegException::check(e, "failure to configure FilterGraph ");

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    mState = STATE_OPENED;
  } catch (std::exception & e) {
    // free any memory before rethrowing
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    mState = STATE_ERROR;
    throw e;
  }

}

void
FilterGraph::fillAVFilterInOut(std::vector<AVFilterContext*>& list, AVFilterInOut** inOut) {
  int32_t n = list.size();
  for(int i = 0; i < n; i++) {
    AVFilterInOut* io = avfilter_inout_alloc();
    if (!io) {
      VS_THROW(HumbleBadAlloc());
    }
    AVFilterContext* source = list[i];
    io->name = av_strdup(source->name);
    io->filter_ctx = source;
    io->pad_idx = 0;
    io->next = *inOut;
    *inOut = io;
  }
}
Filter*
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
  return getFilter(name);
}

Filter*
FilterGraph::getFilter(const char* name) {
  if (!name || !*name) {
    VS_THROW(HumbleInvalidArgument("name must not be empty"));
  }

  AVFilterContext* fc = avfilter_graph_get_filter(mCtx, (char*) name);
  if (!fc) {
    VS_THROW(PropertyNotFoundException(name));
  }

  return getFilter(fc);
}
Filter*
FilterGraph::getFilter(AVFilterContext* ctx)
{
  // get the type
  const AVFilter* filter = ctx->filter;
  const char* filter_name = filter->name;
  if (strcmp(filter_name, VS_FILTER_AUDIO_SINK)==0) {
    return FilterAudioSink::make(this, ctx);
  }
  else if (strcmp(filter_name, VS_FILTER_AUDIO_SOURCE)==0) {
    return FilterAudioSource::make(this, ctx);
  }
  else if (strcmp(filter_name, VS_FILTER_VIDEO_SINK)==0) {
    return FilterPictureSink::make(this, ctx);
  }
  else if (strcmp(filter_name, VS_FILTER_VIDEO_SOURCE)==0) {
    return FilterPictureSource::make(this, ctx);
  }
  else
    return Filter::make(this, ctx);
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

char*
FilterGraph::getDisplayString() {
  if (getState() != STATE_OPENED) {
    VS_THROW(HumbleRuntimeError("can only get displayString on opened graphs"));
  }
  return avfilter_graph_dump(mCtx, "");
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
