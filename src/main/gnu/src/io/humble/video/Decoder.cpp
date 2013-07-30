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
#include <io/humble/video/Error.h>
#include <io/humble/video/IndexEntryImpl.h>
#include <io/humble/video/MediaAudioImpl.h>
#include <io/humble/video/MediaPictureImpl.h>
#include <io/humble/video/MediaPacketImpl.h>
#include <io/humble/video/MediaSubtitleImpl.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Decoder::Decoder(Codec* codec, const AVCodecContext* src) : Coder(codec, src) {
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
Decoder::getFrameSize() {
  int32_t retval = mCtx->frame_size;
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
Decoder::ensureAudioParamsMatch(MediaAudio* audio)
{
  if (!audio)
    VS_THROW(HumbleInvalidArgument("null audio passed to decoder"));

  if (getChannels() != audio->getChannels())
    VS_THROW(HumbleInvalidArgument("audio channels does not match what decoder expects"));

  if (getSampleRate() != audio->getSampleRate())
    VS_THROW(HumbleInvalidArgument("audio sample rate does not match what decoder expects"));

  if (getSampleFormat() != audio->getFormat())
    VS_THROW(HumbleInvalidArgument("audio sample format does not match what decoder expects"));

}

int
Decoder::prepareFrame(AVFrame* frame, int flags) {
  if (!mCachedMedia)
    return Coder::prepareFrame(frame, flags);

  switch (getCodecType()) {
  case MediaDescriptor::MEDIA_AUDIO: {
    MediaAudioImpl* audio = dynamic_cast<MediaAudioImpl*>(mCachedMedia.value());
    if (!audio ||
        audio->getSampleRate() != frame->sample_rate ||
        audio->getChannelLayout() != frame->channel_layout ||
        audio->getFormat() != frame->format)
      return Coder::prepareFrame(frame, flags);
    av_frame_unref(frame);
    // reuse our audio frame.
    av_frame_ref(frame, audio->getCtx());
  }
  break;
  case MediaDescriptor::MEDIA_VIDEO: {
    MediaPictureImpl* pict = dynamic_cast<MediaPictureImpl*>(mCachedMedia.value());
    if (!pict ||
        pict->getWidth() != frame->width ||
        pict->getHeight() != frame->height ||
        pict->getFormat() != frame->format)
      return Coder::prepareFrame(frame, flags);
    av_frame_unref(frame);
    // reuse our audio frame.
    av_frame_ref(frame, pict->getCtx());
  }
  break;
  default:
    VS_LOG_ERROR("Got unknown codec type to allocate for");
    break;
  }
  return 0;
}

int32_t
Decoder::decodeAudio(MediaAudio* aOutput, MediaPacket* aPacket,
    int32_t byteOffset) {
  MediaAudioImpl* output = dynamic_cast<MediaAudioImpl*>(aOutput);
  MediaPacketImpl* packet = dynamic_cast<MediaPacketImpl*>(aPacket);

  if (byteOffset < 0) {
    VS_THROW(HumbleInvalidArgument("byteOffset must be >= 0"));
  }
  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to decodeAudio, but Decoder is not opened"));

  // let's check the audio parameters.
  ensureAudioParamsMatch(aOutput);

  if (packet) {
    if (!packet->isComplete()) {
      VS_THROW(HumbleRuntimeError("Passed in a non-null but not complete packet; this is an error"));
    }
    if (byteOffset >= packet->getSize()) {
      VS_THROW(HumbleRuntimeError("Byteoffset is greater than total length of data in packet"));
    }
  } else {
    if (byteOffset > 0) {
      VS_LOG_WARN("Passing null packet with a non zero byte offset makes no sense");
    }
  }


  // arguments now confirmed; let's do a decode
  int32_t retval = -1;
  int got_frame = 0;

  // let's get the ffmpeg structures
  AVPacket* inPkt = packet ? packet->getCtx() : 0;

  // now we're going to copy the inPkt to do the byte-offset stuff. This
  // will copy by reference.
  AVPacket tmp;
  AVPacket* pkt = &tmp;
  av_init_packet(pkt);
  if (inPkt) {
    // copy in the actual packet
    av_copy_packet(pkt, inPkt);
    // now, adjust the data and size pointers
    pkt->data = pkt->data + byteOffset;
    pkt->size = pkt->size - byteOffset;
  } else {
    pkt->data = 0;
    pkt->size = 0;
  }
  // 'empty' out the input samples
  output->setComplete(false);

  AVFrame *frame = av_frame_alloc();
  if (!frame)
    VS_THROW(HumbleBadAlloc());
  /** DO NOT THROW EXCEPTIONS **/
  // create a frame to decode into, so that FFmpeg doesn't get knickers
  // in a twist re: allocation; but we will attempt to re-use our output
  // frame by setting a call back that Coder::getBuffer2 will use.

  mCachedMedia.reset(output, true);
  // try out decode
  retval = avcodec_decode_audio4(mCtx, frame, &got_frame, pkt);
  // always free the packet so that we don't have an exception make us leak it.
  av_free_packet(pkt);
  if (got_frame) {
    // copy the output frame to our frame
    output->copy(frame, true);
  }
  // release the temporary reference
  mCachedMedia = 0;
  av_frame_unref(frame);
  av_freep(&frame);
  /** END DO NOT THROW EXCEPTIONS **/
  if (retval < 0) {
    // let's make an error
    RefPointer<Error> error = Error::make(retval);
    VS_THROW(HumbleRuntimeError::make("Error while decoding: %s", error ? error->getDescription() : ""));
  }
  return retval;
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
  retval.reset(new Decoder(codec, 0), true);
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

  RefPointer<Decoder> retval;
  // new Decoder DOES NOT increment refcount but the reset should catch it.
  retval.reset(new Decoder(codec, src), true);
  return retval.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
