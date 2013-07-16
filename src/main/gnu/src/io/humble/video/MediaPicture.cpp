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
 * MediaPicture.cpp
 *
 *  Created on: Jul 13, 2013
 *      Author: aclarke
 */

#include "MediaPicture.h"
#include "MediaPictureImpl.h"
#include "Global.h"
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/RefPointer.h>

namespace io {
namespace humble {
namespace video {

using namespace io::humble::ferry;

MediaPicture::MediaPicture() {

}

MediaPicture::~MediaPicture() {
}

MediaPicture*
MediaPicture::make(int32_t width, int32_t height, PixelFormat::Type format)
{
  Global::init();
  return MediaPictureImpl::make(width, height, format);
}

MediaPicture*
MediaPicture::make(io::humble::ferry::IBuffer* buffer, int32_t width, int32_t height,
    PixelFormat::Type format)
{
  Global::init();
  return MediaPictureImpl::make(buffer, width, height, format);
}

MediaPicture*
MediaPicture::make(MediaPicture* src, bool copy)
{
  Global::init();
  return MediaPictureImpl::make(dynamic_cast<MediaPictureImpl*>(src), copy);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
