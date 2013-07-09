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

#include <io/humble/ferry/Logger.h>

#include "VideoPicture.h"
#include "Global.h"
#include "VideoPictureImpl.h"
#include <stdexcept>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video
{

  VideoPicture::VideoPicture()
  {
  }

  VideoPicture::~VideoPicture()
  {
  }

  VideoPicture*
  VideoPicture::make(PixelFormat::Type format, int width, int height)
  {
    Global::init();
    return VideoPictureImpl::make(format, width, height);
  }

  VideoPicture*
  VideoPicture::make(
      io::humble::ferry::IBuffer* buffer,
      PixelFormat::Type format, int width, int height)
  {
    Global::init();
    return VideoPictureImpl::make(buffer, format, width, height);
  }
  

  VideoPicture*
  VideoPicture::make(VideoPicture *srcFrame)
  {
    VideoPicture* retval = 0;
    Global::init();
    try
    {
      if (!srcFrame)
        throw std::runtime_error("no source data to copy");

      retval = VideoPicture::make(srcFrame->getPixelType(), srcFrame->getWidth(), srcFrame->getHeight());
      if (!retval)
        throw std::runtime_error("could not allocate new frame");
      
      if (!retval->copy(srcFrame))
        throw std::runtime_error("could not copy source frame");
    }
    catch (std::bad_alloc &e)
    {
      VS_REF_RELEASE(retval);
      throw e;
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("got error: %s", e.what());
      VS_REF_RELEASE(retval);
    }
    return retval;
  }
}}}
