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
 *
 * OutputFormat.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: aclarke
 */

#include "OutputFormat.h"
#include <io/humble/video/Global.h>

namespace io {
namespace humble {
namespace video {

OutputFormat::OutputFormat()
{
  mFormat = 0;
}

OutputFormat::~OutputFormat()
{
  mFormat = 0;
}
OutputFormat*
OutputFormat::make(AVOutputFormat* format)
{
  OutputFormat* retval = 0;
  if (format)
  {
    retval = make();
    retval->mFormat = format;
  }
  return retval;
}

OutputFormat*
OutputFormat::guessFormat(const char * shortName, const char *filename,
    const char *mimeType)
{
  // This is an entry static into this class, so we
  // ensure we initialize FFmpeg, if not already inited.
  Global::init();
  OutputFormat *retval = 0;
  AVOutputFormat* format = av_guess_format(shortName, filename, mimeType);
  if (format)
    retval = OutputFormat::make(format);
  return retval;
}

int32_t
OutputFormat::getNumFormats()
{
  Global::init();
  int i = 0;
  for(AVOutputFormat* f = 0;
  (f = av_oformat_next(f))!=0;
  ++i)
    ;
  return i;
}

OutputFormat*
OutputFormat::getFormat(int32_t index)
{
  Global::init();
  int i = 0;
  for(AVOutputFormat* f = 0;
  (f = av_oformat_next(f))!=0;
  ++i)
    if (i == index) {
      OutputFormat * retval = OutputFormat::make(f);
      return retval;
    }
  return 0;
}
} /* namespace video */
} /* namespace humble */
} /* namespace io */
