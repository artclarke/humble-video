/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef MEDIAPICTURERESAMPLERIMPL_H_
#define MEDIAPICTURERESAMPLERIMPL_H_

#include <io/humble/video/MediaPictureResampler.h>


/*
 * Do not include anything from the swscale library here.
 * 
 * We're just keeping an opaque pointer here.
 */
struct SwsContext;

namespace io { namespace humble { namespace video
{

class MediaPictureResamplerImpl : public MediaPictureResampler
{
private:
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaPictureResamplerImpl)
public:
virtual int32_t getInputWidth();
virtual int32_t getInputHeight();
virtual PixelFormat::Type getInputFormat();

virtual int32_t getOutputWidth();
virtual int32_t getOutputHeight();
virtual PixelFormat::Type getOutputFormat();

virtual void open();
virtual int32_t resample(MediaSampled *pOutFrame, MediaSampled *pInFrame);
virtual int32_t resamplePicture(MediaPicture *pOutFrame, MediaPicture *pInFrame);

virtual State getState() { return mState; }
static MediaPictureResamplerImpl* make(
    int32_t outputWidth, int32_t outputHeight,
    PixelFormat::Type outputFmt,
    int32_t inputWidth, int32_t inputHeight,
    PixelFormat::Type inputFmt,
    int32_t flags);
protected:
virtual void* getCtx() { return mContext; }
MediaPictureResamplerImpl();
virtual ~MediaPictureResamplerImpl();
private:
int32_t mIHeight;
int32_t mIWidth;
int32_t mOHeight;
int32_t mOWidth;
PixelFormat::Type mIPixelFmt;
PixelFormat::Type mOPixelFmt;
State mState;
SwsContext* mContext;
};

}}}

#endif /*MEDIAPICTURERESAMPLERIMPL_H_*/
