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
 * FilterGraph.h
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#ifndef HUMBLEFILTERGRAPH_H_
#define HUMBLEFILTERGRAPH_H_

#include <map>
#include <vector>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/FilterType.h>

namespace io {
namespace humble {
namespace video {

class Filter;
class FilterSource;
class FilterSink;

class FilterGraph : public io::humble::video::Configurable
{
  VS_JNIUTILS_REFCOUNTED_OBJECT(FilterGraph);
public:
  /**
   * States a graph can be in.
   */
  typedef enum State {
    /** Initialized but not yet opened. Properties and graph strings may still be set. */
    STATE_INITED,
    /** Opened. Properites and graphs can no longer be set, but {@link MediaRaw} objects
     * can be processed.
     */
    STATE_OPENED,
    /**
     * An error occurred and this graph should be discarded.
     */
    STATE_ERROR
  } State;

  typedef enum AutoConvertFlag {
    /** all automatic conversions enabled */
     AUTO_CONVERT_ALL=AVFILTER_AUTO_CONVERT_ALL,
     /** all automatic conversions disabled */
     AUTO_CONVERT_NONE=AVFILTER_AUTO_CONVERT_NONE,
  } AutoConvertFlag;

  /**
   * Add a filter with the given name to the graph.
   */
  virtual void addFilter(FilterType* type, const char* name);

  /**
   * @return the filter with the given name, or null if not found.
   */
  virtual Filter* getFilter(const char* name);

  /**
   * Add a {@link FilterSource}.
   * @param source the source
   * @param name the name; must be unique in graph
   *
   * @throws RuntimeException if name is already in graph.
   */
  virtual void addSource(FilterSource* source, const char* name);

  /**
   * @return number of {@link FilterSource} added so far.
   */
  virtual int32_t getNumSources();

  /**
   * @param index The n'th of {@link #getNumSoruces()} {@link FilterSource}s attached to this {@link FilterGraph}.
   * @return the {@link FilterSource}
   * @throws InvalidArgument if index < 0 || index >= {@link #getNumSources()}
   */
  virtual FilterSource* getSource(int32_t index);

  /**
   * @param name unique name of a {@link FilterSource} in this {@link FilterGraph}. Should have been added with {@link #addSource(FilterSource,String)}.
   * @throws PropertyNotFoundException if not in graph.
   */
  virtual FilterSource* getSource(const char* name);

  /**
   * Add a {@link FilterSink}.
   * @param sink the source
   * @param name the name; must be unique in graph
   *
   * @throws RuntimeException if name is already in graph.
   */
  virtual void addSink(FilterSink* sink, const char* name);

  /**
   * @return number of {@link FilterSink} added so far.
   */
  virtual int32_t getNumSinks();

  /**
   * @param index The n'th of {@link #getNumSoruces()} {@link FilterSink}s attached to this {@link FilterGraph}.
   * @return the {@link FilterSink}
   * @throws InvalidArgument if index < 0 || index >= {@link #getNumSinks()}
   */
  virtual FilterSink* getSink(int32_t index);

  /**
   * @param name unique name of a {@link FilterSink} in this {@link FilterGraph}. Should have been added with {@link #addSink(FilterSink,String)}.
   * @throws PropertyNotFoundException if not in graph.
   */
  virtual FilterSink* getSink(const char* name);

  /**
   * Should this graph auto-convert audio or pictures into the formats
   * different filters require (rather than require the user to construct
   * a graph with all filters sets correctly).
   *
   * @param value whether to auto-convert with {@link MediaPictureResampler} or {@link MediaAudioResampler} objects.
   */
  virtual void setAutoConvert(AutoConvertFlag value);

  /**
   * @return does this graph auto convert {@link MediaPicture} and {@link MediaRaw} objects to different
   * dimensions/sample-rates/channels/etc. when
   * pulling them through the graph.
   */
  virtual AutoConvertFlag getAutoConvert();

  /**
   * Add a graph described by a string to a graph. For any Sinks or Sources
   * the caller must have called {@link #addSource} or {@link #addSink} before
   * this call.
   *
   * @param filterDescription The filter string to be parsed, in FFmpeg libavfilter format.
   */
  virtual void loadGraph(const char* filterDescription);

  /**
   * Call after all filters have been added and you are ready to begin pushing data through
   * the graph. Any calls to set properties after this call <i>may</i> be ignored.
   *
   * @throws RuntimeException if <b>any inputs or outputs</b> are open (i.e. each filter
   *   in the graph must either point to another filter on all inputs or outputs, or point to
   *   a {@link FilterSink} or {@link FilterSource} when done).
   */
  virtual void open();

  /**
   * Send a command to one or more filter instances.
   *
   * @param target the filter(s) to which the command should be sent
   *               "all" sends to all filters
   *               otherwise it can be a filter or filter instance name
   *               which will send the command to all matching filters.
   * @param command the command to send, for handling simplicity all commands must be alphanumeric only
   * @param arguments the argument for the command
   * @returns a response form the command.
   * @throws RuntimeException on error or {@link PropertyNotFoundException} for unsupported
   *   commands.
   */
#ifdef SWIG
  %newobject sendCommand(const char*, const char*, const char*, int);
  %typemap(newfree) char * "av_free($1);";
#endif

  virtual char* sendCommand(
      const char* target,
      const char* command,
      const char* arguments,
      int flags);

  /**
   * Queue a command for one or more filter instances.
   *
   * @param target the filter(s) to which the command should be sent
   *               "all" sends to all filters
   *               otherwise it can be a filter or filter instance name
   *               which will send the command to all matching filters.
   * @param command    the command to sent, for handling simplicity all commands must be alphanummeric only
   * @param arguments    the argument for the command
   * @param ts     time at which the command should be sent to the filter
   *
   * @note As this executes commands after this function returns, no return code
   *       from the filter is provided, also AVFILTER_CMD_FLAG_ONE is not supported.
   */
  virtual void queueCommand(const char *target,
      const char *command,
      const char *arguments, int flags, double ts);

#ifdef SWIG
  %newobject getHumanReadableString();
  %typemap(newfree) char * "av_free($1);";
#endif
  virtual char* getHumanReadableString() {
    return avfilter_graph_dump(mCtx, "");
  }

  virtual State getState() { return mState; }


protected:
  virtual void* getCtx() { return mCtx; }
  FilterGraph();
  virtual
  ~FilterGraph();

private:
  AVFilterGraph* mCtx;
  State mState;
  std::map<std::string, io::humble::ferry::RefPointer<Configurable> > mSources;
  std::vector<std::string> mSourceNames;
  std::map<std::string, io::humble::ferry::RefPointer<Configurable> > mSinks;
  std::vector<std::string> mSinkNames;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* HUMBLEFILTERGRAPH_H_ */
