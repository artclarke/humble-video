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
  if (!(codec->getCapabilities() & Codec::CAP_VARIABLE_FRAME_SIZE)) {
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
  // let's check the picture parameters.
  ensurePictureParamsMatch(frame);

  if (frame && !frame->isComplete()) {
    VS_THROW(HumbleInvalidArgument("Can only pass complete media to encode"));
  }

  AVFrame* in = frame ? frame->getCtx() : 0;
  AVPacket* out = output->getCtx();

  int got_frame = 0;
  int e = avcodec_encode_video2(getCodecCtx(), out, in, &got_frame);
  if (got_frame) {
    output->setComplete(true, out->size);
  } else {
    output->setComplete(false, 0);
  }
  FfmpegException::check(e, "could not encode video ");
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
      inputAudio = mResampledAudio.get();
    }
  }

  AVFrame* in = inputAudio ? inputAudio->getCtx() : 0;
  AVPacket* out = output->getCtx();

  int got_frame = 0;
  int e = avcodec_encode_audio2(getCodecCtx(), out, in, &got_frame);
  if (got_frame) {
    output->setComplete(true, out->size);
  } else {
    output->setComplete(false, 0);
  }

  FfmpegException::check(e, "could not encode audio ");
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
