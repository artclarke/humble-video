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
 * Muxer.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/customio/URLProtocolManager.h>

#include "Muxer.h"
#include "Decoder.h"
#include "VideoExceptions.h"
#include "KeyValueBagImpl.h"
#include "MediaPacketImpl.h"

VS_LOG_SETUP(VS_CPP_PACKAGE.Muxer);

using namespace io::humble::ferry;
using namespace io::humble::video::customio;

namespace io {
namespace humble {
namespace video {

#if 0
int32_t
Muxer::acquire()
{
  VS_LOG_DEBUG("muxer acquire: %p", this);
  return RefCounted::acquire();
}
int32_t
Muxer::release()
{
  VS_LOG_DEBUG("muxer release: %p", this);
  return RefCounted::release();
}
#endif // 0

Muxer::Muxer(MuxerFormat* format, const char* filename,
    const char* formatName) {
  mState = STATE_INITED;
  mIOHandler = 0;
  mBufferLength = 2048;

  mCtx = 0;
  int e = avformat_alloc_output_context2(&mCtx, format ? format->getCtx() : 0, formatName,
      filename);
  FfmpegException::check(e, "could not allocate output context ");
  if (!mCtx) {
    VS_THROW(HumbleBadAlloc());
  }
  if (!filename || !*filename) mCtx->filename[0] = 0;
  mCtx->interrupt_callback.callback = Global::avioInterruptCB;
  mCtx->interrupt_callback.opaque = this;

  // now let's look at the output format; it should have been guessed.
  if (!format) {
    if (!mCtx->oformat) {
      avformat_free_context(mCtx);
      VS_THROW(
          HumbleRuntimeError::make(
              "could not determine format to use for muxer. Filename: %s. FormatName: %s",
              filename, formatName));
    }
    mFormat = MuxerFormat::make(mCtx->oformat);
  } else mFormat.reset(format, true);

  // determine if this format NEEDs a file.
  if (!mFormat->getFlag(ContainerFormat::NO_FILE)) {
    if (!*mCtx->filename) {
      avformat_free_context(mCtx);
      VS_THROW(
          HumbleRuntimeError::make(
              "No filename specified, but MuxerFormat needs a file. Filename: %s. FormatName: %s",
              filename, formatName));
    }
  }
  VS_LOG_TRACE("Created: %p", this);
}

Muxer::~Muxer() {
  if (mState == STATE_OPENED) {
    VS_LOG_ERROR(
        "Open Muxer destroyed without Muxer.close() being called. Closing anyway: %s",
        this->getURL());
    (void) this->close();
  }
  if (mCtx) {
    for(uint32_t i = 0; i < mCtx->nb_streams; i++) {
      if (mCtx->streams[i]->codec)
        (void) avcodec_close(mCtx->streams[i]->codec);
    }
    avformat_free_context(mCtx);
  }
  VS_LOG_TRACE("Destroyed: %p", this);
}

Muxer*
Muxer::make(const char* filename, MuxerFormat *format, const char* formatName) {
  Global::init();

  RefPointer<Muxer> retval;

  if (!format) {
    if ((!filename || !*filename) && (!formatName && !*formatName)) {
      VS_THROW(HumbleInvalidArgument("cannot pass in all null parameters"));
    }
  }
  retval.reset(new Muxer(format, filename, formatName), true);
  return retval.get();
}

void
Muxer::setOutputBufferLength(int32_t size) {
  if (size <= 0)
  VS_THROW(HumbleInvalidArgument("size <= 0"));
  if (mState != STATE_INITED)
  VS_THROW(HumbleRuntimeError("Muxer object has already been opened"));
  mBufferLength = size;
}

int32_t
Muxer::getOutputBufferLength() {
  return mBufferLength;
}

void
Muxer::open(KeyValueBag *aInputOptions, KeyValueBag* aOutputOptions) {
  AVFormatContext* ctx = this->getFormatCtx();
  int retval = -1;
  if (mState != STATE_INITED) {
    VS_THROW(
        HumbleRuntimeError::make(
            "Open can only be called when container is in init state. Current state: %d",
            mState));
  }

  AVDictionary* tmp = 0;
  const char* url = ctx->filename;
  AVOutputFormat* fmt = mFormat ? mFormat->getCtx() : 0;


  // Let's check for custom IO
  mIOHandler = URLProtocolManager::findHandler(mCtx->filename,
      URLProtocolHandler::URL_WRONLY_MODE, 0);

  if (mIOHandler) {
    ctx->flags |= AVFMT_FLAG_CUSTOM_IO;
    // free and realloc the input buffer length
    uint8_t* buffer = (uint8_t*) av_malloc(mBufferLength);
    if (!buffer) {
      mState = STATE_ERROR;
      VS_THROW(HumbleBadAlloc());
    }

    // we will allocate ourselves an io context;
    // ownership of buffer passes here.
    ctx->pb = avio_alloc_context(buffer, mBufferLength, 1, mIOHandler,
        Container::url_read, Container::url_write, Container::url_seek);
    if (!ctx->pb) {
      av_free(buffer);
      mState = STATE_ERROR;
      VS_THROW(
          HumbleRuntimeError::make(
              "could not open url due to internal error: %s", url));
    }
  }
  // Check for passed in options
  KeyValueBagImpl* realOpts = dynamic_cast<KeyValueBagImpl*>(aInputOptions);
  if (realOpts) av_dict_copy(&tmp, realOpts->getDictionary(), 0);

  try {
    if (mIOHandler) {
      retval = mIOHandler->url_open(url, URLProtocolHandler::URL_WRONLY_MODE);
    } else if (!fmt || !(fmt->flags & AVFMT_NOFILE)) {
      retval = avio_open2(&ctx->pb, url, AVIO_FLAG_WRITE,
          &ctx->interrupt_callback, 0);
    }
    if (retval < 0) {
      mState = STATE_ERROR;
      FfmpegException::check(retval, "Error opening url: %s; ", url);
    }

    int32_t numStreams = getNumStreams();
    if (numStreams < 0 &&
        !(ctx->ctx_flags & AVFMTCTX_NOHEADER))
      VS_THROW(io::humble::ferry::HumbleIOException("no streams added to muxer"));

    if (numStreams == 0)
    {
      RefPointer<ContainerFormat> format = getFormat();
      if (format)
      {
        const char *shortName = format->getName();
        if (shortName && !strcmp(shortName, "mp3"))
          VS_THROW(io::humble::ferry::HumbleIOException("no streams added to mp3 muxer"));
      }
    }

    pushCoders();
    /* Write the stream header, if any. */
    retval = avformat_write_header(ctx, &tmp);
    popCoders();
    if (retval < 0) {
      mState = STATE_ERROR;
      FfmpegException::check(retval, "Could not write header for url: %s. ", url);
    }
  } catch (std::exception & e) {
    if (tmp) av_dict_free(&tmp);
    throw;
  }
  KeyValueBagImpl* realUnsetOpts =
      dynamic_cast<KeyValueBagImpl*>(aOutputOptions);
  if (realUnsetOpts) realUnsetOpts->copy(tmp);
  if (tmp) av_dict_free(&tmp);

  mState = STATE_OPENED;
  // let's log the state of the world.
  logOpen(this);
}

void
Muxer::close() {
  if (getState() != STATE_OPENED) {
    VS_THROW(HumbleRuntimeError::make("closed container that was not open"));
  }
  AVFormatContext* ctx = getFormatCtx();
  pushCoders();
  int e = av_write_trailer(ctx);
  popCoders();
  if (e < 0) {
    mState = STATE_ERROR;
    FfmpegException::check(e, "could not write trailer ");
  }
  if (mIOHandler) {
    e = mIOHandler->url_close();
  } else
    e = avio_close(ctx->pb);
  FfmpegException::check(e, "could not close url ");
  mState = STATE_CLOSED;
}

const char*
Muxer::getURL() {
  return this->getFormatCtx()->filename;
}

MuxerStream*
Muxer::addNewStream(Coder* aCoder) {
  if (!aCoder) {
    VS_THROW(HumbleInvalidArgument("coder must be not null"));
  }
  RefPointer<Coder> coder;
  if (dynamic_cast<Decoder*>(aCoder)) {
    // we're going to make a copy as we will modify some items to support
    // remuxing.
    coder = Decoder::make(aCoder);
    // now we're doing to fix some stuff.
    AVCodecContext* ctx = coder->getCodecCtx();
    // right; got to find a codec tag for the id passed in. Suck.
    ctx->codec_tag = mFormat->getBestCodecTag(coder->getCodecID());

    // not sure this is the right fix here
    if (mFormat->getFlag(ContainerFormat::GLOBAL_HEADER))
      coder->setFlag(Coder::FLAG_GLOBAL_HEADER, true);

    // and open the new decoder
    (dynamic_cast<Decoder*>(coder.value()))->open(0, 0);

  } else
    coder.reset(aCoder, true);

  if (coder->getState() != Coder::STATE_OPENED) {
    VS_THROW(HumbleInvalidArgument("coder must be open"));
  }
  if (getState() != STATE_INITED) {
    VS_THROW(HumbleInvalidArgument("cannot add MuxerStream after Muxer is opened"));
  }

  if (mFormat->getFlag(ContainerFormat::GLOBAL_HEADER)) {
    // the Codec must have the global header flag set. We'll throw an error if not.
    if (!coder->getFlag(Coder::FLAG_GLOBAL_HEADER)) {
      VS_THROW(HumbleInvalidArgument("this container requires a global header, and this Coder does not have the global header flag set"));
    }
  }

  RefPointer<MuxerStream> r;

  AVFormatContext* ctx = getFormatCtx();
  // Tell Ffmpeg about the new stream.
  AVStream* avStream = avformat_new_stream(ctx, coder->getCodecCtx()->codec);
  if (!avStream) {
    VS_THROW(HumbleRuntimeError("Could not add new stream to container"));
  }
  if (avStream->codec && !avStream->codec->codec) {
    // fixes a memory leak on closing.
    avStream->codec->codec = coder->getCodecCtx()->codec;
  }
  // tell the container to update all the known streams.
  doSetupStreams();
  // and set the coder for the given stream
  Container::Stream* stream = Container::getStream(avStream->index);
  // grab a reference to the passed in coder.
  stream->setCoder(coder.value());

  r.reset(this->getStream(avStream->index), false);


  // let's log the state of the world.
  VS_LOG_TRACE("addNewStream Muxer@%p[i:%"PRId32";c:%p;tb:%"PRId32"/%"PRId32"]",
               this,
               (int32_t)avStream->index,
               coder.value(),
               (int32_t)avStream->time_base.num,
               (int32_t)avStream->time_base.den
               );
  return r.get();
}

MuxerStream*
Muxer::getStream(int32_t i) {
  if (!(getState() == STATE_OPENED || getState() == STATE_INITED)) {
    VS_THROW(HumbleRuntimeError("Attempt to get MuxerStream from Muxer when Muxer is in invalid state"));
  }
  if (i < 0) {
    VS_THROW(HumbleInvalidArgument("position must be >= 0"));
  }
  if (i >= getNumStreams()) {
    VS_THROW(HumbleInvalidArgument("position must be < getNumStreams()"));
  }
  return MuxerStream::make(this, i);
}


void
Muxer::logWrite(Muxer* muxer, MediaPacket* in, MediaPacket* out, int32_t retval)
{
  // necessary to avoid a compiler warning in release mode since
  // in that case, this is a NO_OP method.
  (void) muxer;
  (void) in;
  (void) out;
  (void) retval;
#ifdef VS_DEBUG
  char inDescr[256];
  char outDescr[256];
  if (in)
    in->logMetadata(inDescr, sizeof(inDescr));
  if (out)
    out->logMetadata(outDescr, sizeof(outDescr));
  VS_LOG_TRACE("write Muxer@%p[out:%s;in:%s;e:%"  PRIi64 "]",
               muxer,
               outDescr?outDescr:"(null)",
               inDescr?inDescr:"(null)",
               (int64_t)retval);
#endif // VS_DEBUG
}

void
Muxer::logOpen(Muxer* muxer) {
  (void) muxer;
#ifdef VS_DEBUG
  // We only do this in debug mode. Because a muxer can have effectively
  // an unlimited number of strings, we need to build our log up piece meal.

  int32_t n = muxer->getNumStreams();

  char msg[4096];
  char* buf = msg;
  size_t bufSize = sizeof(msg);
  int32_t chars = snprintf(buf, bufSize, "open Muxer@%p[u:%s;n:%"PRId32";", muxer,
                           muxer->getURL(),
                           (int32_t)n);
  buf += chars;
  bufSize -= chars;

  for(int i = 0; i < n; i++) {
    Container::Stream* s = ((Container*)muxer)->getStream(i);
    RefPointer<Coder> c = s->getCoder();
    AVStream* avStream = s->getCtx();
    chars = snprintf(buf, bufSize, "s@%"PRId32"[c:%p;tb:%"PRId32"/%"PRId32"];",
           (int32_t)i,
           c.value(),
           (int32_t)avStream->time_base.num,
           (int32_t)avStream->time_base.den
    );
    buf+= chars;
    bufSize -= chars;
  }
  chars = snprintf(buf, bufSize, "]");
  VS_LOG_TRACE(msg);
#endif
}

bool
Muxer::write(MediaPacket* aPacket, bool forceInterleave) {
  MediaPacketImpl* packet = dynamic_cast<MediaPacketImpl*>(aPacket);
  bool allDataFlushed = false;

  if (getState() != STATE_OPENED) {
    VS_THROW(HumbleRuntimeError("Cannot write to unopened Muxer"));
  }
  if (!packet) {
    VS_THROW(HumbleInvalidArgument("null packet"));
  }
  if (!packet->isComplete()) {
    VS_THROW(HumbleInvalidArgument("cannot write incomplete packet"));
  }
  int numStreams = getNumStreams();
  if (packet->getStreamIndex() >= numStreams) {
    VS_THROW(HumbleRuntimeError("attempt to write packet to stream that does not exist in this muxer"));
  }
  if (!packet->getSize()) {
    VS_THROW(HumbleRuntimeError("Cannot write empty packet"));
  }

  // we copy the metadata into a new packet, but not the actual data.
  RefPointer<MediaPacketImpl> outPacket = MediaPacketImpl::make(packet, false);

  Container::Stream* stream=0;

  int32_t index = outPacket->getStreamIndex();
  if (index < 0) {
    RefPointer<Coder> encoder = packet->getCoder();
    for(int i = 0; i < numStreams; i++) {
      stream = Container::getStream(i);
      RefPointer<Coder> streamCoder = stream->getCoder();
      Coder *a = encoder.value();
      Coder *b = streamCoder.value();
      if (b && a == b) {
        // this is the one we actually care about.
        outPacket->setStreamIndex(i);
        break;
      }
      stream = 0;
    }
  } else {
    stream = Container::getStream(index);
  }
  if (!stream) {
    VS_THROW(HumbleRuntimeError("Could not find stream that corresponds to this packet. Did you add it?"));
  }

  // then we adjust timestamps if necessary for this muxer.
  stampOutputPacket(stream, outPacket.value());

  /// now, do the madness.
  AVPacket* out = outPacket->getCtx();
  int e;
  pushCoders();
  if (forceInterleave)
    e = av_interleaved_write_frame(getFormatCtx(), out);
  else
    e = av_write_frame(getFormatCtx(), out);
  popCoders();
  Muxer::logWrite(this, aPacket, outPacket.value(), e);
  FfmpegException::check(e, "Could not write packet to muxer ");
  if (e == 1)
    allDataFlushed = true;

  return allDataFlushed;
}

void
Muxer::stampOutputPacket(Container::Stream* stream, MediaPacket* packet) {

  if (!packet) {
    VS_THROW(HumbleInvalidArgument("no packet specified"));
  }

  // let's record these for posterity. We'll output it later.
  RefPointer<MediaPacket> origPacket = MediaPacket::make(packet, false);

  if (packet->getStreamIndex() < 0)
    packet->setStreamIndex(stream->getIndex());
  else if (packet->getStreamIndex() != stream->getIndex())
    VS_THROW(HumbleInvalidArgument::make("Packet with stream index %d unexpectedly passed to stream #%d",
                                   packet->getStreamIndex(), stream->getIndex()));

  AVStream* avStream = stream->getCtx();

  io::humble::ferry::RefPointer<Rational> thisBase = Rational::make(avStream->time_base.num, avStream->time_base.den);
  io::humble::ferry::RefPointer<Rational> packetBase = packet->getTimeBase();
  if (!thisBase || !packetBase) {
    VS_THROW(HumbleRuntimeError("no timebases on either stream or packet"));
  }
  if (thisBase->compareTo(packetBase.value()) == 0) {
    // it's already got the right time values
    return;
  }

  int64_t duration = packet->getDuration();
  int64_t dts = packet->getDts();
  int64_t pts = packet->getPts();

  if (duration >= 0)
    duration = thisBase->rescale(duration, packetBase.value(),
      Rational::ROUND_DOWN);

  if (pts != Global::NO_PTS) {
    pts = thisBase->rescale(pts, packetBase.value(), Rational::ROUND_DOWN);
  }
  if (dts != Global::NO_PTS) {
    dts = thisBase->rescale(dts, packetBase.value(), Rational::ROUND_DOWN);
    if (stream->getLastDts() != Global::NO_PTS && dts == stream->getLastDts()) {
      // adjust for rounding; we never want to insert a frame that
      // is not monotonically increasing.  Note we only do this if
      // we're off by one; that's because we ROUND_DOWN and then assume
      // that can be off by at most one.  If we're off by more than one
      // then it's really an error on the person muxing to this stream.
      dts = stream->getLastDts() + 1;
      // and round up pts
      if (pts != Global::NO_PTS) ++pts;
      // and if after all that adjusting, pts is less than dts
      // let dts win.
      if (pts == Global::NO_PTS || pts < dts) pts = dts;
    }
    stream->setLastDts(dts);
  }

  packet->setDuration(duration);
  packet->setPts(pts);
  packet->setDts(dts);
  packet->setTimeBase(thisBase.value());
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
