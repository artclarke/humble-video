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
 * Decoder.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#include "Decoder.h"
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/IndexEntryImpl.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Decoder::Decoder(Codec* codec) : Coder(codec) {
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canDecode())
    throw HumbleInvalidArgument("passed in codec cannot decode");
  VS_LOG_TRACE("Created decoder");
}


Decoder::~Decoder() {

}

void
Decoder::flush() {
  if (getState() != STATE_OPENED)
    throw HumbleRuntimeError("Attempt to flush Decoder when not opened");
  avcodec_flush_buffers(mCtx);
}

int32_t
Decoder::decodeAudio(MediaAudio* output, MediaPacket* packet,
    int32_t byteOffset) {
  (void) output;
  (void) packet;
  (void) byteOffset;
  return -1;
}

int32_t
Decoder::decodeVideo(MediaPicture* output, MediaPacket* packet,
    int32_t byteOffset) {
  (void) output;
  (void) packet;
  (void) byteOffset;
  return -1;
}

int32_t
Decoder::decodeSubtitle(MediaSubtitle* output, MediaPacket* packet,
    int32_t byteOffset) {
  (void) output;
  (void) packet;
  (void) byteOffset;
  return -1;
}

Decoder*
Decoder::make(Codec* codec)
{
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canDecode())
    throw HumbleInvalidArgument("passed in codec cannot decode");

  RefPointer<Decoder> retval;
  // new Decoder DOES NOT increment refcount but the reset should catch it.
  retval.reset(new Decoder(codec), true);
  return retval.get();
}

Decoder*
Decoder::make(Decoder* src)
{
  if (!src)
    throw HumbleInvalidArgument("no Decoder to copy");

  RefPointer<Codec> codec = src->getCodec();
  return make(codec.value(), src->mCtx);
}

Decoder*
Decoder::make(Codec* codec, const AVCodecContext* src) {
  if (!src)
    throw HumbleInvalidArgument("no Decoder to copy");
  if (!src->codec_id)
    throw HumbleRuntimeError("No codec set on coder???");

  RefPointer<Decoder> retval = make(codec);
  // free the mCtx
  (void) avcodec_close(retval->mCtx);
  av_free(retval->mCtx);
  retval->mCtx = avcodec_alloc_context3(0);
  // create again for the copy

  // now copy the codecs.
  if (avcodec_copy_context(retval->mCtx, src) < 0)
    throw HumbleRuntimeError("Could not copy source context");
  return retval.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
