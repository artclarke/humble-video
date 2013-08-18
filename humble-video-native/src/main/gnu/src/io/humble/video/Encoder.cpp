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
 * Encoder.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#include "Encoder.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

Encoder::Encoder(Codec* codec, AVCodecContext* src, bool copySrc) : Coder(codec, src, copySrc) {
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canEncode())
    throw HumbleInvalidArgument("passed in codec cannot encode");
  VS_LOG_TRACE("Created encoder");

}

Encoder::~Encoder() {
}

Encoder*
Encoder::make(Codec* codec)
{
  RefPointer<Encoder> r;
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canEncode())
    throw HumbleInvalidArgument("passed in codec cannot encode");

  r.reset(new Encoder(codec, 0, false), true);

  return r.get();
}

Encoder*
Encoder::make(Encoder* src)
{
  if (!src)
    throw HumbleInvalidArgument("no Encoder to copy");

  RefPointer<Encoder> r;
  RefPointer<Codec> c = src->getCodec();
  r.reset(new Encoder(c.value(), src->getCodecCtx(), false), true);
  return r.get();
}

Encoder*
Encoder::make(Codec* codec, AVCodecContext* src) {
  if (!src)
    throw HumbleInvalidArgument("no Encoder to copy");
  if (!src->codec_id)
    throw HumbleRuntimeError("No codec set on coder???");

  RefPointer<Encoder> retval;
  // new Encoder DOES NOT increment refcount but the reset should catch it.
  retval.reset(new Encoder(codec, src, false), true);
  return retval.get();
}

int32_t
Encoder::encodeVideo(MediaPacket* output, MediaPicture* frame,
    int32_t suggestedBufferSize) {
  (void) output;
  (void) frame;
  (void) suggestedBufferSize;
  VS_THROW(HumbleRuntimeError("not implemented"));
  return 0;
}

int32_t
Encoder::encodeAudio(MediaPacket* output, MediaAudio* samples,
    int32_t sampleToStartFrom) {
  (void) output;
  (void) samples;
  (void) sampleToStartFrom;
  VS_THROW(HumbleRuntimeError("not implemented"));
  return 0;
}

int32_t
Encoder::encodeSubtitle(MediaPacket* output, MediaSubtitle* subtitles) {
  (void) output;
  (void) subtitles;
  VS_THROW(HumbleRuntimeError("not implemented"));
  return 0;
}



} /* namespace video */
} /* namespace humble */
} /* namespace io */
