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

#include <io/humble/ferry/Buffer.h>
#include <io/humble/video/MediaRaw.h>
#include <io/humble/video/PixelFormat.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO MediaPicture : public io::humble::video::MediaSampled
{
public:

  /**
    * The different types of images that we can set.
    *
    * @see #getPictureType()
    */
   typedef enum Type {
     /** Undefined */
     PICTURE_TYPE_NONE = AV_PICTURE_TYPE_NONE,
     /** Intra */
     PICTURE_TYPE_I = AV_PICTURE_TYPE_I,
     /** Predicted */
     PICTURE_TYPE_P = AV_PICTURE_TYPE_P,
     /** Bi-dir predicted */
     PICTURE_TYPE_B = AV_PICTURE_TYPE_B,
     /** S(GMC)-VOP MPEG4 */
     PICTURE_TYPE_S = AV_PICTURE_TYPE_S,
     /** Switching Intra */
     PICTURE_TYPE_SI = AV_PICTURE_TYPE_SI,
     /** Switching Predicted */
     PICTURE_TYPE_SP = AV_PICTURE_TYPE_SP,
     /** BI type */
     PICTURE_TYPE_BI = AV_PICTURE_TYPE_BI,
   } Type;

  /**
   * Create a media picture.
   *
   * @param width Number of pixels wide.
   * @param height Number of pixels high.
   * @param format PixelFormat.Type of the {@link MediaPicture}
   *
   * @return A MediaPicture with memory allocated for it.
   *
   * @throws InvalidArgument if width or height or negative, of format is {@link PixelFormat.Type.PIX_FMT_NONE}
   */
  static MediaPicture*
  make(int32_t width, int32_t height, PixelFormat::Type format);

  /**
   * Create a media picture using a buffer as the memory backing it.
   *
   * @param buffer A buffer of data to use for the image.
   * @param width Number of pixels wide.
   * @param height Number of pixels high.
   * @param format PixelFormat.Type of the {@link MediaPicture}
   *
   * @return A MediaPicture with memory allocated for it.
   *
   * @throws InvalidArgument if buffer is null or too small for the image, width or height or negative, of format is {@link PixelFormat.Type.PIX_FMT_NONE}
   */
  static MediaPicture*
  make(io::humble::ferry::Buffer* buffer, int32_t width, int32_t height,
      PixelFormat::Type format);

  /**
   * Create a media picture from src.
   *
   * @param src A source {@link MediaPicture}.
   * @param copy If true then all data is copied from src into the new object. If false, then the new object just copies meta-data but references the same underlying buffers.
   *
   * @return A {@link MediaPicture}
   *
   * @throws InvalidArgument if src is null.
   */
  static MediaPicture*
  make(MediaPicture* src, bool copy);

  /**
   * Get any underlying raw data available for this object.
   *
   * @param plane The plane number if {@link getFormat()} is Planar (rather than packed) image data.  Pass zero for packed data.
   * @return The raw data, or null if not accessible.
   */
  virtual io::humble::ferry::Buffer*
  getData(int32_t plane)=0;

  /**
   * The total number of bytes in {@link #getData()} that represent valid image data.
   *
   * @return The size in bytes of that plane of image data.
   */
  virtual int32_t
  getDataPlaneSize(int32_t plane)=0;

  /** Returns the number of data planes in this object. */
  virtual int32_t
  getNumDataPlanes()=0;

  /**
   * The width in pixels of a line of the image in the given plane.
   * @plane The plane from the data. Not all images have more than one plane. None have more than 4.
   * @return The width in pixels, or 0 on error.
   */
  virtual int32_t
  getLineSize(int32_t plane)=0;

  /**
   * Set this frame to complete.
   */
  virtual void
  setComplete(bool complete)=0;

  /**
   * Returns true if the data in this picture is marked as complete.
   * @see #setComplete(long)
   */
  virtual bool
  isComplete()=0;

  virtual int32_t
  getWidth()=0;

  virtual int32_t
  getHeight()=0;

  virtual PixelFormat::Type
  getFormat()=0;

  /**
   * picture number in bitstream order
   */
  virtual int32_t
  getCodedPictureNumber()=0;

  /**
   * set picture number
   * @see #getCodedPictureNumber()
   */
  virtual void
  setCodedPictureNumber(int32_t n)=0;

  /**
   * picture number in display order
   */
  virtual int32_t
  getDisplayPictureNumber()=0;

  /**
   * set picture number in display order
   * @see #getDisplayPictureNumber
   */
  virtual void
  setDisplayPictureNumber(int32_t n)=0;

  /**
   * quality (between 1 (good) and FF_LAMBDA_MAX (bad))
   */
  virtual int32_t
  getQuality()=0;

  /**
   * set quality.
   * @see #getQuality()
   */
  virtual void
  setQuality(int32_t q)=0;

  /**
   * Get the error flags (if any) that the decoder set for this image on any given plane.
   * @param dataPlane The plane to search for error flags.
   */
  virtual int64_t
  getError(int32_t dataPlane)=0;

  /**
   * When decoding, this signals how much the picture must be delayed.
   * extra_delay = repeat_pict / (2*fps)
   */
  virtual int32_t
  getRepeatPicture()=0;
  virtual void
  setRepeatPicture(int32_t n)=0;

  /**
   * The content of the picture is interlaced.
   */
  virtual bool
  isInterlacedFrame()=0;
  virtual void
  setInterlacedFrame(bool val)=0;

  /**
   * If the content is interlaced, is top field displayed first.
   */
  virtual bool
  isTopFieldFirst()=0;
  virtual void
  setTopFieldFirst(bool val)=0;

  /**
   * Tell user application that palette has changed from previous frame.
   */
  virtual bool
  isPaletteChanged()=0;
  virtual void
  setPaletteChange(bool val)=0;

  /**
   * Get the type of picture this object was decoded from when in a encoded stream
   * (or give a hint to an encoder about how you think it should be encoded).
   */
  virtual MediaPicture::Type getType()=0;
  virtual void setType(MediaPicture::Type type)=0;

protected:
  MediaPicture();
  virtual
  ~MediaPicture();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAPICTURE_H_ */
