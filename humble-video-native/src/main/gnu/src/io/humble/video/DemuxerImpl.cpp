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
 * DemuxerImpl.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: aclarke
 */
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/video/customio/URLProtocolManager.h>
#include "Global.h"
#include "DemuxerImpl.h"
#include "MediaPacketImpl.h"
#include "KeyValueBagImpl.h"
#include "VideoExceptions.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::video::customio;
using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

DemuxerImpl::DemuxerImpl() {
  mStreamInfoGotten = 0;
  mReadRetryMax = 1;
  mInputBufferLength = 2048;
  mIOHandler = 0;
  mCtx = avformat_alloc_context();
  if (!mCtx) {
    VS_THROW(HumbleBadAlloc());
  }
  // Set up thread interrupt capabilities
  mCtx->interrupt_callback.callback = Global::avioInterruptCB;
  mCtx->interrupt_callback.opaque = this;
  mState = STATE_INITED;
}

DemuxerImpl::~DemuxerImpl() {
  if (mState == STATE_OPENED ||
      mState == STATE_PLAYING ||
      mState == STATE_PAUSED) {
    VS_LOG_ERROR("Open Demuxer destroyed without Demuxer.close() being called. Closing anyway: %s",
        this->getURL());
    (void) this->close();
  }
  if (mCtx)
    avformat_free_context(mCtx);
}

AVFormatContext*
DemuxerImpl::getFormatCtx() {
  // throw an exception if in wrong state.
  if (mState == STATE_CLOSED || mState == STATE_ERROR || !mCtx) {
    VS_THROW(HumbleRuntimeError::make("Method called on Demuxer in CLOSED or ERROR state."));
  }
  return mCtx;
}

DemuxerImpl*
DemuxerImpl::make() {
  Global::init();
  DemuxerImpl *retval = new DemuxerImpl();
  VS_REF_ACQUIRE(retval);
  return retval;
}

void
DemuxerImpl::open(const char *url, DemuxerFormat* format,
    bool streamsCanBeAddedDynamically, bool queryMetaData,
    KeyValueBag* options, KeyValueBag* optionsNotSet)
{
  AVFormatContext* ctx = this->getFormatCtx();
  int retval = -1;
  if (mState != STATE_INITED) {
    VS_THROW(HumbleRuntimeError::make("Open can only be called when container is in init state. Current state: %d", mState));
  }
  if (!url || !*url) {
    VS_THROW(HumbleInvalidArgument("Open cannot be called with an empty URL"));
  }

  AVDictionary* tmp=0;

  if (format) {
    // acquire a long-lived reference
    mFormat.reset(format, true);
    ctx->iformat = mFormat->getCtx();
  }

  AVInputFormat* oldFormat = ctx->iformat;

  // Let's check for custom IO
  mIOHandler = URLProtocolManager::findHandler(
         url,
         URLProtocolHandler::URL_RDONLY_MODE,
         0);

  if (mIOHandler) {
    ctx->flags |= AVFMT_FLAG_CUSTOM_IO;
    // free and realloc the input buffer length
    uint8_t* buffer = (uint8_t*)av_malloc(mInputBufferLength);
    if (!buffer) {
      mState = STATE_ERROR;
      VS_THROW(HumbleBadAlloc());
    }

    // we will allocate ourselves an io context;
    // ownership of buffer passes here.
    ctx->pb = avio_alloc_context(
        buffer,
        mInputBufferLength,
        0,
        mIOHandler,
        Container::url_read,
        Container::url_write,
        Container::url_seek);
    if (!ctx->pb) {
      av_free(buffer);
      mState = STATE_ERROR;
      VS_THROW(HumbleRuntimeError::make("could not open url due to internal error: %s", url));
    }
  }
  // Check for passed in options
  KeyValueBagImpl* realOpts = dynamic_cast<KeyValueBagImpl*>(options);
  if (realOpts)
    av_dict_copy(&tmp, realOpts->getDictionary(), 0);

  // Now call the real open method; this is done
  // in another function to ensure we clean up tmp
  // afterwards.
  retval = doOpen(url, &tmp);
  KeyValueBagImpl* realUnsetOpts = dynamic_cast<KeyValueBagImpl*>(optionsNotSet);
  if (realUnsetOpts)
    realUnsetOpts->copy(tmp);
  if (tmp)
    av_dict_free(&tmp);

  VS_CHECK_INTERRUPT(true);

  if (retval >= 0) {
    mState = STATE_OPENED;

    if (oldFormat != ctx->iformat)
      mFormat = DemuxerFormat::make(ctx->iformat);

    if (streamsCanBeAddedDynamically)
      ctx->ctx_flags |= AVFMTCTX_NOHEADER;
  }
  if (retval < 0) {
    mState = STATE_ERROR;
    FfmpegException::check(retval, "Error opening url: %s; ", url);
  }
  if (queryMetaData)
    queryStreamMetaData();
  return;
}

