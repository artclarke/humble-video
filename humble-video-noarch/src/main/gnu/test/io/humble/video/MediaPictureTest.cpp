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
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/ferry/HumbleException.h>

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
  const PixelFormat::Type format = PixelFormat::PIX_FMT_YUV420P;
  const int32_t width = 17; // use a prime
  const int32_t height = 191; // use a prime
  RefPointer<MediaPicture> picture = MediaPicture::make(width, height, format);
  TS_ASSERT(picture);

  // let's test the data planes.
  RefPointer<PixelFormatDescriptor> pixDesc = PixelFormat::getDescriptor(format);
  TS_ASSERT(pixDesc);

  TS_ASSERT_EQUALS(3, pixDesc->getNumComponents());

  TS_ASSERT_EQUALS(pixDesc->getNumComponents(), picture->getNumDataPlanes());

  // This part of the test is meant for running under valgrind.
  // it touches each valid 'pixel' in the image and sets it to a
  // value. If Valgrind gives any errors about reaching outsides of
  // memory bounds, then there's a bug somewhere downstream in humble.
  RefPointer<Buffer> buf;

  for(int32_t i = 0; i < pixDesc->getNumComponents(); i++) {
    buf = picture->getData(i);
    TS_ASSERT(buf);
    uint8_t* data = (uint8_t*)buf->getBytes(0, buf->getBufferSize());
    // and we're going to set every bit and byte
    for(int32_t w = 0; w < width; w++)
      for(int32_t h = 0; h< height; h++) {
        RefPointer<PixelComponentDescriptor> compDesc = pixDesc->getComponentDescriptor(i);
        // adjust width and height to correct
        int32_t shiftWidth = i == 0 ? w : (w >> pixDesc->getLog2ChromaWidth());
        int32_t shiftHeight = i == 0 ? h : (h >> pixDesc->getLog2ChromaHeight());
        int32_t index = shiftHeight*(i == 0 ? width : (width >> pixDesc->getLog2ChromaWidth()))+shiftWidth;
        data[index] = (uint8_t)(((width*h)+w)/256);
      }

  }
}

void
MediaPictureTest::testCreationInvalidParameters() {
  RefPointer<MediaPicture> picture;
  const PixelFormat::Type format = PixelFormat::PIX_FMT_YUV420P;
  const int32_t width = 17; // use a prime
  const int32_t height = 191; // use a prime
  int32_t bufSize = PixelFormat::getBufferSizeNeeded(width, height, format);
  RefPointer<Buffer> buf = Buffer::make(0, bufSize);

  // and this method is going to spew lots of log
  // error unlesss we turn them off.
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

  TS_ASSERT_THROWS(
          MediaPicture::make(0, height, format),
          HumbleInvalidArgument);
  TS_ASSERT_THROWS(
          MediaPicture::make(width, 0, format),
          HumbleInvalidArgument);
  TS_ASSERT_THROWS(
          MediaPicture::make(width, height, PixelFormat::PIX_FMT_NONE),
          HumbleInvalidArgument);
  TS_ASSERT_THROWS(
          MediaPicture::make(buf.value(), 0, height, format),
          HumbleInvalidArgument);
  TS_ASSERT_THROWS(
          MediaPicture::make(buf.value(), width, 0, format),
          HumbleInvalidArgument);
  TS_ASSERT_THROWS(
          MediaPicture::make(buf.value(), width, height, PixelFormat::PIX_FMT_NONE),
          HumbleInvalidArgument);
  TS_ASSERT_THROWS(
          MediaPicture::make(0, width, height, format),
          HumbleInvalidArgument);
}

void
MediaPictureTest::testCreationFromBuffer() {
  RefPointer<MediaPicture> picture;
  const PixelFormat::Type format = PixelFormat::PIX_FMT_YUV420P;
  const int32_t width = 17; // use a prime
  const int32_t height = 191; // use a prime
  int32_t bufSize = PixelFormat::getBufferSizeNeeded(width, height, format);
  RefPointer<Buffer> buf = Buffer::make(0, bufSize);

  picture = MediaPicture::make(buf.value(), width, height, format);
  TS_ASSERT(picture);
}
