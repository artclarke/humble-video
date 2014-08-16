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
 * A source of MediaPicture objects for a FilterGraph.
 */
class VS_API_HUMBLEVIDEO FilterPictureSource : public io::humble::video::FilterSource
{
public:
  /**
   * Adds a picture to this source. NOTE: If you add a picture to a FilterSource
   * be careful with re-using or rewriting the underlying data. Filters will
   * try hard to avoid copying data, so if you change the data out from under
   * them unexpected results can occur.
   * @param picture the picture to add. Must be non-null and complete.
   * @throws InvalidArgument if picture is null or audio is not complete.
   */
  void
  addPicture(MediaPicture* picture);
#ifndef SWIG
  static FilterPictureSource* make(FilterGraph*, AVFilterContext*);
#endif // ! SWIG.
protected:
  FilterPictureSource(FilterGraph* graph, AVFilterContext* ctx);
  virtual
  ~FilterPictureSource();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERPICTURESOURCE_H_ */
