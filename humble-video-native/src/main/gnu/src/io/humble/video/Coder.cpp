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
 * Coder.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#include "Coder.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/KeyValueBagImpl.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPicture.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

using namespace io::humble::ferry;

Coder::Coder(Codec* codec, AVCodecContext* src, bool copySrc) {
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");
  mCodec.reset(codec, true);

  if (!src) {
    mCtx = avcodec_alloc_context3(codec->getCtx());
    if (!mCtx)
      throw HumbleRuntimeError("could not allocate coder context");
    mCtx->codec = codec->getCtx();
  } else if (copySrc) {
    // create again for the copy
    mCtx = avcodec_alloc_context3(0);
    if (!mCtx)
      throw HumbleRuntimeError("could not allocate coder context");
    // now copy the codecs.
    if (avcodec_copy_context(mCtx, src) < 0)
      throw HumbleRuntimeError("Could not copy source context");

  } else {
    // just wrap the context
    mCtx = src;
  }
  // set fields we override/use
  mCtx->refcounted_frames = 1;
  mCtx->get_buffer2 = Coder::getBuffer;
  mCtx->opaque = this;

  mState = STATE_INITED;

  VS_LOG_TRACE("Created coder");

}

Coder::~Coder() {
  (void) avcodec_close(mCtx);
  if (mCtx->extradata)
    av_freep(&mCtx->extradata);
  av_freep(&mCtx);
}

void
Coder::open(KeyValueBag* inputOptions, KeyValueBag* aUnsetOptions) {
  int32_t retval = -1;
  AVDictionary* tmp=0;
  try {
      // Time to set options
    if (inputOptions) {
      KeyValueBagImpl* options = dynamic_cast<KeyValueBagImpl*>(inputOptions);
      // make a copy of the data returned.
      av_dict_copy(&tmp, options->getDictionary(), 0);
    }

    RefPointer<Codec> codec = getCodec();
    retval = avcodec_open2(mCtx, codec->getCtx(), &tmp);

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


Rational*
Coder::getTimeBase() {
  if (!mTimebase || mTimebase->getNumerator() != mCtx->time_base.num || mTimebase->getDenominator() != mCtx->time_base.den)
    mTimebase = Rational::make(mCtx->time_base.num, mCtx->time_base.den);
  return mTimebase.get();
}

void
Coder::setTimeBase(Rational* newTimeBase) {
  if (!newTimeBase)
    throw HumbleInvalidArgument("no timebase passed in");
  if (mState != STATE_INITED)
    throw HumbleRuntimeError("can only setTimeBase on Decoder before open() is called.");
  mTimebase.reset(newTimeBase, true);
  mCtx->time_base.num = newTimeBase->getNumerator();
  mCtx->time_base.den = newTimeBase->getDenominator();
}

int
Coder::getBuffer(struct AVCodecContext *s, AVFrame *frame, int flags) {
  Coder* coder = static_cast<Coder*>(s->opaque);
  if (!coder)
    return avcodec_default_get_buffer2(s, frame, flags);

  if (!(coder->mCodec->getCapabilities() & Codec::CAP_DR1))
    return avcodec_default_get_buffer2(s, frame, flags);

  return coder->prepareFrame(frame, flags);
}

int32_t
Coder::getFrameSize() {
  int32_t retval = (int32_t) getPropertyAsLong("frame_size");
  if (retval < 0)
    return retval;
  RefPointer<Codec> codec = getCodec();
  if (codec->getType() == MediaDescriptor::MEDIA_AUDIO)
  {
    if (retval <= 1)
    {
      // Rats; some PCM encoders give a frame size of 1, which is too
      //small.  We pick a more sensible value.
      retval = 576;
    }
  }
  return retval;
}

void
Coder::ensurePictureParamsMatch(MediaPicture* pict)
{
  if (!pict)
    return;

  if (getWidth() != pict->getWidth())
    VS_THROW(HumbleInvalidArgument("width on picture does not match what coder expects"));

  if (getHeight() != pict->getHeight())
    VS_THROW(HumbleInvalidArgument("height on picture does not match what coder expects"));

  if (getPixelFormat() != pict->getFormat())
    VS_THROW(HumbleInvalidArgument("Pixel format on picture does not match what coder expects"));

}

void
Coder::ensureAudioParamsMatch(MediaAudio* audio)
{
  if (!audio)
    return;

  if (getChannels() != audio->getChannels())
    VS_THROW(HumbleInvalidArgument("audio channels does not match what coder expects"));

  if (getSampleRate() != audio->getSampleRate())
    VS_THROW(HumbleInvalidArgument("audio sample rate does not match what coder expects"));

  if (getSampleFormat() != audio->getFormat())
    VS_THROW(HumbleInvalidArgument("audio sample format does not match what coder expects"));

}

int32_t
Coder::getFlags()
{
  return mCtx->flags;
}
int32_t
Coder::getFlag(Flag flag) {
  return mCtx->flags & flag;
}
int32_t
Coder::getFlags2()
{
  return mCtx->flags2;
}
int32_t
Coder::getFlag2(Flag2 flag) {
  return mCtx->flags2 & flag;
}
void
Coder::setFlags(int32_t val)
{
  if (getState() != STATE_INITED)
    VS_THROW(HumbleInvalidArgument("Cannot set flags after coder is opened"));

  mCtx->flags = val;
}
void
Coder::setFlag(Flag flag, bool value)
{
  if (getState() != STATE_INITED)
    VS_THROW(HumbleInvalidArgument("Cannot set flags after coder is opened"));

  if (value)
  {
    mCtx->flags |= flag;
  }
  else
  {
    mCtx->flags &= (~flag);
  }
}
void
Coder::setFlags2(int32_t val)
{
  if (getState() != STATE_INITED)
    VS_THROW(HumbleInvalidArgument("Cannot set flags after coder is opened"));

  mCtx->flags2 = val;
}
void
Coder::setFlag2(Flag2 flag, bool value)
{
  if (getState() != STATE_INITED)
    VS_THROW(HumbleInvalidArgument("Cannot set flags after coder is opened"));

  if (value)
  {
    mCtx->flags2 |= flag;
  }
  else
  {
    mCtx->flags2 &= (~flag);
  }
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
