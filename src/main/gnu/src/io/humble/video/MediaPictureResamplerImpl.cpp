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

#include <stdexcept>

#include <io/humble/ferry/Logger.h>

#include <io/humble/video/MediaPictureResamplerImpl.h>
#include <io/humble/video/MediaPictureImpl.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Property.h>

// This is the only place we include this to limit
// how much it pollutes our code, and make it
// easier to yank later.
extern "C" {
#include <libswscale/swscale.h>
}

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
  
  int32_t
  MediaPictureResamplerImpl :: resample(MediaPicture* pOutFrame, MediaPicture* pInFrame)
  {
    int32_t retval = -1;
    MediaPictureImpl* outFrame = dynamic_cast<MediaPictureImpl*>(pOutFrame);
    MediaPictureImpl* inFrame  = dynamic_cast<MediaPictureImpl*>(pInFrame);
    try
    {
      if (!outFrame)
        throw std::invalid_argument("invalid output frame");
      if (outFrame->getHeight() != mOHeight)
        throw std::runtime_error("output frame height does not match expected value");
      if (outFrame->getWidth() != mOWidth)
        throw std::runtime_error("output frame width does not match expected value");
      if (outFrame->getFormat() != mOPixelFmt)
        throw std::runtime_error("output frame pixel format does not match expected value");

      
      if (!inFrame)
        throw std::invalid_argument("invalid input frame");
      
      if (inFrame->getHeight() != mIHeight)
        throw std::runtime_error("input frame height does not match expected value");
      if (inFrame->getWidth() != mIWidth)
        throw std::runtime_error("input frame width does not match expected value");
      if (inFrame->getFormat() != mIPixelFmt)
        throw std::runtime_error("input frame pixel format does not match expected value");
      if (!inFrame->isComplete())
        throw std::runtime_error("incoming frame doesn't have complete data");

      // Allocate our output frame.
      outFrame->setComplete(false);
      AVFrame *outAVFrame = outFrame->getCtx();

      // Get our input frame; since it's complete() we can pass anything
      // for the argument
      AVFrame *inAVFrame = inFrame->getCtx(); 

      /*
      VS_LOG_DEBUG("%dx%d(%d:%d)=>%dx%d(%d:%d)",
          inFrame->getWidth(), inFrame->getHeight(), inFrame->getFormat(), inAVFrame->linesize[0],
          outFrame->getWidth(), outFrame->getHeight(), outFrame->getFormat(), outAVFrame->linesize[0]);

      memset(outAVFrame->data[0], 64*3-1, outAVFrame->linesize[0]);
      */
      // And do the conversion; works around FFmpeg checkin r30306
      const uint8_t* srcFrame[4] = {
          inAVFrame->data[0],
          inAVFrame->data[1],
          inAVFrame->data[2],
          inAVFrame->data[3],
      };
      retval = sws_scale(mContext, srcFrame, inAVFrame->linesize, 0,
          mIHeight, outAVFrame->data, outAVFrame->linesize);
      
      // and go home happy.
      {
        outFrame->setQuality(inFrame->getQuality());
        outFrame->setComplete(retval >= 0);
      }
    }
    catch (std::bad_alloc& e)
    {
      throw e;
    }
    catch (std::exception& e)
    {
      VS_LOG_DEBUG("error: %s", e.what());
      retval = -1;
    }
    return retval;
  }
  
/*
 * The next two methods are to work around a break in backwards compatibility introduced
 * in FFmpeg in September 2010
 */
static int handle_jpeg(enum AVPixelFormat *format)
{
    switch (*format) {
      case PIX_FMT_YUVJ420P: *format = PIX_FMT_YUV420P; return 1;
      case PIX_FMT_YUVJ422P: *format = PIX_FMT_YUV422P; return 1;
      case PIX_FMT_YUVJ444P: *format = PIX_FMT_YUV444P; return 1;
      case PIX_FMT_YUVJ440P: *format = PIX_FMT_YUV440P; return 1;
      default: return 0;
    }
}

static SwsContext *humbleSws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
                           int dstW, int dstH, enum AVPixelFormat dstFormat, int flags,
                           SwsFilter *srcFilter, SwsFilter *dstFilter, const double *param)
{
    SwsContext *c;
    const int *coeff;
    int srcRange=handle_jpeg(&srcFormat);
    int dstRange=handle_jpeg(&dstFormat);

    if(!(c=sws_alloc_context()))
        return NULL;

    av_opt_set_int(c, "sws_flags", flags, 0);
    av_opt_set_int(c, "srcw", srcW, 0);
    av_opt_set_int(c, "srch", srcH, 0);
    av_opt_set_int(c, "dstw", dstW, 0);
    av_opt_set_int(c, "dsth", dstH, 0);
    av_opt_set_int(c, "src_range", srcRange, 0);
    av_opt_set_int(c, "dst_range", dstRange, 0);
    av_opt_set_int(c, "src_format", srcFormat, 0);
    av_opt_set_int(c, "dst_format", dstFormat, 0);

    if (param) {
        av_opt_set_double(c, "param0", param[0], 0);
        av_opt_set_double(c, "param1", param[1], 0);
    }

    coeff = sws_getCoefficients(SWS_CS_DEFAULT);
    sws_setColorspaceDetails(c, coeff, srcRange, coeff /* FIXME*/, dstRange, 0, 1<<16, 1<<16);

    if(sws_init_context(c, srcFilter, dstFilter) < 0){
        sws_freeContext(c);
        return NULL;
    }

    return c;
}
  MediaPictureResamplerImpl*
  MediaPictureResamplerImpl :: make(
          int32_t outputWidth, int32_t outputHeight,
          PixelFormat::Type outputFmt,
          int32_t inputWidth, int32_t inputHeight,
          PixelFormat::Type inputFmt)
  {
    MediaPictureResamplerImpl* retval = 0;
    try
    {
      if (outputWidth <= 0)
        throw std::invalid_argument("invalid output width");
      if (outputHeight <= 0)
        throw std::invalid_argument("invalid output height");
      if (outputFmt == PixelFormat::PIX_FMT_NONE)
        throw std::invalid_argument("cannot set output pixel format to none");
      if (inputWidth <= 0)
        throw std::invalid_argument("invalid input width");
      if (inputHeight <= 0)
        throw std::invalid_argument("invalid input height");
      if (inputFmt == PixelFormat::PIX_FMT_NONE)
        throw std::invalid_argument("cannot set input pixel format to none");
      
      retval = MediaPictureResamplerImpl::make();
      if (retval)
      {
        retval->mOHeight = outputHeight;
        retval->mOWidth = outputWidth;
        retval->mOPixelFmt = outputFmt;

        retval->mIHeight = inputHeight;
        retval->mIWidth = inputWidth;
        retval->mIPixelFmt = inputFmt;
        
        int32_t flags = 0;
        if (inputWidth < outputWidth)
          // We're upscaling
          flags = SWS_BICUBIC;
        else
          // We're downscaling
          flags = SWS_AREA;
        
        retval->mContext = humbleSws_getContext(
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
        if (!retval->mContext)
          throw std::runtime_error("could not allocate a image rescaler");
      }
    }
    catch (std::bad_alloc& e)
    {
      VS_REF_RELEASE(retval);
    }
    catch (std::exception& e)
    {
      VS_LOG_DEBUG("error: %s", e.what());
      VS_REF_RELEASE(retval);
    }

    return retval;
  }

  }}}
