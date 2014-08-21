/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
/*
 * Filter.h
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/FilterType.h>
#include <io/humble/video/FilterGraph.h>

namespace io {
namespace humble {
namespace video {

class FilterLink;

/**
 * A Filter that can convert one MediaRaw object into zero or more other
 * MediaRaw objects.
 */
class VS_API_HUMBLEVIDEO Filter : public io::humble::video::Configurable
{
public:
  /**
   * Flags that can be passed when processing commands.
   */
  typedef enum CommandFlag
  {
    /**
     * Stop once a filter understood the command (for target=all for example), fast filters are favored automatically
     */
    COMMAND_FLAG_ONE = AVFILTER_CMD_FLAG_ONE,
    /**
     * Only execute command when its fast (like a video out that supports contrast adjustment in hw)
     */
    COMMAND_FLAG_FAST = AVFILTER_CMD_FLAG_FAST,

  } CommandFlag;
  /**
   *  @return name of filter.
   */
  virtual const char*
  getName() {
    return mCtx->name;
  }

  /**
   * @return number of inputs this FilterType expects.
   */
  virtual int32_t
  getNumInputs();

  /**
   * @param index which input to get name of
   * @return the input name
   * @throws InvalidArgument if index < 0 || index > #getNumInputs().
   */
  virtual const char*
  getInputName(int32_t index);

  /**
   * @param index which input to get type of
   * @return the input media type
   * @throws InvalidArgument if index < 0 || index > #getNumInputs().
   */
  virtual MediaDescriptor::Type
  getInputType(int32_t index);

  /**
   * @param index which input to get link of
   * @return the FilterLink that is inputting into this filter at the given position.
   * @throws InvalidArgument if index < 0 || index > #getNumInputs().
   */
  virtual FilterLink*
  getInputLink(int32_t index);

  /**
   * @return number of outputs this FilterType expects.
   */
  virtual int32_t
  getNumOutputs();

  /**
   * @param index which output to get name of
   * @return the output name
   * @throws InvalidArgument if index < 0 || index > #getNumOutputs().
   */
  virtual const char*
  getOutputName(int32_t index);

  /**
   * @param index which output to get type of
   * @return the output media type
   * @throws InvalidArgument if index < 0 || index > #getNumOutputs().
   */
  virtual MediaDescriptor::Type
  getOutputType(int32_t index);

  /**
   * @param index which output to get link of
   * @return the FilterLink that is outputting from this filter at the given position.
   * @throws InvalidArgument if index < 0 || index > #getNumInputs().
   */
  virtual FilterLink*
  getOutputLink(int32_t index);

  /**
   * @return the graph this filter is part of.
   */
  virtual FilterGraph*
  getGraph() { return mGraph.get(); }

#ifndef SWIG
  static Filter*
  make(FilterGraph* graph, AVFilterContext* mCtx);
  AVFilterContext*
  getFilterCtx() {
    return mCtx;
  }
#endif // ! SWIG
protected:
  Filter(FilterGraph* graph, AVFilterContext* ctx);
  virtual
  ~Filter();
  void*
  getCtx() {
    return mCtx;
  }

private:
  AVFilterContext* mCtx;
  io::humble::ferry::RefPointer<FilterGraph> mGraph;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTER_H_ */
