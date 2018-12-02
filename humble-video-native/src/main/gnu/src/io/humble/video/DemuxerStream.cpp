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
 * DemuxerStream.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: aclarke
 */

#include "Demuxer.h"
#include "DemuxerStream.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/VideoExceptions.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.DemuxerStream);

namespace io {
namespace humble {
namespace video {

DemuxerStream::DemuxerStream(Container* container, int32_t index) :
    ContainerStream(container, index) {
  VS_LOG_TRACE("Created: %p", this);
}

DemuxerStream::~DemuxerStream() {
  VS_LOG_TRACE("Destroyed: %p", this);
}

DemuxerStream*
DemuxerStream::make(Container* container, int32_t index) {
  RefPointer<DemuxerStream> r;
  r.reset(new DemuxerStream(container, index), true);
  return r.get();
}
Demuxer*
DemuxerStream::getDemuxer() {
  return dynamic_cast<Demuxer*>(getContainer());
}

Decoder*
DemuxerStream::getDecoder() {
  AVStream* stream = getCtx();

  if (!mDecoder) {
    if (stream->codecpar) {
      // make a copy of the decoder so we decouple it from the container
      // completely
      if (stream->codecpar->codec_id != AV_CODEC_ID_NONE) {
        RefPointer<Codec> codec = Codec::findDecodingCodec((Codec::ID)stream->codecpar->codec_id);
        if (!codec) {
          VS_THROW(HumbleRuntimeError("could not find decoding codec"));
        }
        mDecoder = Decoder::make(codec->getCtx(), stream->codecpar);
        // now, if the stream has a timebase, use it. The CodecParameters do not pass that through.
        if (stream->time_base.den != 0) {
          RefPointer<Rational> tb = Rational::make(stream->time_base.num, stream->time_base.den);
          mDecoder->setTimeBase(tb.value());
        }
      }
    }
  }
  return mDecoder.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
