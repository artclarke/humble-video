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
 * MediaPictureImpl.h
 *
 *  Created on: Jul 13, 2013
 *      Author: aclarke
 */

#ifndef MEDIAPICTUREIMPL_H_
#define MEDIAPICTUREIMPL_H_

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/MediaPicture.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO MediaPictureImpl : public io::humble::video::MediaPicture
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaPictureImpl)
public:
  static MediaPictureImpl*
  make(int32_t width, int32_t height, PixelFormat::Type format);

  static MediaPictureImpl*
  make(io::humble::ferry::Buffer* buffer, int32_t width, int32_t height,
      PixelFormat::Type format);

  static MediaPictureImpl*
  make(MediaPictureImpl* src, bool copy);

  void copy(AVFrame*, bool complete);

  virtual io::humble::ferry::Buffer*
  getData(int32_t plane);

  virtual int32_t
  getDataPlaneSize(int32_t plane);

  virtual int32_t
  getLineSize(int32_t plane);

  virtual int32_t
  getNumDataPlanes();

  virtual void
  setComplete(bool val);

  virtual bool
  isComplete() { return mComplete; }

  virtual int32_t
  getWidth() {
    return getCtx()->width;
  }

  virtual int32_t
  getHeight() {
    return getCtx()->height;
  }

  virtual PixelFormat::Type
  getFormat() {
    return (PixelFormat::Type) getCtx()->format;
  }

  virtual int32_t
  getCodedPictureNumber() {
    return getCtx()->coded_picture_number;
  }

  virtual void
  setCodedPictureNumber(int32_t n) {
    getCtx()->coded_picture_number = n;
  }

  virtual int32_t
  getDisplayPictureNumber() { return getCtx()->display_picture_number; }

  virtual void
  setDisplayPictureNumber(int32_t n) { getCtx()->display_picture_number = n; }

  virtual int32_t
  getQuality() { return getCtx()->quality; }

  virtual void
  setQuality(int32_t q) { getCtx()->quality = q; }

  virtual int64_t
  getError(int32_t dataPlane);

  virtual int32_t
  getRepeatPicture() { return getCtx()->repeat_pict; }
  virtual void
  setRepeatPicture(int32_t n) { getCtx()->repeat_pict = n; }

  virtual bool
  isInterlacedFrame() { return getCtx()->interlaced_frame; }
  virtual void
  setInterlacedFrame(bool val) { getCtx()->interlaced_frame = val; }

  virtual bool
  isTopFieldFirst() { return getCtx()->top_field_first; }
  virtual void
  setTopFieldFirst(bool val) { getCtx()->top_field_first = val; }

  virtual bool
  isPaletteChanged() { return getCtx()->palette_has_changed; }
  virtual void
  setPaletteChange(bool val) { getCtx()->palette_has_changed = val; }

  virtual MediaPicture::Type getType() { return (MediaPicture::Type) getCtx()->pict_type; }
  virtual void setType(MediaPicture::Type type) { getCtx()->pict_type = (enum AVPictureType)type; }

  virtual int32_t getNumSamples() { return 1; }
  virtual AVFrame*
  getCtx() {
    return mFrame;
  }
  virtual int64_t logMetadata(char*, size_t);
protected:
  MediaPictureImpl();
  virtual
  ~MediaPictureImpl();

private:
  void validatePlane(int32_t plane);
  AVFrame* mFrame;
  bool     mComplete;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAPICTUREIMPL_H_ */
