/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MEDIAPICTURERESAMPLER_H_
#define MEDIAPICTURERESAMPLER_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/PixelFormat.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/MediaPicture.h>
extern "C" {
#include <libswscale/swscale.h>
}
namespace io { namespace humble { namespace video
{

/**
 * Converts MediaPicture objects of a given width, height and format to a new
 * width, height or format.
 */
class VS_API_HUMBLEVIDEO MediaPictureResampler : public io::humble::video::Configurable
{
public:
  typedef enum Flag {
    FLAG_FAST_BILINEAR = SWS_FAST_BILINEAR,
    FLAG_BILINEAR = SWS_BILINEAR,
    FLAG_BICUBIC = SWS_BICUBIC,
    FLAG_X = SWS_X,
    FLAG_POINT = SWS_POINT,
    FLAG_AREA = SWS_AREA,
    FLAG_BICUBLIN = SWS_BICUBLIN,
    FLAG_GAUSS = SWS_GAUSS,
    FLAG_SINC = SWS_SINC,
    FLAG_LANCZOS = SWS_LANCZOS,
    FLAG_SPLINE = SWS_SPLINE,
    FLAG_SRC_V_CHR_DROP_MASK = SWS_SRC_V_CHR_DROP_MASK,
    FLAG_SRC_V_CHR_DROP_SHIFT = SWS_SRC_V_CHR_DROP_SHIFT,
    FLAG_PARAM_DEFAULT = SWS_PARAM_DEFAULT,
    FLAG_FULL_CHR_H_INT = SWS_FULL_CHR_H_INT,
    FLAG_FULL_CHR_H_INP = SWS_FULL_CHR_H_INP,
    FLAG_DIRECT_BGR = SWS_DIRECT_BGR,
    FLAG_ACCURATE_RND = SWS_ACCURATE_RND,
    FLAG_BITEXACT = SWS_BITEXACT,
    FLAG_ERROR_DIFFUSION = SWS_ERROR_DIFFUSION,
  } Flag;

  typedef enum State {
    STATE_INITED,
    STATE_OPENED,
    STATE_ERROR
  } State;
  virtual State getState()=0;

  /** Get the width in pixels we expect on the input frame to the resampler.
   * @return The width we expect on the input frame to the resampler.
   */
  virtual int32_t getInputWidth()=0;

  /** Get the height in pixels we expect on the input frame to the resampler.
   * @return The height we expect on the input frame to the resampler.
   */
  virtual int32_t getInputHeight()=0;

  /**
   * Get the input pixel format.
   * @return The pixel format we expect on the input frame to the resampler.
   */
  virtual PixelFormat::Type getInputFormat()=0;

  /**
   * Get the output width, in pixels.
   * @return The width we will resample the output frame to
   */
  virtual int32_t getOutputWidth()=0;

  /**
   * Get the output height, in pixels.
   * @return The height we will resample the output frame to
   */
  virtual int32_t getOutputHeight()=0;

  /**
   * Get the output pixel format.
   * @return The pixel format we will resample the output frame to
   */
  virtual PixelFormat::Type getOutputFormat()=0;


  /**
   * Opens the resampler so it can be ready for resampling.
   * You should NOT set options after you open this object.
   */
  virtual void open()=0;

  /**
   * Resample in to out based on the resampler parameters.
   *
   * Resamples the in picture based on the parameters set when
   * this resampler was constructed.
   *
   * @param out The picture we'll resample to.  Check
   *     MediaPicture#isComplete() after the call.
   * @param in The picture we'll resample from.
   *
   * @throws InvalidArgument if in our out does not match the parameters this
   *         resampler was set with.
   */
  virtual void resample(MediaPicture *out, MediaPicture *in)=0;

  /**
   * Get a new picture resampler.
   *
   * @param outputWidth The width in pixels you want to output frame to have.
   * @param outputHeight The height in pixels you want to output frame to have.
   * @param outputFmt The pixel format of the output frame.
   * @param inputWidth The width in pixels the input frame will be in.
   * @param inputHeight The height in pixels the input frame will be in.
   * @param inputFmt The pixel format of the input frame.
   * @return a new object, or null if we cannot allocate one.
   */
  static MediaPictureResampler* make(
      int32_t outputWidth, int32_t outputHeight,
      PixelFormat::Type outputFmt,
      int32_t inputWidth, int32_t inputHeight,
      PixelFormat::Type inputFmt,
      int32_t flags);

protected:
  MediaPictureResampler();
  virtual ~MediaPictureResampler();
};

}}}

#endif /*MEDIAPICTURERESAMPLER_H_*/
