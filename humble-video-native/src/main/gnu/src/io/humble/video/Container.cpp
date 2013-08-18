/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Container.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#include "Container.h"
#include "PropertyImpl.h"
#include "Encoder.h"
#include "Decoder.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/customio/URLProtocolHandler.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::video::customio;
using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Container::Container() {
}

Container::~Container() {
  // Remove all the stream meta-data objects.
  while(mStreams.size() > 0)
  {
    Stream* stream=mStreams.back();
    if (stream) {
      delete stream;
    }
    mStreams.pop_back();
  }

}

int
Container::url_read(void*h, unsigned char* buf, int size) {
  int retval = -1;
  try {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler) retval = handler->url_read(buf, size);
  } catch (...) {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_read(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}
int
Container::url_write(void*h, unsigned char* buf, int size) {
  int retval = -1;
  try {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler) retval = handler->url_write(buf, size);
  } catch (...) {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_write(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}

int64_t
Container::url_seek(void*h, int64_t position, int whence) {
  int64_t retval = -1;
  try {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler) retval = handler->url_seek(position, whence);
  } catch (...) {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_seek(%p, %lld) ==> %d", h, position,
      whence, retval);
  return retval;
}

Container::Stream::Stream(Container* container, int32_t index) {
  mContainer = container;
  mIndex = index;
  mLastDts = Global::NO_PTS;
  mCachedCtx = 0;
  mCtx = mContainer->getFormatCtx()->streams[index];
}

Container::Stream::~Stream() {

}

Coder*
Container::Stream::getCoder() {
  if (!mCoder) {
    // we need to know the stream direction...
    AVFormatContext* ctx = mContainer->getFormatCtx();
    AVStream* stream = getCtx();

    if (!ctx || !stream) {
      VS_THROW(HumbleRuntimeError("could not get container context to find coder"));
    }
    if (!stream->codec) {
      VS_THROW(HumbleRuntimeError("No codec set for stream"));
    }
    RefPointer<Codec> codec;

    if (ctx->iformat) {
      // make a copy of the decoder so we decouple it from the container
      // completely
      codec = Codec::findDecodingCodec((Codec::ID)stream->codec->codec_id);
      if (!codec) {
        VS_THROW(HumbleRuntimeError("could not find decoding codec"));
      }
      mCoder = Decoder::make(codec.value(), stream->codec);
    } else {
      VS_THROW(HumbleRuntimeError("Got null encoder on MuxerStream which should not be possible"));
    }
  }
  return mCoder.get();
}

void
Container::doSetupStreams() {
  // do nothing if we're already all set up.
  AVFormatContext* ctx = getFormatCtx();
  if (!ctx) {
    VS_THROW(HumbleRuntimeError("Attempt to setup streams on closed or error container"));
  }
  if (mStreams.size() == ctx->nb_streams) return;

  // loop through and find the first non-zero time base
  AVRational *goodTimebase = 0;
  for (uint32_t i = 0; i < ctx->nb_streams; i++) {
    AVStream *avStream = ctx->streams[i];
    if (avStream && avStream->time_base.num && avStream->time_base.den) {
      goodTimebase = &avStream->time_base;
      break;
    }
  }

  // Only look for new streams
  for (uint32_t i = mStreams.size(); i < ctx->nb_streams; i++) {
    AVStream *avStream = ctx->streams[i];
    if (!avStream)
      VS_THROW(HumbleRuntimeError::make("no FFMPEG allocated stream: %d", i));
    if (!avStream->time_base.num || !avStream->time_base.den) {
      if (avStream->codec && avStream->codec->sample_rate && avStream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
      {
        // for audio, 1/sample-rate is a good timebase.
        avStream->time_base.num = 1;
        avStream->time_base.den = avStream->codec->sample_rate;
      } else if(goodTimebase) {
        avStream->time_base = *goodTimebase;
      }
    }
    // now let's initialize our stream object.
    Stream* stream = new Stream(this, i);
    mStreams.push_back(stream);
  }
}

Container::Stream*
Container::getStream(int32_t index) {
  if (index < 0) {
    VS_THROW(HumbleInvalidArgument("index must be >= 0"));
  }
  doSetupStreams();
  if ((size_t)index >= mStreams.size()) {
    VS_THROW(HumbleInvalidArgument("index must be < #getNumStreams()"));
  }
  return mStreams[index];
}

void
Container::popCoders() {
  int32_t n = getNumStreams();
  for(int32_t i = 0; i < n; i++) {
    Container::Stream* stream = mStreams[i];
    stream->popCoder();
  }
}

void
Container::pushCoders() {
  int32_t n = getNumStreams();
  for(int32_t i = 0; i < n; i++) {
    Container::Stream* stream = mStreams[i];
    stream->pushCoder();
  }
}
int32_t
Container::getNumStreams()
{
  doSetupStreams();
  return mStreams.size();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
