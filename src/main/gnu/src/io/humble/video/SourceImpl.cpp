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
 * SourceImpl.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: aclarke
 */
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/JNIHelper.h>
#include "SourceImpl.h"
#include "Global.h"
#include <io/humble/video/customio/URLProtocolManager.h>
#include "KeyValueBagImpl.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::video::customio;
using namespace io::humble::ferry;

extern "C"
{
/** Some static functions used by custom IO
 */
int
Container_url_read(void*h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_read(buf, size);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_read(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}
int
Container_url_write(void*h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_write(buf, size);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_write(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}

int64_t
Container_url_seek(void*h, int64_t position, int whence)
{
  int64_t retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_seek(position, whence);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_seek(%p, %lld) ==> %d", h, position,
      whence, retval);
  return retval;
}

}

namespace io {
namespace humble {
namespace video {

SourceImpl::SourceImpl() {
  mStreams = 0;
  mStreamInfoGotten = 0;
  mReadRetryMax = 1;
  mInputBufferLength = 0;
  mIOHandler = 0;
  mCtx = avformat_alloc_context();
  if (!mCtx) throw std::bad_alloc();
  // Set up thread interrupt capabilities
  mCtx->interrupt_callback.callback = Global::avioInterruptCB;
  mCtx->interrupt_callback.opaque = this;
  // we're going to always clean up the avio structures ourselves.
  mCtx->flags |= AVFMT_FLAG_CUSTOM_IO;
  mState = Container::STATE_INITED;
}

SourceImpl::~SourceImpl() {
  if (mIOHandler) {
    if (mCtx->pb)
      av_freep(&mCtx->pb->buffer);
    av_freep(&mCtx->pb);
    delete mIOHandler;
    mIOHandler = 0;
  }
  avformat_free_context(mCtx);
}

SourceImpl*
SourceImpl::make() {
  Global::init();
  SourceImpl *retval = new SourceImpl();
  VS_REF_ACQUIRE(retval);
  return retval;
}

int32_t
SourceImpl::open(const char *url, InputFormat* format,
    bool streamsCanBeAddedDynamically, bool queryMetaData,
    KeyValueBag* options, KeyValueBag* optionsNotSet) {

  int retval = -1;
  if (mState != Container::STATE_INITED) {
    VS_LOG_DEBUG("Open can only be called when container is in init state. Current state: %d", mState);
    return retval;
  }

  AVDictionary* tmp=0;

  if (format) {
    // acquire a long-lived reference
    mFormat.reset(format, true);
    mCtx->iformat = mFormat->getCtx();
  }

  AVInputFormat* oldFormat = mCtx->iformat;

  // Let's check for custom IO
  mIOHandler = URLProtocolManager::findHandler(
         url,
         URLProtocolHandler::URL_RDONLY_MODE,
         0);

  if (mIOHandler) {
    if (mInputBufferLength <= 0)
      // default to 2k
      mInputBufferLength = 2048;
    // free and realloc the input buffer length
    uint8_t* buffer = (uint8_t*)av_malloc(mInputBufferLength);
    if (!buffer) {
      mState = Container::STATE_ERROR;
      return retval;
    }

    // we will allocate ourselves an io context;
    // ownership of buffer passes here.
    mCtx->pb = avio_alloc_context(
        buffer,
        mInputBufferLength,
        0,
        mIOHandler,
        Container_url_read,
        Container_url_write,
        Container_url_seek);
    if (!mCtx->pb) {
      av_free(buffer);
      mState = Container::STATE_ERROR;
      return retval;
    }
  }
  // Check for passed in options
  KeyValueBagImpl* realOpts = dynamic_cast<KeyValueBagImpl*>(options);
  if (realOpts)
    av_dict_copy(&tmp, realOpts->getDictionary(), 0);

  KeyValueBagImpl* realUnsetOpts = dynamic_cast<KeyValueBagImpl*>(optionsNotSet);
  if (realUnsetOpts)
    realUnsetOpts->copy(tmp);

  // Now call the real open method; this is done
  // in another function to ensure we clean up tmp
  // afterwards.
  retval = doOpen(url, &tmp);
  VS_CHECK_INTERRUPT(retval, true);

  if (retval >= 0) {
    mState = Container::STATE_OPENED;

    if (oldFormat != mCtx->iformat)
      mFormat = InputFormat::make(mCtx->iformat);

    if (streamsCanBeAddedDynamically)
      mCtx->ctx_flags |= AVFMTCTX_NOHEADER;

    if (queryMetaData)
      retval = this->queryStreamMetaData();
  }
  if (tmp)
    av_dict_free(&tmp);
  if (retval < 0)
    mState = Container::STATE_ERROR;
  return retval;
}

int32_t
SourceImpl::setInputBufferLength(int32_t size) {
  if (mState != Container::STATE_INITED)
    return -1;
  if (size < 0)
    return -1;
  int32_t retval = mInputBufferLength;
  mInputBufferLength = size;
  return retval;
}

int32_t
SourceImpl::getInputBufferLength() {
  return mInputBufferLength;
}

int32_t
SourceImpl::getNumStreams() {
  return (int32_t)mCtx->nb_streams;
}

int32_t
SourceImpl::close() {
  int32_t retval=-1;
  switch(mState)
  {
  case Container::STATE_OPENED:
  case Container::STATE_PLAYING:
  case Container::STATE_PAUSED:
    // All these cases are valid.
    break;
  default:
  {
    VS_LOG_WARN("Attempt to close container (%s) when not opened, playing or paused is ignored", getURL());
    return -1;
  }

  }
  // we need to remember the avio context
  AVIOContext* pb = mCtx->pb;

  avformat_close_input(&mCtx);
  if (mIOHandler) {
    retval = mIOHandler->url_close();
    if (retval < 0) {
      VS_LOG_ERROR("Error when closing container (%s): %d", getURL(), retval);
      mState = Container::STATE_ERROR;
    }

    if (pb)
      av_freep(&pb->buffer);
    av_free(pb);
  } else
    retval = avio_close(pb);
  if (mState != Container::STATE_ERROR)
    mState = Container::STATE_CLOSED;
  return retval;
}

Stream*
SourceImpl::getStream(int32_t streamIndex) {
  (void) streamIndex;
  return 0;
}

int32_t
SourceImpl::read(Packet* packet) {
  (void) packet;
  return -1;
}

int32_t
SourceImpl::queryStreamMetaData() {
  int32_t retval = -1;
  if (!mStreamInfoGotten) {
    retval = avformat_find_stream_info(mCtx, 0);
    if (retval >= 0)
      mStreamInfoGotten = true;
  } else
    retval = 0;
  VS_CHECK_INTERRUPT(retval, true);
  return retval;
}

int64_t
SourceImpl::getDuration() {
  return mCtx->duration;
}

int64_t
SourceImpl::getStartTime() {
  return mCtx->start_time;
}

int64_t
SourceImpl::getFileSize() {
  int64_t retval = -1;
  if (mCtx->iformat && (mCtx->iformat->flags & AVFMT_NOFILE))
    retval = 0;
  else {
    retval = avio_size(mCtx->pb);
    retval = FFMAX(0, retval);
  }
  return retval;
}

int32_t
SourceImpl::getBitRate() {
  return mCtx->bit_rate;
}

int32_t
SourceImpl::getFlags() {
  int32_t flags = mCtx->flags;
  // remove custom io if set
  flags &= ~(AVFMT_FLAG_CUSTOM_IO);
  return flags;
}

void
SourceImpl::setFlags(int32_t newFlags) {
  mCtx->flags = newFlags;
  mCtx->flags |= AVFMT_FLAG_CUSTOM_IO;
}

bool
SourceImpl::getFlag(Flag flag) {
  return mCtx->flags & flag;
}

void
SourceImpl::setFlag(Flag flag, bool value) {
  if (value)
    mCtx->flags |= flag;
  else
    mCtx->flags &= (~flag);
}

const char*
SourceImpl::getURL() {
  return mCtx->filename;
}

int32_t
SourceImpl::getReadRetryCount() {
  return mReadRetryMax;
}

void
SourceImpl::setReadRetryCount(int32_t count) {
  if (count >= 0)
    mReadRetryMax = count;
}

bool
SourceImpl::canStreamsBeAddedDynamically() {
  return mCtx->ctx_flags & AVFMTCTX_NOHEADER;
}

KeyValueBag*
SourceImpl::getMetaData() {
  if (!mMetaData)
    mMetaData = KeyValueBagImpl::make(mCtx->metadata);
  return mMetaData.get();
}

int32_t
SourceImpl::setForcedAudioCodec(Codec::ID id) {
  mCtx->audio_codec_id = (enum AVCodecID) id;
  return 0;
}

int32_t
SourceImpl::setForcedVideoCodec(Codec::ID id) {
  mCtx->video_codec_id = (enum AVCodecID) id;
  return 0;
}

int32_t
SourceImpl::setForcedSubtitleCodec(Codec::ID id) {
  mCtx->subtitle_codec_id = (enum AVCodecID)id;
  return 0;
}

int32_t
SourceImpl::getMaxDelay() {
  return mCtx->max_delay;
}

int32_t
SourceImpl::seek(int32_t stream_index, int64_t min_ts, int64_t ts,
    int64_t max_ts, int32_t flags) {
  if (mState != Container::STATE_OPENED)
  {
    VS_LOG_WARN("Can only seek on OPEN (not paused or playing) containers.");
    return -1;
  }
  int32_t retval = avformat_seek_file(mCtx,
      stream_index,
      min_ts,
      ts,
      max_ts,
      flags);
  // TODO: Make sure all FFmpeg input buffers are cleared after seek

  VS_CHECK_INTERRUPT(retval, true);
  return retval;
}

int32_t
SourceImpl::pause() {
  if (mState != Container::STATE_PLAYING)
  {
    VS_LOG_WARN("Can only pause containers in PLAYING state. Current state: %d", mState);
    return -1;
  }
  int32_t retval = av_read_pause(mCtx);
  VS_CHECK_INTERRUPT(retval, true);
  if (retval >= 0)
    mState = Container::STATE_PAUSED;
  return retval;
}

int32_t
SourceImpl::play() {
  if (mState != Container::STATE_PAUSED || mState != Container::STATE_OPENED)
  {
    VS_LOG_WARN("Can only play containers in OPENED or PAUSED states. Current state: %d", mState);
    return -1;
  }
  int32_t retval = av_read_play(mCtx);
  VS_CHECK_INTERRUPT(retval, true);
  if (retval >= 0)
    mState = Container::STATE_PLAYING;
  return retval;
}

int32_t
SourceImpl::doOpen(const char* url, AVDictionary** options)
{
  int32_t retval=0;

  if (mIOHandler)
    retval = mIOHandler->url_open(url, URLProtocolHandler::URL_RDONLY_MODE);

  if (retval >= 0)
    retval = avformat_open_input(&mCtx,
        url,
        mCtx->iformat,
        options);

  return retval;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
