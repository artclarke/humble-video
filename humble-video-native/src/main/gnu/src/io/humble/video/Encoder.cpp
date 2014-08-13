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

VS_LOG_SETUP(VS_CPP_PACKAGE.Encoder);

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

  VS_LOG_TRACE("Created: %p", this);
}

Encoder::~Encoder() {
  VS_LOG_TRACE("Destroyed: %p", this);
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
Encoder::checkOptionsBeforeOpen() {
  // now check to see if we require an audio resampler.
  RefPointer<Codec> codec = getCodec();
  switch (codec->getType()) {
    case MediaDescriptor::MEDIA_AUDIO: {
      int32_t sampleRate = getSampleRate();
      AudioChannel::Layout channelLayout = getChannelLayout();
      AudioFormat::Type format = getSampleFormat();

      // Only do this check if the codec reports more than 0 supported rates or formats.
      int n = codec->getNumSupportedAudioSampleRates();
      int i = 0;

      for(i = 0; i < n; ++i) {
        int32_t s = codec->getSupportedAudioSampleRate(i);
        if (s == sampleRate)
          break;
      }
      if (n > 0 && i == n) {
        VS_THROW(HumbleInvalidArgument::make("Sample rate %ld not supported by encoder.",
            (int32_t)sampleRate
            ));
      }

      n = codec->getNumSupportedAudioChannelLayouts();
      for(i = 0; i < n; ++i) {
        AudioChannel::Layout c = codec->getSupportedAudioChannelLayout(i);
        if (c == channelLayout)
          break;
      }
      if (n > 0 && i == n) {
        VS_THROW(HumbleInvalidArgument::make("Channel layout %ld not supported by encoder.",
            (int32_t)channelLayout
            ));
      }

      n = codec->getNumSupportedAudioFormats();
      for(i = 0; i < n; i++) {
        AudioFormat::Type f = codec->getSupportedAudioFormat(i);
        if (f == format)
          break;
      }
      if (n > 0 && i == n) {
        VS_THROW(HumbleInvalidArgument::make("Audio format %ld not supported by encoder.",
                    (int32_t)format
                    ));
      }
    }
    break;
    case MediaDescriptor::MEDIA_VIDEO: {
//      PixelFormat::Type format = getPixelFormat();
//      RefPointer<Rational> tb = getTimeBase();

    }
    break;
    default:
      break;
  }
}
void
Encoder::open(KeyValueBag * inputOptions, KeyValueBag* unsetOptions) {
  RefPointer<Codec> codec = getCodec();

  Coder::open(inputOptions, unsetOptions);
  switch(codec->getType()) {
    case MediaDescriptor::MEDIA_AUDIO: {
      if (!(codec->getCapabilities() & Codec::CAP_VARIABLE_FRAME_SIZE)) {
        int32_t frameSize = getFrameSize();
        if (frameSize <= 0) {
          setState(STATE_ERROR);
          VS_THROW(HumbleRuntimeError("Codec requires fixed frame size, but does not specify frame size"));
        }
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
      VS_LOG_TRACE("open Encoder@%p[t=AUDIO;sr=%"PRId32";c:%"PRId32";cl:%"PRId32";f=%"PRId32";]",
                   this,
                   (int32_t)getSampleRate(),
                   (int32_t)getChannels(),
                   (int32_t)getChannelLayout(),
                   (int32_t)getSampleFormat());

    }
    break;
    case MediaDescriptor::MEDIA_VIDEO: {
      VS_LOG_TRACE("open Encoder@%p[t=VIDEO;d=%"PRId32"x%"PRId32";f:%"PRId32";]",
                   this,
                   (int32_t)getWidth(),
                   (int32_t)getHeight(),
                   (int32_t)getPixelFormat());
    }
    break;
    default:
      break;
  }
}
void
Encoder::encodeVideo(MediaPacket* aOutput, MediaPicture* aFrame) {
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


  RefPointer<Rational> coderTb = this->getTimeBase();

  // let's check the picture parameters.
  bool dropFrame = false;

  // copy the frame so we can modify meta-data
  MediaPicture* frame = aFrame ? MediaPicture::make(aFrame, false) : 0;

  if (frame) {

    if (!frame->isComplete()) {
      VS_THROW(HumbleInvalidArgument("Can only pass complete media to encode"));
    }

    if (frame->getPts() == Global::NO_PTS) {
      VS_THROW(HumbleInvalidArgument("Passed in media must have a valid time stamp"));
    }

    ensurePictureParamsMatch(frame);

    RefPointer<Rational> frameTb = frame->getTimeBase();
    /**
     * check time stamps and drop frames when needed if the time
     * base of the input frame cannot be converted to the encoder
     * time base without rounding.
     *
     */
    int64_t inTs = coderTb->rescale(frame->getTimeStamp(), frameTb.value(), Rational::ROUND_DOWN);
    if (mLastPtsEncoded != Global::NO_PTS) {
      if (inTs <= mLastPtsEncoded) {
        VS_LOG_DEBUG(
            "Encoder@%p: Dropping frame with timestamp %lld (if coder supports higher time-base use that instead)",
            this,
            frame->getPts());
        dropFrame = true;
      }
    }
    if (!dropFrame) {
        mLastPtsEncoded = inTs;
        frame->setTimeBase(coderTb.value());
        // set the timestamp after the timbase as setTimeBase does a conversion.
        frame->setTimeStamp(inTs);
    }
  }


  AVFrame* in = frame ? frame->getCtx() : 0;
  AVPacket* out = output->getCtx();
  int got_frame = 0;

  // set the packet to be the max size if can be
  output->setComplete(false, 0);
  if (out->buf && out->data)
    out->size = out->buf->size;
  else
    out->size = 0;

  int oldStreamIndex = output->getStreamIndex();
  int e = 0;
  if (!dropFrame)
    e = avcodec_encode_video2(getCodecCtx(), out, in, &got_frame);
  // some codec erroneously set stream_index, but our encoders are always
  // muxer independent. we fix that here.
  output->setStreamIndex(oldStreamIndex);
  if (got_frame) {
    output->setCoder(this);
    output->setTimeBase(coderTb.value());
    output->setComplete(out->size > 0, out->size);
  }
#ifdef VS_DEBUG
  char outDescr[256]; *outDescr = 0;
  char inDescr[256]; *inDescr = 0;
  if (aFrame) aFrame->logMetadata(inDescr, sizeof(inDescr));
  if (aOutput) aOutput->logMetadata(outDescr, sizeof(outDescr));
  VS_LOG_TRACE("encodeVideo Encoder@%p[out:%s;in:%s;encoded:%" PRIi64,
               this,
               outDescr,
               inDescr,
               (int64_t)e);
#endif

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
  RefPointer<Rational> coderTb = this->getTimeBase();

  bool dropFrame = false;

  if (samples) {
    // make copy so we can modify meta-data
    inputAudio = MediaAudio::make(samples, false);
    RefPointer<Rational> inputAudioTb = inputAudio->getTimeBase();
    /**
     * check time stamps and drop frames when needed if the time
     * base of the input frame cannot be converted to the encoder
     * time base without rounding.
     *
     */
    int64_t inTs = coderTb->rescale(inputAudio->getTimeStamp(), inputAudioTb.value(), Rational::ROUND_DOWN);
    if (mLastPtsEncoded != Global::NO_PTS) {
      if (inTs <= mLastPtsEncoded) {
        VS_LOG_DEBUG(
            "Encoder@%p Dropping frame with timestamp %lld (if coder supports higher time-base use that instead)",
            this,
            inputAudio->getPts());
        dropFrame = true;
      }
    }
    if (!dropFrame) {
        mLastPtsEncoded = inTs;
        inputAudio->setTimeBase(coderTb.value());
        // set the timestamp after the timbase as setTimeBase does a conversion.
        inputAudio->setTimeStamp(inTs);
    }
  }
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
      // This clearly does not work -- it should be maintaining frame size but
      // it is not. Need a good think on the right way to solve this.
      mAResampler->resample(mResampledAudio.value(), samples);
      if (mResampledAudio->isComplete())
        inputAudio = mResampledAudio.get();
      else
        inputAudio = 0;
    }
    // now a sanity check
    if (!(!samples || !inputAudio || inputAudio->getNumSamples() == getFrameSize())) {
      VS_THROW(HumbleRuntimeError::make("not flushing, but samples returned (%ld) are less than frame size (%ld)",
                                        inputAudio->getNumSamples(),
                                        getFrameSize()

          ));
    }
  }

  AVFrame* in = inputAudio ? inputAudio->getCtx() : 0;
  AVPacket* out = output->getCtx();

  int got_frame = 0;
  int oldStreamIndex = output->getStreamIndex();
  int e = 0;
  if (!dropFrame)
    avcodec_encode_audio2(getCodecCtx(), out, in, &got_frame);
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

#ifdef VS_DEBUG
  char outDescr[256]; *outDescr = 0;
  char inDescr[256]; *inDescr = 0;
  if (samples) samples->logMetadata(inDescr, sizeof(inDescr));
  if (aOutput) aOutput->logMetadata(outDescr, sizeof(outDescr));
  VS_LOG_TRACE("encodeAudio Encoder@%p[out:%s;in:%s;encoded:%" PRIi64,
               this,
               outDescr,
               inDescr,
               (int64_t)e);
#endif


  FfmpegException::check(e, "could not encode audio ");
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
