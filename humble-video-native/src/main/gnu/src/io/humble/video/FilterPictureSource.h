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
 * FilterPictureSource.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERPICTURESOURCE_H_
#define FILTERPICTURESOURCE_H_

#include <io/humble/video/FilterSource.h>
#include <io/humble/video/MediaPicture.h>

namespace io {
namespace humble {
namespace video {

/**
 * A Source of MediaPicture objects for a FilterGraph.
 */
class VS_API_HUMBLEVIDEO FilterPictureSource : public io::humble::video::FilterSource
{
public:
  /**
   * @param picture The picture to fill if possible.
   * @return >=0 if a successful picture is fetched, or -1 for EOF.
   */
  virtual int32_t getPicture(MediaPicture* picture);
#ifndef SWIG
  static FilterPictureSource* make(FilterGraph*, AVFilterContext*);
#endif // ! SWIG
protected:
  FilterPictureSource(FilterGraph* graph, AVFilterContext* ctx);
  virtual
  ~FilterPictureSource();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERPICTURESOURCE_H_ */
