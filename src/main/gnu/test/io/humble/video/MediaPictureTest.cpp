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
 * MediaPictureTester.cpp
 *
 *  Created on: Jul 13, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/MediaPicture.h>
#include "MediaPictureTest.h"

using namespace io::humble::ferry;
using namespace io::humble::video;

MediaPictureTest::MediaPictureTest() {
}

MediaPictureTest::~MediaPictureTest() {
}

void
MediaPictureTest::testCreation() {
  RefPointer<MediaPicture> picture = MediaPicture::make(1024, 1024, PixelFormat::PIX_FMT_YUV420P);
  TS_ASSERT(picture);
}

void
MediaPictureTest::testCreationFromBuffer() {

}