void
DemuxerImpl::setInputBufferLength(int32_t size) {
  if (size <= 0)
    VS_THROW(HumbleInvalidArgument("size <= 0"));
  if (mState != STATE_INITED)
    VS_THROW(HumbleRuntimeError("Demuxer object has already been opened"));
  mInputBufferLength = size;
}

int32_t
DemuxerImpl::getInputBufferLength() {
  return mInputBufferLength;
}

int32_t
DemuxerImpl::getNumStreams() {
  if (!(mState == STATE_OPENED ||
      mState == STATE_PLAYING ||
      mState == STATE_PAUSED)) {
    VS_THROW(HumbleRuntimeError("Attempt to query number of streams in Demuxer when not opened, playing or paused is ignored"));
  }
  return Container::getNumStreams();
}

void
DemuxerImpl::close() {
  int32_t retval=-1;
  if (!(mState == STATE_OPENED ||
      mState == STATE_PLAYING ||
      mState == STATE_PAUSED)) {
    VS_THROW(HumbleRuntimeError("Attempt to close container when not opened, playing or paused"));
  }

  // we need to remember the avio context
  AVIOContext* pb = this->getFormatCtx()->pb;

  // set this BEFORE we close the input so that potential races can be minimized.
  mState = STATE_CLOSED;

  avformat_close_input(&mCtx);
  // avformat_close_input frees the context, so...
  mCtx = 0;

  retval = doCloseFileHandles(pb);
  if (retval < 0) {
    mState = STATE_ERROR;
    FfmpegException::check(retval, "Error when closing container (%s): %d", getURL(), retval);
  }
}

int32_t
DemuxerImpl::doCloseFileHandles(AVIOContext* pb)
{
  int32_t retval = -1;
  if (mIOHandler) {
    // make sure all data is pushed out.
    if (pb) avio_flush(pb);
    // close our handle
    retval = mIOHandler->url_close();
    if (pb) av_freep(&pb->buffer);
    av_free(pb);
    delete mIOHandler;
    mIOHandler = 0;
  } else
    retval = 0;
  return retval;
}

DemuxerStream*
DemuxerImpl::getStream(int32_t position) {
  if (!(mState == STATE_OPENED ||
      mState == STATE_PLAYING ||
      mState == STATE_PAUSED)) {
    VS_THROW(HumbleRuntimeError("Attempt to get Demuxer stream from Demuxer when not opened, playing or paused is ignored"));
  }
  // always make a new object, but do not maintain a reference to it in this
  // container to avoid a reference count loop.
  return DemuxerStream::make(this, position);
}

int32_t
DemuxerImpl::read(MediaPacket* ipkt) {
  int32_t retval = -1;
  MediaPacketImpl* pkt = dynamic_cast<MediaPacketImpl*>(ipkt);
  if (pkt)
  {
    pkt->reset(0);
    AVPacket* packet=pkt->getCtx();

    int32_t numReads=0;
    pkt->setComplete(false, pkt->getSize());
    do
    {
      retval = av_read_frame(this->getFormatCtx(),
          packet);
      ++numReads;
    }
    while (retval == AVERROR(EAGAIN) &&
        (mReadRetryMax < 0 || numReads <= mReadRetryMax));

    // and dump it's contents
    VS_LOG_TRACE("read: %lld, %lld, %d, %d, %d, %lld, %lld: %p",
        pkt->getDts(),
        pkt->getPts(),
        pkt->getFlags(),
        pkt->getStreamIndex(),
        pkt->getSize(),
        pkt->getDuration(),
        pkt->getPosition(),
        packet->data);

    // and let's try to set the packet time base if known
    if (retval >= 0) {
      if (pkt->getStreamIndex() >= 0)
      {
        RefPointer<DemuxerStream> stream = this->getStream(pkt->getStreamIndex());
        if (stream)
        {
          RefPointer<Rational> streamBase = stream->getTimeBase();
          if (streamBase)
          {
            pkt->setTimeBase(streamBase.value());
          }
        }
      }
      pkt->setComplete(true, pkt->getSize());
    }
  }
  VS_CHECK_INTERRUPT(true);
  if (retval < 0 && retval != AVERROR_EOF)
    // throw exception in this case
    FfmpegException::check(retval, "exception on read of: %s; ", getURL());
  return retval;
}

void
DemuxerImpl::queryStreamMetaData() {
  if (!(mState == STATE_OPENED ||
      mState == STATE_PLAYING ||
      mState == STATE_PAUSED)) {
    VS_THROW(HumbleRuntimeError("Attempt to query stream information from container when not opened, playing or paused"));
  }
  if (!mStreamInfoGotten) {
    FfmpegException::check(avformat_find_stream_info(this->getFormatCtx(), 0), "could not queryStreamMetaData on: %s; ", getURL());
    mStreamInfoGotten = true;
  }
  Container::doSetupStreams();

  if (getFormatCtx()->nb_streams <= 0)
  {
    VS_LOG_WARN("Could not find streams in input container");
  }
}

