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
 * FilterPictureSink.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERPICTURESINK_H_
#define FILTERPICTURESINK_H_

#include <io/humble/video/FilterSink.h>

namespace io {
namespace humble {
namespace video {

/**
 * A Sink of {@link MediaPicture} objects for a {@link FilterGraph}.
 */
class FilterPictureSink : public io::humble::video::FilterSink
{
protected:
  FilterPictureSink();
  virtual
  ~FilterPictureSink();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERPICTURESINK_H_ */
