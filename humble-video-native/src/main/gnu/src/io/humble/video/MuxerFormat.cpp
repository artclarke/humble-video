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

#include "MuxerFormat.h"
#include <io/humble/video/Global.h>

namespace io {
namespace humble {
namespace video {

MuxerFormat::MuxerFormat()
{
  mFormat = 0;
}

MuxerFormat::~MuxerFormat()
{
  mFormat = 0;
}
MuxerFormat*
MuxerFormat::make(AVOutputFormat* format)
{
  MuxerFormat* retval = 0;
  if (format)
  {
    retval = make();
    retval->mFormat = format;
  }
  return retval;
}

MuxerFormat*
MuxerFormat::guessFormat(const char * shortName, const char *filename,
    const char *mimeType)
{
  // This is an entry static into this class, so we
  // ensure we initialize FFmpeg, if not already inited.
  Global::init();
  MuxerFormat *retval = 0;
  AVOutputFormat* format = av_guess_format(shortName, filename, mimeType);
  if (format)
    retval = MuxerFormat::make(format);
  return retval;
}

int32_t
MuxerFormat::getNumFormats()
{
  Global::init();
  int i = 0;
  void * iterator=0;
  for(const AVOutputFormat* f = 0;
  (f = av_muxer_iterate(&iterator))!=0;
  ++i)
    ;
  return i;
}

MuxerFormat*
MuxerFormat::getFormat(int32_t index)
{
  Global::init();
  int i = 0;
  void * iterator=0;

  for(const AVOutputFormat* f = 0;
  (f = av_muxer_iterate(&iterator))!=0;
  ++i)
    if (i == index) {
      MuxerFormat * retval = MuxerFormat::make((AVOutputFormat*)f);
      return retval;
    }
  return 0;
}

AVOutputFormat*
MuxerFormat::getCtx()
{
  return mFormat;
}

uint32_t
MuxerFormat::getBestCodecTag(Codec::ID id) {
  int32_t n = getNumSupportedCodecs();
  uint32_t r = 0;
  for (int32_t i = 0; i < n; i++) {
    Codec::ID sId = getSupportedCodecId(i);
    if (sId == id) {
      r = getSupportedCodecTag(i);
      break;
    }
  }
  return r;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