int64_t
DemuxerImpl::getDuration() {
  return this->getFormatCtx()->duration;
}

int64_t
DemuxerImpl::getStartTime() {
  return this->getFormatCtx()->start_time;
}

int64_t
DemuxerImpl::getFileSize() {
  int64_t retval = -1;
  AVFormatContext* ctx = this->getFormatCtx();
  if (ctx->iformat && (ctx->iformat->flags & AVFMT_NOFILE))
    retval = 0;
  else {
    retval = avio_size(ctx->pb);
    retval = FFMAX(0, retval);
  }
  return retval;
}

int32_t
DemuxerImpl::getBitRate() {
  return this->getFormatCtx()->bit_rate;
}

int32_t
DemuxerImpl::getFlags() {
  int32_t flags = this->getFormatCtx()->flags;
  return flags;
}

void
DemuxerImpl::setFlags(int32_t newFlags) {
  AVFormatContext* ctx=this->getFormatCtx();
  ctx->flags = newFlags;
  if (mIOHandler)
    ctx->flags |= AVFMT_FLAG_CUSTOM_IO;
}

bool
DemuxerImpl::getFlag(Flag flag) {
  return this->getFormatCtx()->flags & flag;
}

void
DemuxerImpl::setFlag(Flag flag, bool value) {
  AVFormatContext* ctx=this->getFormatCtx();

  if (value)
    ctx->flags |= flag;
  else
    ctx->flags &= (~flag);
}

const char*
DemuxerImpl::getURL() {
  return this->getFormatCtx()->filename;
}

int32_t
DemuxerImpl::getReadRetryCount() {
  return mReadRetryMax;
}

void
DemuxerImpl::setReadRetryCount(int32_t count) {
  if (count >= 0)
    mReadRetryMax = count;
}

bool
DemuxerImpl::canStreamsBeAddedDynamically() {
  return this->getFormatCtx()->ctx_flags & AVFMTCTX_NOHEADER;
}

KeyValueBag*
DemuxerImpl::getMetaData() {
  if (!mMetaData)
    mMetaData = KeyValueBagImpl::make(this->getFormatCtx()->metadata);
  return mMetaData.get();
}

int32_t
DemuxerImpl::setForcedAudioCodec(Codec::ID id) {
  this->getFormatCtx()->audio_codec_id = (enum AVCodecID) id;
  return 0;
}

int32_t
DemuxerImpl::setForcedVideoCodec(Codec::ID id) {
  this->getFormatCtx()->video_codec_id = (enum AVCodecID) id;
  return 0;
}

int32_t
DemuxerImpl::setForcedSubtitleCodec(Codec::ID id) {
  this->getFormatCtx()->subtitle_codec_id = (enum AVCodecID)id;
  return 0;
}

int32_t
DemuxerImpl::getMaxDelay() {
  return this->getFormatCtx()->max_delay;
}

int32_t
DemuxerImpl::seek(int32_t stream_index, int64_t min_ts, int64_t ts,
    int64_t max_ts, int32_t flags) {
  if (mState != STATE_OPENED)
  {
    VS_THROW(HumbleRuntimeError("Can only seek on OPEN (not paused or playing) Demuxers"));
  }
  int32_t retval = avformat_seek_file(this->getFormatCtx(),
      stream_index,
      min_ts,
      ts,
      max_ts,
      flags);
  // TODO: Make sure all FFmpeg input buffers are cleared after seek

  VS_CHECK_INTERRUPT(true);
  return retval;
}

void
DemuxerImpl::pause() {
  if (mState != STATE_PLAYING)
  {
    VS_THROW(HumbleRuntimeError("Can only pause containers in PLAYING state."));
  }
  int32_t retval = av_read_pause(this->getFormatCtx());
  FfmpegException::check(retval, "Could not pause url: %s; ", getURL());
  mState = STATE_PAUSED;
}

void
DemuxerImpl::play() {
  if (mState != STATE_PAUSED || mState != STATE_OPENED)
  {
    VS_THROW(HumbleRuntimeError("Can only play containers in OPENED or PAUSED states"));
  }
  int32_t retval = av_read_play(this->getFormatCtx());
  FfmpegException::check(retval, "Could not play url: %s; ", getURL());
  mState = STATE_PLAYING;
}

int32_t
DemuxerImpl::doOpen(const char* url, AVDictionary** options)
{
  int32_t retval=0;
  AVFormatContext* ctx = this->getFormatCtx();

  if (mIOHandler)
    retval = mIOHandler->url_open(url, URLProtocolHandler::URL_RDONLY_MODE);

  if (retval >= 0) {
    AVIOContext* pb = ctx->pb;
    retval = avformat_open_input(&mCtx,
        url,
        ctx->iformat,
        options);
    if (retval < 0)
      // close open filehandle
      doCloseFileHandles(pb);
  }

  return retval;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
