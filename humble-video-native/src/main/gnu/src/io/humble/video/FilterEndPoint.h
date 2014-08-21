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
 * FilterEndPoint.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERENDPOINT_H_
#define FILTERENDPOINT_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Filter.h>

namespace io {
namespace humble {
namespace video {

/**
 * An source or a sink that terminates the end points of a FilterGraph.
 */
class VS_API_HUMBLEVIDEO FilterEndPoint : public Filter
{
protected:
  FilterEndPoint(FilterGraph* graph, AVFilterContext* ctx);
  virtual
  ~FilterEndPoint();

};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERENDPOINT_H_ */
