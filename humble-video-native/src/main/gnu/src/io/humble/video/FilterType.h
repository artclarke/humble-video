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
 * FilterType.h
 *
 *  Created on: Aug 4, 2013
 *      Author: aclarke
 */

#ifndef FILTERTYPE_H_
#define FILTERTYPE_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/Codec.h>


namespace io {
namespace humble {
namespace video {

/**
 * A type of filter that can be created.
 */
class VS_API_HUMBLEVIDEO FilterType : public io::humble::ferry::RefCounted
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(FilterType);
public:
  /**
   * Flags that show features different FilterType object support.
   */
  typedef enum Flag {
    /**
     * Do not use.
     */
    FILTER_FLAG_UNKNOWN = -1,
    /**
     * The number of the filter inputs is not determined just by the number of inputs explicitly added.
     * The filter might add additional inputs during initialization depending on the
     * options supplied to it.
     */
    FILTER_FLAG_DYNAMIC_INPUTS = AVFILTER_FLAG_DYNAMIC_INPUTS,
    /**
     * The number of the filter outputs is not determined just by AVFilter.outputs.
     * The filter might add additional outputs during initialization depending on
     * the options supplied to it.
     */
    FILTER_FLAG_DYNAMIC_OUTPUTS = AVFILTER_FLAG_DYNAMIC_OUTPUTS,
    /**
     * The filter supports multithreading by splitting frames into multiple parts
     * and processing them concurrently.
     */
    FILTER_FLAG_SLICE_THREADS = AVFILTER_FLAG_SLICE_THREADS,
    /**
     * Some filters support a generic "enable" expression option that can be used
     * to enable or disable a filter in the timeline. Filters supporting this
     * option have this flag set. When the enable expression is false, the default
     * no-op filter_frame() function is called in place of the filter_frame()
     * callback defined on each input pad, thus the frame is passed unchanged to
     * the next filters.
     */
    FILTER_FLAG_SUPPORT_TIMELINE_GENERIC = AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC,
    /**
     * Same as AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC, except that the filter will
     * have its filter_frame() callback(s) called as usual even when the enable
     * expression is false. The filter will disable filtering within the
     * filter_frame() callback(s) itself, for example executing code depending on
     * the AVFilterContext->is_disabled value.
     */
    FILTER_FLAG_SUPPORT_TIMELINE_INTERNAL = AVFILTER_FLAG_SUPPORT_TIMELINE_INTERNAL,
    /**
     * Handy mask to test whether the filter supports or no the timeline feature
     * (internally or generically).
     */
    FILTER_FLAG_SUPPORT_TIMELINE = AVFILTER_FLAG_SUPPORT_TIMELINE,
  } Flag;

  /**
   * @return name of this filter type.
   */
  virtual const char* getName() { return mCtx->name; }

  /**
   * @return description of this filter type.
   */
  virtual const char* getDescription() { return mCtx->description; }

  /**
   * @return bitmask of Flags set on this FilterType.
   */
  virtual int getFlags() { return mCtx->flags; }

  /**
   * @param flag flag to check setting of.
   * @return is this flag set on the given FilterType
   */
  virtual bool getFlag(Flag flag) { return mCtx->flags & flag; }

  /**
   * @return number of inputs this FilterType expects.
   */
  virtual int32_t getNumInputs();

  /**
   * @param index which input to get name of
   * @return the input name
   * @throws InvalidArgument if index < 0 || index > #getNumInputs().
   */
  virtual const char* getInputName(int32_t index);

  /**
   * @param index which input to get type of
   * @return the input media type
   * @throws InvalidArgument if index < 0 || index > #getNumInputs().
   */
  virtual MediaDescriptor::Type getInputType(int32_t index);

  /**
   * @return number of outputs this FilterType expects.
   */
  virtual int32_t getNumOutputs();

  /**
   * @param index which output to get name of
   * @return the output name
   * @throws InvalidArgument if index < 0 || index > #getNumOutputs().
   */
  virtual const char* getOutputName(int32_t index);

  /**
   * @param index which output to get type of
   * @return the output media type
   * @throws InvalidArgument if index < 0 || index > #getNumOutputs().
   */
  virtual MediaDescriptor::Type getOutputType(int32_t index);

  /**
   * Find a given filter by name.
   *
   * @param name name of filter.
   */
  static FilterType* findFilterType(const char* name);

  /**
   * @return the number of filters installed in this system.
   */
  static int32_t getNumFilterTypes();

  /**
   * @param index the position in the list of installed filters to get. Use
   *   with #getNumFilters() to iterate over all the filters installed
   *   in the system.
   * @return get the filter at the <code>index</code> position in the installed filters.
   * @throws InvalidArgument if index < 0 || index >= #getNumFilterTypes().
   */
  static FilterType* getFilterType(int32_t index);
#ifndef SWIG
  const AVFilter* getCtx() { return mCtx; }
  static FilterType* make(const AVFilter* f);
#endif
protected:
  FilterType() : mCtx(0) {}
  virtual ~FilterType() {}
private:
  const AVFilter* mCtx;
};


} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERTYPE_H_ */
