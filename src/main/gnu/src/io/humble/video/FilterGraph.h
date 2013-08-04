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

#ifndef FILTERGRAPH_H_
#define FILTERGRAPH_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/FilterType.h>

namespace io {
namespace humble {
namespace video {

class Filter;
class FilterGraph : public io::humble::video::Configurable
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(FilterGraph);
public:
  typedef enum AutoConvertFlag {
    /** all automatic conversions enabled */
     AUTO_CONVERT_ALL=AVFILTER_AUTO_CONVERT_ALL,
     /** all automatic conversions disabled */
     AUTO_CONVERT_NONE=AVFILTER_AUTO_CONVERT_NONE,
  } AutoConvertFlag;
  virtual void addFilter(FilterType* type, const char* name);
  virtual Filter* getFilter(const char* name);
  virtual void setAutoConvert(AutoConvertFlag value);
  virtual AutoConvertFlag getAutoConvert();
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
  void queueCommand(const char *target,
      const char *command,
      const char *arguments, int flags, double ts);

#ifdef SWIG
  %newobject getHumanReadableString();
  %typemap(newfree) char * "av_free($1);";
#endif
  char* getHumanReadableString() {
    return avfilter_graph_dump(mCtx, "");
  }



protected:
  virtual void* getCtx() { return mCtx; }
  FilterGraph();
  virtual
  ~FilterGraph();

private:
  AVFilterGraph* mCtx;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERGRAPH_H_ */
