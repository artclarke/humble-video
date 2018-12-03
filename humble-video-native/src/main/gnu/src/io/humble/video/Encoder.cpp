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

Encoder::Encoder(const AVCodec* codec, const AVCodecParameters* src) : Coder(codec, src) {
  VS_LOG_TRACE("Created: %p", this);
}

Encoder::~Encoder() {
  VS_LOG_TRACE("Destroyed: %p", this);
}

Encoder*
Encoder::make(Codec* codec)
{
  if (!codec) {
    VS_THROW(HumbleInvalidArgument("no codec passed in"));
  }

  if (!codec->canEncode()) {
    VS_THROW(HumbleInvalidArgument("passed in codec cannot encode"));
  }

  RefPointer<Encoder> r;
  r.reset(new Encoder(codec->getCtx(), 0), true);
  return r.get();
}

Encoder*
Encoder::make(Coder* src)
{
  if (!src) {
    VS_THROW(HumbleInvalidArgument("no coder to copy"));
  }

  RefPointer<Encoder> retval;
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
    retval = make(c.value());
  } else {
    AVCodecParameters* pars = avcodec_parameters_alloc();
    if (!pars)
      VS_THROW(HumbleBadAlloc());
    if (avcodec_parameters_from_context(pars, src->getCodecCtx()) < 0)
    {
      avcodec_parameters_free(&pars);
      VS_THROW(HumbleRuntimeError::make("cannot copy parameters from codec"));
    }
    retval.reset(new Encoder(c->getCtx(), pars), true);
    avcodec_parameters_free(&pars);
  }
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

ProcessorResult
Encoder::sendRaw(MediaRaw* media) {

  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to receive(Media), but Encoder is not opened"));

   // null media is allowed, but we should check if not null that the type is valid.
  MediaDescriptor::Type type = getCodecType();
  switch(type) {
  case MediaDescriptor::MEDIA_AUDIO: {
    MediaAudio* audio = dynamic_cast<MediaAudio*>(media);
    if (!audio && media)
      VS_THROW(HumbleInvalidArgument("passed non-audio Media to an audio encoder"));
    ensureAudioParamsMatch(audio);

  }
  break;
  case MediaDescriptor::MEDIA_VIDEO: {
    MediaPicture* picture = dynamic_cast<MediaPicture*>(media);
    if (!picture && media)
      VS_THROW(HumbleInvalidArgument("passed non-video Media to a video encoder"));
    ensurePictureParamsMatch(picture);

  }
  break;
  case MediaDescriptor::MEDIA_SUBTITLE: {
      MediaSubtitle* subtitle = dynamic_cast<MediaSubtitle*>(media);
      if (!subtitle && media)
        VS_THROW(HumbleInvalidArgument("passed non-subtitle Media to a subtitle encoder"));
    }
    break;
  default:
    VS_THROW(HumbleInvalidArgument("passed a media type that is not compatible with this encoder"));
  }
  // now let's get the avframe
  const AVFrame* frame = media ? media->getCtx() : 0;

  int e = 0;
  e = avcodec_send_frame(getCodecCtx(), frame);
#ifdef VS_DEBUG
  char desc[256]; *desc = 0;
  if (media) media->logMetadata(desc, sizeof(desc));
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
Encoder::receiveEncoded(MediaEncoded* media) {
  if (STATE_OPENED != getState())
    VS_THROW(HumbleRuntimeError("Attempt to send(Media), but Encoder is not opened"));

  MediaPacketImpl* packet = dynamic_cast<MediaPacketImpl*>(media);
  if (!packet)
    VS_THROW(HumbleInvalidArgument("Encoders require non-null MediaRaw objects"));

  // now reset the packet so we allocate new memory (because encoders sometimes change packet sizes).
  packet->reset(0);

  int e = 0;
  RefPointer<Rational> coderTb = this->getTimeBase();

  AVPacket* out = packet->getCtx();
  if (out->buf && out->data)
    out->size = out->buf->size;
  else
    out->size = 0;

  e = avcodec_receive_packet(getCodecCtx(), out);
  if (e != AVERROR(EAGAIN) && e != AVERROR_EOF)
    FfmpegException::check(e, "could not encode media");

  int32_t oldStreamIndex = packet->getStreamIndex();
  packet->setStreamIndex(oldStreamIndex);
  if (e == 0) {
    packet->setCoder(this);
    packet->setTimeBase(coderTb.value());
    packet->setComplete(out->size > 0, out->size);
  }
#ifdef VS_DEBUG
  char descr[256]; *descr = 0;
  packet->logMetadata(descr, sizeof(descr));
  VS_LOG_TRACE("receiveEncoded Encoder@%p[%s]:%" PRIi64,
               this,
               descr,
               (int64_t)e);
#endif
  return (ProcessorResult) e;
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
