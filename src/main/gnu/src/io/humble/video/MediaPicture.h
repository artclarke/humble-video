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
 * MediaPicture.h
 *
 *  Created on: Jul 13, 2013
 *      Author: aclarke
 */

#ifndef MEDIAPICTURE_H_
#define MEDIAPICTURE_H_

#include <io/humble/video/MediaRaw.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO MediaPicture : public io::humble::video::MediaRaw
{
protected:
  MediaPicture();
  virtual
  ~MediaPicture();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAPICTURE_H_ */
