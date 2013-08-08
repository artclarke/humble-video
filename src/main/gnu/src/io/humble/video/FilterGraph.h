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
class FilterAudioSource;
class FilterPictureSource;
class FilterSink;
class FilterAudioSink;
class FilterPictureSink;

class FilterGraph : public io::humble::video::Configurable
{
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
   * Creates a new {@link FilterGraph}.
   */
  static FilterGraph* make();

  /**
   * Add a filter with the given name to the graph.
   * @return An object that refers to the new filter.
   */
  virtual Filter* addFilter(FilterType* type, const char* name);

  /**
   * @return the filter with the given name, or null if not found.
   */
  virtual Filter* getFilter(const char* name);

  /**
   * Add a {@link FilterAudioSource}.
   * @param name the name; must be unique in graph
   * @param timeBase timebase of frames that will be input. If null 1/sampleRate is assumed.
   * @param sampleRate the audio sample rate
   * @param channelLaout the channel layout
   * @param format the sample format
   *
   * @return The FilterSource that was added.
   * @throws RuntimeException if name is already in graph.
   * @throws InvalidArgument if any argument is invalid.
   */
  virtual FilterAudioSource* addAudioSource(const char* name,
      int32_t sampleRate,
      AudioChannel::Layout channelLayout,
      AudioFormat::Type format,
      Rational* timeBase

  );

  /**
   * Add a {@link FilterPictureSource}.
   * @param name the name; must be unique in graph
   * @param width the width in pixels of {@link MediaPicture} objects that will be added to this source.
   * @param height the height in pixels  of {@link MediaPicture} objects that will be added to this source.
   * @param format the pixel format
   * @param timeBase timebase of frames that will be input. If null, 1/{@link Global.DEFAULT_PTS_PER_SECOND} is assumed.
   * @param pixelAspectRatio pixel aspect ratio. If null, 1/1 is assumed.
   *
   * @return The FilterSource that was added.
   * @throws RuntimeException if name is already in graph.
   * @throws InvalidArgument if any argument is invalid.
   */
  virtual FilterPictureSource* addPictureSource(const char* name,
      int32_t width,
      int32_t height,
      PixelFormat::Type format,
      Rational* timeBase,
      Rational* pixelAspectRatio);

  /**
   * Add a {@link FilterAudioSink}.
   * @param name the name; must be unique in graph
   * @param sampleRate the audio sample rate
   * @param channelLaout the channel layout
   * @param format the sample format
   *
   * @return The FilterAudioSink that was added.
   * @throws RuntimeException if name is already in graph.
   * @throws InvalidArgument if any argument is invalid.
   */
  virtual FilterAudioSink* addAudioSink(const char* name,
       int32_t sampleRate,
       AudioChannel::Layout channelLayout,
       AudioFormat::Type format
   );

  /**
   * Add a {@link FilterPictureSink}.
   * @param name the name; must be unique in graph
   * @param format the pixel format desired of pictures taken from this sink.
   * @return The FilterPictureSink that was added.
   * @throws RuntimeException if name is already in graph.
   * @throws InvalidArgument if any argument is invalid.
   */
   virtual FilterPictureSink* addPictureSink(const char* name,
       PixelFormat::Type format);

protected:
   virtual void addSource(AVFilterContext* source);
   virtual void addSink(AVFilterContext* sink);
public:

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
   * @throws RuntimeException if <b>any inputs or outputs</b> are open (i.e. each filter
   *   in the graph must either point to another filter on all inputs or outputs, or point to
   *   a {@link FilterSink} or {@link FilterSource} when done).
   */
  virtual void open(const char* filterDescription);

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
  %newobject getDisplayString();
  %typemap(newfree) char * "av_free($1);";
#endif
  virtual char* getDisplayString();

  virtual State getState() { return mState; }


protected:
  virtual void* getCtx() { return mCtx; }
  FilterGraph();
  virtual
  ~FilterGraph();

private:
  static void fillAVFilterInOut(std::vector<AVFilterContext*>& list, AVFilterInOut** inOut);
  virtual Filter* getFilter(AVFilterContext*);
  AVFilterGraph* mCtx;
  State mState;
  std::vector<AVFilterContext*> mSources;
  std::vector<AVFilterContext*> mSinks;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* HUMBLEFILTERGRAPH_H_ */
