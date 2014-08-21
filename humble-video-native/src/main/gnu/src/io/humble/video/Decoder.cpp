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

Decoder::Decoder(Codec* codec, AVCodecContext* src, bool copy) : Coder(codec, src, copy) {
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canDecode())
    throw HumbleInvalidArgument("passed in codec cannot decode");

  mSamplesSinceLastTimeStampDiscontinuity = 0;
  mAudioDiscontinuityStartingTimeStamp = Global::NO_PTS;

  VS_LOG_TRACE("Created: %p", this);
}


Decoder::~Decoder() {
  VS_LOG_TRACE("Destroyed: %p", this);
}

void
Decoder::flush() {
  if (getState() != STATE_OPENED)
    throw HumbleRuntimeError("Attempt to flush Decoder when not opened");
  avcodec_flush_buffers(getCodecCtx());
}

int
Decoder::prepareFrame(AVFrame* frame, int flags) {
  if (!mCachedMedia)
    return Coder::prepareFrame(frame, flags);

  switch (getCodecType()) {
  case MediaDescriptor::MEDIA_AUDIO: {
    MediaAudio* audio = dynamic_cast<MediaAudio*>(mCachedMedia.value());
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
    // reusing video frames is too tricky given all the alignments; so we
    // just let FFmpeg allocate it.
    // adventurous souls can try changing this later.
    return Coder::prepareFrame(frame, flags);
  }
  break;
  default:
    VS_LOG_ERROR("Got unknown codec type to allocate for");
    break;
  }
  return 0;
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
int32_t
Decoder::decodeAudio(MediaAudio* output, MediaPacket* aPacket,
    int32_t byteOffset) {
  MediaPacketImpl* packet = dynamic_cast<MediaPacketImpl*>(aPacket);

  if (getCodecType() != MediaDescriptor::MEDIA_AUDIO)
    VS_THROW(HumbleRuntimeError("Attempting to decode audio on non-audio decoder"));

  if (byteOffset < 0) {
    VS_THROW(HumbleInvalidArgument("byteOffset must be >= 0"));
  }
  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to decodeAudio, but Decoder is not opened"));

  if (!output)
    VS_THROW(HumbleInvalidArgument("null audio passed to coder"));

  // let's check the audio parameters.
  ensureAudioParamsMatch(output);

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

  int64_t packetTs = packet ? packet->getPts() : Global::NO_PTS;
  if (mAudioDiscontinuityStartingTimeStamp == Global::NO_PTS) {
    if (packetTs != Global::NO_PTS) {
      // rebase packet timestamp to coder's timestamp
      mAudioDiscontinuityStartingTimeStamp = rebase(packetTs, packet);
    } else {
      VS_LOG_DEBUG("Packet had no timestamp, so setting fake timestamp to 0");
      mAudioDiscontinuityStartingTimeStamp = 0;
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
  // reset the frame timestamp so ffmpeg doesn't get confused
  output->setTimeStamp(Global::NO_PTS);
  // try out decode
  retval = avcodec_decode_audio4(getCodecCtx(), frame, &got_frame, pkt);
  // always free the packet so that we don't have an exception make us leak it.
  av_free_packet(pkt);
  if (got_frame) {
    RefPointer<Rational> coderBase = getTimeBase();

    // calculate what we think the new timestamp should be
    int64_t newPts = mAudioDiscontinuityStartingTimeStamp + Rational::rescale(
        mSamplesSinceLastTimeStampDiscontinuity,
        getCodecCtx()->time_base.num, getCodecCtx()->time_base.den,
        1, getSampleRate(), Rational::ROUND_DOWN);

    // if we have a packet
    if (packetTs != Global::NO_PTS) {
      // make sure the packet timestamps and our fake timestamps are not
      // drifting apart by too much
      RefPointer<Rational> packetBase = packet->getTimeBase();
      // convert our calculated timestamp to the packet base and compare
      int64_t rebasedTs = packetBase ? packetBase->rescale(newPts, coderBase.value()) : newPts;
      int64_t delta = rebasedTs - packetTs;
//      VS_LOG_TRACE("packet: %lld; calculated: %lld; delta: %lld; tb (%d/%d); nextPts: %lld; samples: %lld",
//          packetTs,
//          rebasedTs,
//          delta,
//          packetBase->getNumerator(),
//          packetBase->getDenominator(),
//          newPts,
//          mSamplesSinceLastTimeStampDiscontinuity);
      if (delta <= 1 && delta >= -1) {
        // within one tick; keep the original measure of discontinuity start
      } else {
        // drift occurring
        int64_t newNext = coderBase ? coderBase->rescale(packetTs, packetBase.value()) : packetTs;
        VS_LOG_DEBUG("Gap in audio (%lld). Resetting calculated time stamp from %lld to %lld",
            delta,
            mAudioDiscontinuityStartingTimeStamp,
            newNext);
        mAudioDiscontinuityStartingTimeStamp = newNext;
        mSamplesSinceLastTimeStampDiscontinuity = 0;
      }
    }

    // never allow an audio frame without a guessed best effort timestamp.
    if (frame->pts == Global::NO_PTS) {
      // now we set the time stamp to the calculated value. We use the number
      // of samples as the main measure of audio time to reduce audio timestamp
      // drift.
      frame->pts = mAudioDiscontinuityStartingTimeStamp + Rational::rescale(
          mSamplesSinceLastTimeStampDiscontinuity,
          getCodecCtx()->time_base.num, getCodecCtx()->time_base.den,
          1, getSampleRate(), Rational::ROUND_DOWN);;
    }

    // calculate the next time stamp based on the audio samples
    mSamplesSinceLastTimeStampDiscontinuity += frame->nb_samples;

    // copy the output frame to our frame
    output->copy(frame, true);
    RefPointer<Rational> tb = getTimeBase();
    output->setTimeBase(tb.value());
  }
  // release the temporary reference
  mCachedMedia = 0;
  av_frame_unref(frame);
  av_freep(&frame);

#ifdef VS_DEBUG
  char outDescr[256]; *outDescr = 0;
  char inDescr[256]; *inDescr = 0;
  if (aPacket) aPacket->logMetadata(inDescr, sizeof(inDescr));
  if (output) output->logMetadata(outDescr, sizeof(outDescr));
  VS_LOG_TRACE("decodeAudio Decoder@%p[out:%s;in:%s;offset:%lld;decoded:%" PRIi64,
               this,
               outDescr,
               inDescr,
               (int64_t)byteOffset,
               (int64_t)retval);
#endif

  /** END DO NOT THROW EXCEPTIONS **/
  FfmpegException::check(retval, "Error while decoding ");
  return retval;
}

int32_t
Decoder::decodeVideo(MediaPicture* aOutput, MediaPacket* aPacket,
    int32_t byteOffset) {
  MediaPictureImpl* output = dynamic_cast<MediaPictureImpl*>(aOutput);
  MediaPacketImpl* packet = dynamic_cast<MediaPacketImpl*>(aPacket);

  RefPointer<Codec> codec = getCodec();
  if (getCodecType() != MediaDescriptor::MEDIA_VIDEO)
    VS_THROW(HumbleRuntimeError("Attempting to decode video on non-video decoder"));

  if (byteOffset < 0) {
    VS_THROW(HumbleInvalidArgument("byteOffset must be >= 0"));
  }
  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to decodeAudio, but Decoder is not opened"));

  if (!output)
    VS_THROW(HumbleInvalidArgument("null picture passed to coder"));

  // let's check the picture parameters.
  ensurePictureParamsMatch(output);

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
  retval = avcodec_decode_video2(getCodecCtx(), frame, &got_frame, pkt);
  // always free the packet so that we don't have an exception make us leak it.
  av_free_packet(pkt);
  if (got_frame) {
    // never allow a video frame without a guessed best effort timestamp.
    if (frame->pts == Global::NO_PTS)
      frame->pts = frame->best_effort_timestamp;
    // copy the output frame to our frame
    output->copy(frame, true);
    RefPointer<Rational> tb = getTimeBase();
    output->setTimeBase(tb.value());
    mSamplesSinceLastTimeStampDiscontinuity += 1;
  }
  // release the temporary reference
  mCachedMedia = 0;
  av_frame_unref(frame);
  av_freep(&frame);

#ifdef VS_DEBUG
  char outDescr[256]; *outDescr = 0;
  char inDescr[256]; *inDescr = 0;
  if (aPacket) aPacket->logMetadata(inDescr, sizeof(inDescr));
  if (aOutput) aOutput->logMetadata(outDescr, sizeof(outDescr));
  VS_LOG_TRACE("decodeVideo Decoder@%p[out:%s;in:%s;offset:%lld;decoded:%" PRIi64,
               this,
               outDescr,
               inDescr,
               (int64_t)byteOffset,
               (int64_t)retval);
#endif
  /** END DO NOT THROW EXCEPTIONS **/
  FfmpegException::check(retval, "Error while decoding ");
  return retval;
}

int32_t
Decoder::decode(MediaSampled* output, MediaPacket* packet, int32_t offset) {
  MediaDescriptor::Type type = getCodecType();
  switch(type) {
  case MediaDescriptor::MEDIA_AUDIO: {
    MediaAudio* audio = dynamic_cast<MediaAudio*>(output);
    if (!audio && output)
      VS_THROW(HumbleInvalidArgument("passed non-audio Media to an audio decoder"));
    return decodeAudio(audio, packet, offset);
  }
  break;
  case MediaDescriptor::MEDIA_VIDEO: {
    MediaPicture* picture = dynamic_cast<MediaPicture*>(output);
    if (!picture && output)
      VS_THROW(HumbleInvalidArgument("passed non-video Media to an video decoder"));
    return decodeVideo(picture, packet, offset);
  }
  break;
  default:
    VS_THROW(HumbleInvalidArgument("passed a media type that is not compatible with this decoder"));
  }
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
  retval.reset(new Decoder(codec, 0, true), true);
  return retval.get();
}

Decoder*
Decoder::make(Coder* src)
{
  if (!src)
    throw HumbleInvalidArgument("no coder to copy");

  RefPointer<Codec> c = src->getCodec();
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
  }
  return make(c.value(), src->getCodecCtx(), true);
}

Decoder*
Decoder::make(Codec* codec, AVCodecContext* src, bool copy) {
  if (!src)
    throw HumbleInvalidArgument("no Decoder to copy");
  if (!src->codec_id)
    throw HumbleRuntimeError("No codec set on coder???");

  RefPointer<Decoder> retval;
  // new Decoder DOES NOT increment refcount but the reset should catch it.
  retval.reset(new Decoder(codec, src, copy), true);
  return retval.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
