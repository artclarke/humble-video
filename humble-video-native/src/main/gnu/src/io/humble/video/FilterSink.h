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
 * FilterSink.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERSINK_H_
#define FILTERSINK_H_

#include <io/humble/video/FilterEndPoint.h>
#include <io/humble/video/MediaRaw.h>

namespace io {
namespace humble {
namespace video {

/**
 * A sink of MediaRaw objects for a FilterGraph.
 */
class VS_API_HUMBLEVIDEO FilterSink : public FilterEndPoint
{
public:
  /**
   * Set the frame size of this sink. If set to non-zero then #getAudio(MediaAudio)
   * will only every return exactly that number of samples.
   */
  virtual void setFrameSize(int32_t);

  /**
   * Get the frame size.
   */
  virtual int32_t getFrameSize();

protected:
  int32_t get(MediaRaw*);
  FilterSink(FilterGraph* graph, AVFilterContext* ctx);
  virtual
  ~FilterSink();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERSINK_H_ */
