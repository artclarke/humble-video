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
 * Encoder.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#include "Encoder.h"
#include <libavutil/samplefmt.h>
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
        VS_THROW(HumbleInvalidArgument::make("Audio format '%s' not supported by encoder.",
                                             av_get_sample_fmt_name((enum AVSampleFormat)format)));
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
        /*
         * This codec requires a fixed frame size, and we cannot guarantee our callers
         * will always send in the right audio. So we're going to set up a filter chain
         * whose sole purpose is to get us audio from the input format into right format
         * for this codec
         *
         * For future features, this imposes a overhead for users who are smart enough
         * to align frame-sizes, and we should provide an option to make this optional. But
         * a learning from the past is to assume the callers don't actually know to make
         * correctly aligned frames.
         */
        mAudioGraph = FilterGraph::make();
        mAudioSource = mAudioGraph->addAudioSource("in",
            getSampleRate(),
            getChannelLayout(),
            getSampleFormat(),
            0);
        mAudioSink = mAudioGraph->addAudioSink("out",
            getSampleRate(),
            getChannelLayout(),
            getSampleFormat());
        // a graph that passes through the audio unmodified.
        mAudioGraph->open("[in]anull[out]");
        // now, fix the output frame size.
        mAudioSink->setFrameSize(frameSize);

        mFilteredAudio = MediaAudio::make(getFrameSize(),
            getSampleRate(), getChannels(),
            getChannelLayout(), getSampleFormat());

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

  if (aFrame && getState() == STATE_FLUSHING) {
    VS_THROW(HumbleRuntimeError("Cannot add data to an encoder after you passed in null; encoder is flushing"));
  }
  if (aFrame && getState() != STATE_OPENED) {
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
  RefPointer<MediaPicture> frame = aFrame ? MediaPicture::make(aFrame, false) : 0;

  if (frame) {

    if (!frame->isComplete()) {
      VS_THROW(HumbleInvalidArgument("Can only pass complete media to encode"));
    }

    if (frame->getPts() == Global::NO_PTS) {
      VS_THROW(HumbleInvalidArgument("Passed in media must have a valid time stamp"));
    }

    ensurePictureParamsMatch(frame.value());

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
  } else {
    setState(STATE_FLUSHING);
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
Encoder::encodeAudioInternal (MediaPacket* aOutput, MediaAudio* samples)
{
  // make copy so we can modify meta-data
  RefPointer<MediaAudio> inputAudio= samples ? MediaAudio::make(samples, false) : 0;

  MediaPacketImpl* output = dynamic_cast<MediaPacketImpl*>(aOutput);
  bool dropFrame = false;
  RefPointer<Rational> coderTb = this->getTimeBase();
  if (inputAudio)
  {
    RefPointer<Rational> inputAudioTb = inputAudio->getTimeBase ();
    /**
     * check time stamps and drop frames when needed if the time
     * base of the input frame cannot be converted to the encoder
     * time base without rounding.
     *
     */
    int64_t inTs = coderTb->rescale (inputAudio->getTimeStamp (),
                                     inputAudioTb.value (),
                                     Rational::ROUND_DOWN);
    if (mLastPtsEncoded != Global::NO_PTS)
    {
      if (inTs <= mLastPtsEncoded)
      {
        VS_LOG_DEBUG(
            "Encoder@%p Dropping frame with timestamp %lld as it is <= last timestamp encoded %lld (if coder supports higher time-base use that instead). "
            "Coder timebase: (%lld/%lld); Sample timebase: (%lld/%lld)",
            this, inTs, mLastPtsEncoded,
            coderTb->getNumerator(), coderTb->getDenominator(),
            inputAudioTb->getNumerator(), inputAudioTb->getDenominator()
            );
        dropFrame = true;
      }
    }
    if (!dropFrame)
    {
      mLastPtsEncoded = inTs;
      inputAudio->setTimeBase (coderTb.value ());
      // set the timestamp after the timbase as setTimeBase does a conversion.
      inputAudio->setTimeStamp (inTs);
    }
  }
  AVFrame* in = inputAudio ? inputAudio->getCtx () : 0;
  AVPacket* out = output->getCtx ();
  int got_frame = 0;
  int oldStreamIndex = output->getStreamIndex ();
  int e = 0;
  if (!dropFrame)
    e = avcodec_encode_audio2 (getCodecCtx (), out, in, &got_frame);

  // some codec erroneously set stream_index, but our encoders are always
  // muxer independent. we fix that here.
  output->setStreamIndex (oldStreamIndex);
  if (got_frame)
  {
    output->setCoder (this);
    output->setTimeBase (coderTb.value ());
    output->setComplete (true, out->size);
  }
  else
  {
    output->setComplete (false, 0);
  }
#ifdef VS_DEBUG
  {
    char outDescr[256]; *outDescr = 0;
    char inDescr[256]; *inDescr = 0;
    if (inputAudio) inputAudio->logMetadata(inDescr, sizeof(inDescr));
    if (aOutput) aOutput->logMetadata(outDescr, sizeof(outDescr));
    VS_LOG_TRACE("encodeAudio Encoder@%p[out:%s;in:%s;encoded:%" PRIi64 "]",
                 this,
                 aOutput ? outDescr : "(null)",
                     inputAudio ? inDescr : "(null)",
                         (int64_t)e);
  }
#endif

  FfmpegException::check (e, "could not encode audio ");
}

void
Encoder::encodeAudio(MediaPacket* aOutput, MediaAudio* samples) {
  MediaPacketImpl* output = dynamic_cast<MediaPacketImpl*>(aOutput);
  RefPointer<Codec> codec = getCodec();
  bool cachingAudio = !(codec->getCapabilities() & Codec::CAP_VARIABLE_FRAME_SIZE);

  if (getCodecType() != MediaDescriptor::MEDIA_AUDIO) {
    VS_THROW(HumbleRuntimeError("Attempting to encode audio on non-audio encoder"));
  }
  if (!output) {
    VS_THROW(HumbleInvalidArgument("output cannot be null"));
  }
  if (samples) {
    if (!samples->isComplete()) {
      VS_THROW(HumbleInvalidArgument("Can only pass complete media to encode"));
    }
    // let's check the audio parameters.
    ensureAudioParamsMatch(samples);

    switch(getState()) {
      case STATE_OPENED:
        if (cachingAudio)
          // this codec requires that the right number of audio samples
          // gets passed in each call.
          mAudioSource->addAudio(samples);

        break;
      case STATE_FLUSHING:
        VS_THROW(HumbleRuntimeError("Cannot add new data to an encoder once flushing has started."));
        break;
      default:
        VS_THROW(HumbleRuntimeError("Attempt to pass data to encoder but encoder is not ready."));
        break;
    }
  } else {
    switch(getState()) {
      case STATE_OPENED:
        if (cachingAudio)
          mAudioSource->addAudio(0); // tell the cache we're flushing.
        setState(STATE_FLUSHING);
        break;
      case STATE_FLUSHING:
        break;
      default:
        VS_THROW(HumbleRuntimeError("Attempt to flush encoder but encoder is not open."));
        break;
    }
  }

  // now reset the packet so we allocate new memory (because encoders sometimes change packet sizes).
  output->reset(0);


  switch(getState()) {
    case STATE_OPENED:
      if (cachingAudio) {
        // pull the sink.
        mAudioSink->getAudio(mFilteredAudio.value());

#ifdef VS_DEBUG
        {
          char outDescr[256]; *outDescr = 0;
          char inDescr[256]; *inDescr = 0;
          if (samples) samples->logMetadata(inDescr, sizeof(inDescr));
          if (mFilteredAudio) mFilteredAudio->logMetadata(outDescr, sizeof(outDescr));
          VS_LOG_TRACE("encodeAudio filterAudio Encoder@%p[out:%s;in:%s];",
                       this,
                       mFilteredAudio ? outDescr : "(null)",
                           samples ? inDescr : "(null)");
        }
#endif

        if (mFilteredAudio->isComplete()) {
          encodeAudioInternal(output, mFilteredAudio.value());
        } else {
#ifdef VS_DEBUG
          {
            char outDescr[256]; *outDescr = 0;
            char inDescr[256]; *inDescr = 0;
            if (samples) samples->logMetadata(inDescr, sizeof(inDescr));
            if (aOutput) aOutput->logMetadata(outDescr, sizeof(outDescr));
            VS_LOG_TRACE("encodeAudio Encoder@%p[out:%s;in:%s;message:not enough audio staged yet]",
                         this,
                         aOutput ? outDescr : "(null)",
                             samples ? inDescr : "(null)");
          }
#endif
        }
      } else {
        encodeAudioInternal(output, samples);
      }
      break;
    case STATE_FLUSHING:
      if (cachingAudio) {
        // pull the sink in a loop to get all the audio out while we're making complete packets.
        // this is a fix for issue: https://github.com/artclarke/humble-video/issues/36
        do {
          mAudioSink->getAudio(mFilteredAudio.value());

#ifdef VS_DEBUG
          {
            char outDescr[256]; *outDescr = 0;
            char inDescr[256]; *inDescr = 0;
            if (samples) samples->logMetadata(inDescr, sizeof(inDescr));
            if (mFilteredAudio) mFilteredAudio->logMetadata(outDescr, sizeof(outDescr));
            VS_LOG_TRACE("encodeAudio filterAudio Encoder@%p[out:%s;in:%s];",
                         this,
                         mFilteredAudio ? outDescr : "(null)",
                             samples ? inDescr : "(null)");
          }
#endif

          if (mFilteredAudio->isComplete()) {
            encodeAudioInternal(output, mFilteredAudio.value());
          } else {
            // now done, so we tell the real encode to start flushing.
            encodeAudioInternal(output, 0);
          }
        } while (mFilteredAudio->isComplete() && !output->isComplete());
      } else {
        encodeAudioInternal(output, samples);
      }
      break;
    default:
      VS_THROW(HumbleRuntimeError("Encoder in unknown and error state"));
      break;
  }
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
