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
#include <io/humble/video/MediaPacketImpl.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace
video {

#if 0
int32_t
Encoder::acquire()
{
  VS_LOG_DEBUG("encoder acquire: %p", this);
  return RefCounted::acquire();
}
int32_t
Encoder::release()
{
  VS_LOG_DEBUG("encoder release: %p", this);
  return RefCounted::release();
}
#endif // 0

Encoder::Encoder(Codec* codec, AVCodecContext* src, bool copySrc) : Coder(codec, src, copySrc) {
  if (!codec)
    throw HumbleInvalidArgument("no codec passed in");

  if (!codec->canEncode())
    throw HumbleInvalidArgument("passed in codec cannot encode");

  mNumDroppedFrames = 0;
  mLastPtsEncoded = Global::NO_PTS;

  VS_LOG_TRACE("Created encoder");
}

Encoder::~Encoder() {
}

Encoder*
Encoder::make(Codec* codec)
{
  RefPointer<Encoder> r;
  if (!codec) {
    VS_THROW(HumbleInvalidArgument("no codec passed in"));
  }

  if (!codec->canEncode()) {
    VS_THROW(HumbleInvalidArgument("passed in codec cannot encode"));
  }

  r.reset(new Encoder(codec, 0, false), true);

  return r.get();
}

Encoder*
Encoder::make(Coder* src)
{
  if (!src) {
    VS_THROW(HumbleInvalidArgument("no coder to copy"));
  }

  RefPointer<Encoder> r;
  RefPointer<Codec> c = src->getCodec();
  if (!c) {
    VS_THROW(HumbleRuntimeError("coder has no codec"));
  }
  if (!c->canEncode()) {
    // this codec cannot encode, so we try to find a new codec that can
    // of the same type.
    Codec::ID id = c->getID();
    c = Codec::findEncodingCodec(id);
    if (!c) {
      RefPointer<CodecDescriptor> cd = CodecDescriptor::make(id);
      VS_THROW(HumbleRuntimeError::make("cannot find encoder for id: %s", cd ? cd->getName() : "unknown"));
    }
  }
  r.reset(new Encoder(c.value(), src->getCodecCtx(), true), true);
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

void
Encoder::open(KeyValueBag * inputOptions, KeyValueBag* unsetOptions) {
  Coder::open(inputOptions, unsetOptions);
  // now check to see if we require an audio resampler.
  RefPointer<Codec> codec = getCodec();
  if (codec->getType() == MediaDescriptor::MEDIA_AUDIO &&
      !(codec->getCapabilities() & Codec::CAP_VARIABLE_FRAME_SIZE)) {
    // yes, we do. Let's create one. This looks odd, as it's converting
    // the same params into the same params, but it's really just being
    // used as an audio buffer for us.
    mAResampler = MediaAudioResampler::make(
        getChannelLayout(),
        getSampleRate(),
        getSampleFormat(),
        getChannelLayout(),
        getSampleRate(),
        getSampleFormat()
        );
    mAResampler->open();
  }
}
void
Encoder::encodeVideo(MediaPacket* aOutput, MediaPicture* frame) {
  MediaPacketImpl* output = dynamic_cast<MediaPacketImpl*>(aOutput);

  if (getState() != STATE_OPENED) {
    VS_THROW(HumbleRuntimeError("Cannot encode with unopened encoder"));
  }
  if (getCodecType() != MediaDescriptor::MEDIA_VIDEO) {
    VS_THROW(HumbleRuntimeError("Attempting to encode picture on non-video encoder"));
  }
  if (!output) {
    VS_THROW(HumbleInvalidArgument("output cannot be null"));
  }
  // now reset the packet so we allocate new memory (because encoders sometimes change packet sizes).
  output->reset(0);

  // let's check the picture parameters.
  ensurePictureParamsMatch(frame);

  if (frame && !frame->isComplete()) {
    VS_THROW(HumbleInvalidArgument("Can only pass complete media to encode"));
  }

  if (frame && frame->getPts() == Global::NO_PTS) {
    VS_THROW(HumbleInvalidArgument("Passed in media must have a valid time stamp"));
  }

  /**
   * Ah, this is naive. Here's what we actually need to do:
   *
   * 1) Copy the video frame into a new frame since some codecs will
   * need byte aligned frames (and if the frame came from a Java user, we
   * can't be guaranteed it'll be aligned on the right boundaries).
   *
   * 2) check time stamps and drop frames when needed if the time
   * base of the input frame cannot be converted to the encoder
   * time base without rounding.
   *
   */
  AVFrame* in = frame ? frame->getCtx() : 0;
  AVPacket* out = output->getCtx();

  RefPointer<Rational> coderTb = this->getTimeBase();

  if (in) {
    RefPointer<Rational> frameTb = frame->getTimeBase();
    bool dropFrame = false;

    int64_t inTs = coderTb->rescale(in->pts, frameTb.value());
    if (mLastPtsEncoded != Global::NO_PTS) {
      if (inTs < mLastPtsEncoded) {
        VS_LOG_DEBUG(
            "Dropping frame with timestamp %lld (if coder supports higher time-base use that instead)",
            frame->getPts());
        dropFrame = true;
      }
    } else {
      if (!dropFrame)
        mLastPtsEncoded = inTs;
    }
  }
  int got_frame = 0;

  // set the packet to be the max size if can be
  output->setComplete(false, 0);
  if (out->buf && out->data)
    out->size = out->buf->size;
  else
    out->size = 0;

  int oldStreamIndex = output->getStreamIndex();
  int e = avcodec_encode_video2(getCodecCtx(), out, in, &got_frame);
  // some codec erroneously set stream_index, but our encoders are always
  // muxer independent. we fix that here.
  output->setStreamIndex(oldStreamIndex);
  if (got_frame) {
    output->setCoder(this);
    output->setTimeBase(coderTb.value());
    output->setComplete(out->size > 0, out->size);
  }
  FfmpegException::check(e, "could not encode video ");
}

void
Encoder::encode(MediaPacket* output, MediaSampled* media) {
  MediaDescriptor::Type type = getCodecType();
  switch(type) {
  case MediaDescriptor::MEDIA_AUDIO: {
    MediaAudio* audio = dynamic_cast<MediaAudio*>(media);
    if (!audio && media)
      VS_THROW(HumbleInvalidArgument("passed non-audio Media to an audio encoder"));
    encodeAudio(output, audio);
  }
  break;
  case MediaDescriptor::MEDIA_VIDEO: {
    MediaPicture* picture = dynamic_cast<MediaPicture*>(media);
    if (!picture && media)
      VS_THROW(HumbleInvalidArgument("passed non-video Media to an video encoder"));
    encodeVideo(output, picture);
  }
  break;
  default:
    VS_THROW(HumbleInvalidArgument("passed a media type that is not compatible with this encoder"));
  }
}
void
Encoder::encodeAudio(MediaPacket* aOutput, MediaAudio* samples) {
  MediaPacketImpl* output = dynamic_cast<MediaPacketImpl*>(aOutput);

  if (getState() != STATE_OPENED) {
    VS_THROW(HumbleRuntimeError("Cannot encode with unopened encoder"));
  }
  if (getCodecType() != MediaDescriptor::MEDIA_AUDIO) {
    VS_THROW(HumbleRuntimeError("Attempting to encode audio on non-audio encoder"));
  }
  if (!output) {
    VS_THROW(HumbleInvalidArgument("output cannot be null"));
  }
  // now reset the packet so we allocate new memory (because encoders sometimes change packet sizes).
  output->reset(0);

  // let's check the audio parameters.
  ensureAudioParamsMatch(samples);

  if (samples && !samples->isComplete()) {
    VS_THROW(HumbleInvalidArgument("Can only pass complete media to encode"));
  }

  RefPointer<Codec> codec = getCodec();
  RefPointer<MediaAudio> inputAudio;

  inputAudio.reset(samples, true);
  if (!(codec->getCapabilities() & Codec::CAP_VARIABLE_FRAME_SIZE)) {
    // this codec requires that the right number of audio samples
    // gets passed in each call.
    if (!mResampledAudio) {
      if (samples)
        mResampledAudio = MediaAudio::make(getFrameSize(),
            samples->getSampleRate(), samples->getChannels(),
            samples->getChannelLayout(), samples->getFormat());
    }
    if (mResampledAudio) {
      mAResampler->resample(mResampledAudio.value(), samples);
      if (mResampledAudio->isComplete())
        inputAudio = mResampledAudio.get();
      else
        inputAudio = 0;
    }
  }
  RefPointer<Rational> coderTb = this->getTimeBase();

  AVFrame* in = inputAudio ? inputAudio->getCtx() : 0;
  AVPacket* out = output->getCtx();

  int got_frame = 0;
  int oldStreamIndex = output->getStreamIndex();
  int e = avcodec_encode_audio2(getCodecCtx(), out, in, &got_frame);
  // some codec erroneously set stream_index, but our encoders are always
  // muxer independent. we fix that here.
  output->setStreamIndex(oldStreamIndex);
  if (got_frame) {
    output->setCoder(this);
    output->setTimeBase(coderTb.value());
    output->setComplete(true, out->size);
  } else {
    output->setComplete(false, 0);
  }

  FfmpegException::check(e, "could not encode audio ");
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
