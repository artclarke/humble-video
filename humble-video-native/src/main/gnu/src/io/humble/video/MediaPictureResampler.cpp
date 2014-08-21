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

#include <io/humble/video/MediaPictureResampler.h>
#include <io/humble/video/Global.h>
#include <io/humble/video/MediaPictureResamplerImpl.h>

namespace io { namespace humble { namespace video
{

MediaPictureResampler :: MediaPictureResampler()
{
}

MediaPictureResampler :: ~MediaPictureResampler()
{
}

MediaPictureResampler*
MediaPictureResampler :: make(
    int32_t outputWidth, int32_t outputHeight,
    PixelFormat::Type outputFmt,
    int32_t inputWidth, int32_t inputHeight,
    PixelFormat::Type inputFmt,
    int32_t flags)
{
  Global::init();
  return MediaPictureResamplerImpl::make(outputWidth, outputHeight, outputFmt,
      inputWidth, inputHeight, inputFmt, flags);
}

}}}
