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
#include <io/humble/video/KeyValueBagImpl.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Decoder::Decoder() {
  mState = STATE_INITED;
  mCtx = 0;
  VS_LOG_TRACE("Created decoder");
}

void
Decoder::open(KeyValueBag* inputOptions, KeyValueBag* aUnsetOptions) {
  int32_t retval = -1;
  AVDictionary* tmp=0;
  try {
      // Time to set options
    if (inputOptions) {
      KeyValueBagImpl* options = dynamic_cast<KeyValueBagImpl*>(inputOptions);
      // make a copy of the data returned.
      av_dict_copy(&tmp, options->getDictionary(), 0);
    }


    retval = avcodec_open2(mCtx, 0, &tmp);

    if (retval < 0)
    {
      mState = STATE_ERROR;
      throw HumbleRuntimeError("could not open codec");
    }
    mState  = STATE_OPENED;

    if (aUnsetOptions)
    {
      KeyValueBagImpl* unsetOptions = dynamic_cast<KeyValueBagImpl*>(aUnsetOptions);
      unsetOptions->copy(tmp);
    }
  } catch (std::exception & e) {
    if (tmp)
      av_dict_free(&tmp);
    tmp = 0;
    throw e;
  }
  if (tmp)
    av_dict_free(&tmp);
}

void
Decoder::flush() {
  if (mState != STATE_OPENED)
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

Decoder::~Decoder() {
  (void) avcodec_close(mCtx);
  av_free(mCtx);
}

Rational*
Decoder::getTimeBase() {
  if (!mTimebase || mTimebase->getNumerator() != mCtx->time_base.num || mTimebase->getDenominator() != mCtx->time_base.den)
    mTimebase = Rational::make(mCtx->time_base.num, mCtx->time_base.den);
  return mTimebase.get();
}

void
Decoder::setTimeBase(Rational* newTimeBase) {
  if (!newTimeBase)
    throw HumbleInvalidArgument("no timebase passed in");
  if (mState != STATE_INITED)
    throw HumbleRuntimeError("can only setTimeBase on Decoder before open() is called.");
  mTimebase.reset(newTimeBase, true);
  mCtx->time_base.num = newTimeBase->getNumerator();
  mCtx->time_base.den = newTimeBase->getDenominator();
}
Decoder*
Decoder::make(Codec* codec)
{
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canDecode())
    throw HumbleInvalidArgument("passed in codec cannot decode");

  RefPointer<Decoder> retval = make();
  retval->mCtx = avcodec_alloc_context3(codec->getCtx());
  if (!retval->mCtx)
    throw HumbleRuntimeError("could not allocate decoder context");

  return retval.get();
}

Decoder*
Decoder::make(Decoder* src)
{
  if (!src)
    throw HumbleInvalidArgument("no Decoder to copy");

  return make(src->mCtx);
}

Decoder*
Decoder::make(const AVCodecContext* src) {
  if (!src)
    throw HumbleInvalidArgument("no Decoder to copy");
  if (!src->codec)
    throw HumbleRuntimeError("No codec set on coder???");

  RefPointer<Decoder> retval = make();
  retval->mCtx = avcodec_alloc_context3(0);
  if (!retval->mCtx)
    throw HumbleRuntimeError("could not allocate decoder context");

  // now copy the codecs.
  if (avcodec_copy_context(retval->mCtx, src) < 0)
    throw HumbleRuntimeError("Could not copy source context");
  return retval.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
