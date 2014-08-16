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
 * FilterPictureSink.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERPICTURESINK_H_
#define FILTERPICTURESINK_H_

#include <io/humble/video/FilterSink.h>
#include <io/humble/video/MediaPicture.h>

namespace io {
namespace humble {
namespace video {

/**
 * A Sink of MediaPicture objects for a FilterGraph.
 */
class VS_API_HUMBLEVIDEO FilterPictureSink : public io::humble::video::FilterSink
{
public:
  /**
   * @param picture The picture to fill if possible.
   * @return >=0 if a successful picture is fetched, or -1 for EOF.
   */
  virtual int32_t getPicture(MediaPicture* picture);
#ifndef SWIG
  static FilterPictureSink* make(FilterGraph*, AVFilterContext*);
#endif // ! SWIG
protected:
  FilterPictureSink(FilterGraph* graph, AVFilterContext* ctx);
  virtual
  ~FilterPictureSink();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERPICTURESINK_H_ */
