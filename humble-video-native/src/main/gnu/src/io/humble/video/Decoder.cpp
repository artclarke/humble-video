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
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/IndexEntryImpl.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPictureImpl.h>
#include <io/humble/video/MediaPacketImpl.h>
#include <io/humble/video/MediaSubtitleImpl.h>

VS_LOG_SETUP(VS_CPP_PACKAGE.Decoder);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Decoder::Decoder(const AVCodec* codec, const AVCodecParameters* src) : Coder(codec, src) {
  mSamplesSinceLastTimeStampDiscontinuity = 0;
  mAudioDiscontinuityStartingTimeStamp = Global::NO_PTS;
  VS_LOG_TRACE("Created: %p", this);
}


Decoder::~Decoder() {
  VS_LOG_TRACE("Destroyed: %p", this);
}

int64_t
Decoder::rebase(int64_t ts, MediaPacket* packet) {
  if (!packet)
    return ts;

  RefPointer<Rational> srcTs = packet->getTimeBase();
  RefPointer<Rational> dstTs = this->getTimeBase();

  if (!srcTs
      || !dstTs
      || !srcTs->getDenominator()
      || !dstTs->getDenominator())
    return ts;
  return dstTs->rescale(ts, srcTs.value());
}

Decoder*
Decoder::make(Codec* codec)
{
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canDecode())
    throw HumbleInvalidArgument("passed in codec cannot decode");
  return make(codec->getCtx(), 0);
}

Decoder*
Decoder::make(Coder* src)
{
  if (!src)
    throw HumbleInvalidArgument("no coder to copy");

  RefPointer<Codec> c = src->getCodec();
  RefPointer<Decoder> retval = 0;
  if (!c) {
    VS_THROW(HumbleRuntimeError("coder has no codec"));
  }
  if (!c->canDecode()) {
    // this codec cannot decode, so we try to find a new codec that can
    // of the same type.
    Codec::ID id = c->getID();
    c = Codec::findDecodingCodec(id);
    if (!c) {
      RefPointer<CodecDescriptor> cd = CodecDescriptor::make(id);
      VS_THROW(HumbleRuntimeError::make("cannot find decoder for id: %s", cd ? cd->getName() : "unknown"));
    }
    retval = make(c.value());
  } else {
    // this codec can decode as well... let's make some parameters
    AVCodecParameters* pars = avcodec_parameters_alloc();
    if (!pars)
      VS_THROW(HumbleBadAlloc());
    if (avcodec_parameters_from_context(pars, src->getCodecCtx()) < 0)
    {
      avcodec_parameters_free(&pars);
      VS_THROW(HumbleRuntimeError::make("cannot copy parameters from codec"));
    }
    retval = make(c->getCtx(), pars);
    avcodec_parameters_free(&pars);
  }
  return retval.get();
}

Decoder*
Decoder::make(const AVCodec* codec, const AVCodecParameters *src) {
  RefPointer<Decoder> retval;
  // new Decoder DOES NOT increment refcount but the reset should catch it.
  retval.reset(new Decoder(codec, src), true);
  return retval.get();

}

ProcessorResult
Decoder::sendPacket(MediaPacket* media) {
  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to send(Media), but Decoder is not opened"));

  MediaPacketImpl* packet = dynamic_cast<MediaPacketImpl*>(media);
  if (media && !packet)
    VS_THROW(HumbleInvalidArgument("Decoders require MediaEncoded objects"));

  if (packet && !packet->isComplete()) {
    VS_THROW(HumbleRuntimeError("Passed in a non-null but not complete packet; this is an error"));
  }

  const AVPacket* p = packet ?  packet->getCtx() : 0;
  int e = avcodec_send_packet(getCodecCtx(), p);
#ifdef VS_DEBUG
  char desc[256]; *desc = 0;
  if (packet) packet->logMetadata(desc, sizeof(desc));
  VS_LOG_TRACE("send Decoder@%p[media:%s]:%" PRIi64,
               this,
               desc,
               (int64_t)e);
#endif
  if (e != AVERROR(EAGAIN) && e!= AVERROR_EOF)
    FfmpegException::check(e, "Error on Decoder.send(Media)");

  return (ProcessorResult) e;
}

ProcessorResult
Decoder::receiveRaw(MediaRaw* outputMedia) {

  MediaRaw* output = dynamic_cast<MediaRaw*>(outputMedia);
  if (!output)
    VS_THROW(HumbleInvalidArgument("receive(Media) needs non null media"));

  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to receive(Media), but Decoder is not opened"));

  // First, check that the output type is correct
  MediaDescriptor::Type type = getCodecType();

  switch(type) {
  case MediaDescriptor::MEDIA_AUDIO: {
    MediaAudio* audio = dynamic_cast<MediaAudio*>(output);
    if (!audio)
      VS_THROW(HumbleInvalidArgument("passed non-audio Media to an audio decoder"));

    // let's check the audio parameters.
    ensureAudioParamsMatch(audio);
  }
  break;
  case MediaDescriptor::MEDIA_VIDEO: {
    MediaPicture* picture = dynamic_cast<MediaPicture*>(output);
    if (!picture)
      VS_THROW(HumbleInvalidArgument("passed non-video Media to a video decoder"));
    ensurePictureParamsMatch(picture);
  }
  break;
  case MediaDescriptor::MEDIA_SUBTITLE: {
    MediaSubtitle* subtitle = dynamic_cast<MediaSubtitle*>(output);
    if (!subtitle)
      VS_THROW(HumbleInvalidArgument("passed non-subtitle Media to a subtitle decoder"));
    // TODO: create ensureSubtitleParamsMatch(subtitle)
  }
  break;
  default:
    VS_THROW(HumbleInvalidArgument("passed a media type that is not compatible with this decoder"));
  }
  // now that we've validated the output type,
  AVFrame *frame = av_frame_alloc();
  if (!frame)
    VS_THROW(HumbleBadAlloc());

  // DO NOT THROW EXCEPTIONS FROM HERE UNTIL NEXT MARKER
  int e = avcodec_receive_frame(getCodecCtx(), frame);
  if (e == 0) {
    // we got data
    if (frame->pts == Global::NO_PTS)
      // never allow a video frame without a guessed best effort timestamp.
      frame->pts = frame->best_effort_timestamp;

    // copy the output to our frame
    // release any memory we have
    av_frame_unref(output->getCtx());
    // and copy any data in.
    av_frame_ref(output->getCtx(), frame);
    RefPointer<Rational> timeBase = getTimeBase();
    output->setTimeBase(timeBase.value());
    output->setComplete(true);
  }
#ifdef VS_DEBUG
  char desc[256]; *desc = 0;
  if (output) output->logMetadata(desc, sizeof(desc));
  VS_LOG_TRACE("receive Decoder@%p[media:%s]:%" PRIi64,
               this,
               desc,
               (int64_t)e);
#endif
  av_frame_free(&frame);
  // SAFE TO THROW EXCEPTIONS FROM HERE
  if (e != AVERROR(EAGAIN) && e!= AVERROR_EOF) {
    FfmpegException::check(e, "Error on Decoder.send(Media)");
  }

  return (ProcessorResult) e;
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
