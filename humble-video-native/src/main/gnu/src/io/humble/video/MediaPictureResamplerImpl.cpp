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

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/Logger.h>

#include <io/humble/video/MediaPictureResamplerImpl.h>
#include <io/humble/video/MediaPictureImpl.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Property.h>
#include <io/humble/video/VideoExceptions.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io { namespace humble { namespace video
{

MediaPictureResamplerImpl :: MediaPictureResamplerImpl()
{
  mIHeight = 0;
  mIWidth = 0;
  mOHeight = 0;
  mOWidth = 0;
  mIPixelFmt = PixelFormat::PIX_FMT_NONE;
  mOPixelFmt = PixelFormat::PIX_FMT_NONE;
  mContext = 0;
  mState = STATE_INITED;
}

MediaPictureResamplerImpl :: ~MediaPictureResamplerImpl()
{
  if (mContext)
    sws_freeContext(mContext);
  mContext = 0;
}

int32_t
MediaPictureResamplerImpl :: getInputHeight()
{
  return mIHeight;
}

int32_t
MediaPictureResamplerImpl :: getInputWidth()
{
  return mIWidth;
}

int32_t
MediaPictureResamplerImpl :: getOutputHeight()
{
  return mOHeight;
}

int32_t
MediaPictureResamplerImpl :: getOutputWidth()
{
  return mOWidth;
}

PixelFormat::Type
MediaPictureResamplerImpl :: getInputFormat()
{
  return mIPixelFmt;
}

PixelFormat::Type
MediaPictureResamplerImpl :: getOutputFormat()
{
  return mOPixelFmt;
}

void
MediaPictureResamplerImpl::open() {
  mState = STATE_OPENED;
}
void
MediaPictureResamplerImpl :: resample(MediaPicture* pOutFrame, MediaPicture* pInFrame)
{
  int32_t retval = -1;
  if (mState != STATE_OPENED) {
    VS_THROW(HumbleRuntimeError("open() must be called before resample is attempted"));
  }
  MediaPictureImpl* outFrame = dynamic_cast<MediaPictureImpl*>(pOutFrame);
  MediaPictureImpl* inFrame  = dynamic_cast<MediaPictureImpl*>(pInFrame);
  if (!outFrame)
    VS_THROW(HumbleInvalidArgument("invalid output frame"));
  if (outFrame->getHeight() != mOHeight)
    VS_THROW(HumbleInvalidArgument("output frame height does not match expected value"));
  if (outFrame->getWidth() != mOWidth)
    VS_THROW(HumbleInvalidArgument("output frame width does not match expected value"));
  if (outFrame->getFormat() != mOPixelFmt)
    VS_THROW(HumbleInvalidArgument("output frame pixel format does not match expected value"));

  if (!inFrame)
    VS_THROW(HumbleInvalidArgument("invalid input frame"));

  if (inFrame->getHeight() != mIHeight)
    VS_THROW(HumbleInvalidArgument("input frame height does not match expected value"));
  if (inFrame->getWidth() != mIWidth)
    VS_THROW(HumbleInvalidArgument("input frame width does not match expected value"));
  if (inFrame->getFormat() != mIPixelFmt)
    VS_THROW(HumbleInvalidArgument("input frame pixel format does not match expected value"));
  if (!inFrame->isComplete())
    VS_THROW(HumbleRuntimeError("incoming frame doesn't have complete data"));

  outFrame->setComplete(false);
  AVFrame *outAVFrame = outFrame->getCtx();

  AVFrame *inAVFrame = inFrame->getCtx();

  retval = sws_scale(mContext, inAVFrame->data, inAVFrame->linesize, 0,
      mIHeight, outAVFrame->data, outAVFrame->linesize);

  FfmpegException::check(retval, "Error while resampling. ");

  RefPointer<Rational> timeBase = inFrame->getTimeBase();
  outFrame->setTimeBase(timeBase.value());
  outFrame->setQuality(inFrame->getQuality());
  outFrame->setComplete(retval >= 0);

}

MediaPictureResamplerImpl*
MediaPictureResamplerImpl :: make(
    int32_t outputWidth, int32_t outputHeight,
    PixelFormat::Type outputFmt,
    int32_t inputWidth, int32_t inputHeight,
    PixelFormat::Type inputFmt,
    int32_t flags)
{
  RefPointer<MediaPictureResamplerImpl> retval = 0;
  if (outputWidth <= 0)
    VS_THROW(HumbleInvalidArgument("invalid output width"));
  if (outputHeight <= 0)
    VS_THROW(HumbleInvalidArgument("invalid output height"));
  if (outputFmt == PixelFormat::PIX_FMT_NONE)
    VS_THROW(HumbleInvalidArgument("cannot set output pixel format to none"));
  if (inputWidth <= 0)
    VS_THROW(HumbleInvalidArgument("invalid input width"));
  if (inputHeight <= 0)
    VS_THROW(HumbleInvalidArgument("invalid input height"));
  if (inputFmt == PixelFormat::PIX_FMT_NONE)
    VS_THROW(HumbleInvalidArgument("cannot set input pixel format to none"));

  retval = MediaPictureResamplerImpl::make();
  retval->mOHeight = outputHeight;
  retval->mOWidth = outputWidth;
  retval->mOPixelFmt = outputFmt;

  retval->mIHeight = inputHeight;
  retval->mIWidth = inputWidth;
  retval->mIPixelFmt = inputFmt;

  if (inputWidth < outputWidth)
    // We're upscaling
    flags |= SWS_BICUBIC;
  else
    // We're downscaling
    flags |= SWS_AREA;

  retval->mContext = sws_getCachedContext(
      0, // cached context
      retval->mIWidth, // src width
      retval->mIHeight, // src height
      (enum AVPixelFormat)retval->mIPixelFmt, // src pixel type
      retval->mOWidth, // dst width
      retval->mOHeight, // dst height
      (enum AVPixelFormat)retval->mOPixelFmt, // dst pixel type
      flags, // Flags
      0, // Source Filter
      0, // Destination Filter
      0 // An array of parameters for filters
  );
  if (!retval->mContext) {
    VS_THROW(HumbleRuntimeError("could not allocate an image rescaler"));
  }

  return retval.get();
}

}}}
