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

#include "MediaPacketImpl.h"
#include "AVBufferSupport.h"

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/Buffer.h>
#include <io/humble/video/VideoExceptions.h>

// for memset
#include <cstring>
#include <stdexcept>

VS_LOG_SETUP(VS_CPP_PACKAGE.MediaPacket);

using namespace io::humble::ferry;
  
namespace io { namespace humble { namespace video {

  MediaPacketImpl::MediaPacketImpl()
  {
    mPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    if (!mPacket)
      throw std::bad_alloc();
    
    av_init_packet(mPacket);
    // initialize because ffmpeg doesn't
    mPacket->data = 0;
    mPacket->size = 0;
    // set this to -1 if it's not set.
    mPacket->stream_index = -1;
    mIsComplete = false;
  }

  MediaPacketImpl::~MediaPacketImpl()
  {
    av_free_packet(mPacket);
    av_freep(&mPacket);
  }

  int64_t
  MediaPacketImpl::getPts()
  {
    return mPacket->pts;
  }
  
  void
  MediaPacketImpl::setPts(int64_t aPts)
  {
    mPacket->pts = aPts;
  }
  
  int64_t
  MediaPacketImpl::getDts()
  {
    return mPacket->dts;
  }
  
  void
  MediaPacketImpl::setDts(int64_t aDts)
  {
    mPacket->dts = aDts;
  }
  
  int32_t
  MediaPacketImpl::getSize()
  {
    return mPacket->size;
  }
  int32_t
  MediaPacketImpl::getMaxSize()
  {
    return (mPacket->buf ? mPacket->buf->size : -1);
  }
  int32_t
  MediaPacketImpl::getStreamIndex()
  {
    return mPacket->stream_index;
  }
  int32_t
  MediaPacketImpl::getFlags()
  {
    return mPacket->flags;
  }
  bool
  MediaPacketImpl::isKeyPacket()
  {
    return mPacket->flags & AV_PKT_FLAG_KEY;
  }

  void
  MediaPacketImpl::setKeyPacket(bool bKeyPacket)
  {
    if (bKeyPacket)
      mPacket->flags |= AV_PKT_FLAG_KEY;
    else
      mPacket->flags = 0;
  }

  void
  MediaPacketImpl::setFlags(int32_t flags)
  {
    mPacket->flags = flags;
  }

  void
  MediaPacketImpl::setComplete(bool complete, int32_t size)
  {
    mIsComplete = complete;
    if (mIsComplete)
    {
      mPacket->size = size;
    }
  }
  
  void
  MediaPacketImpl::setStreamIndex(int32_t streamIndex)
  {
    mPacket->stream_index = streamIndex;
  }
  int64_t
  MediaPacketImpl::getDuration()
  {
    return mPacket->duration;
  }
  
  void
  MediaPacketImpl::setDuration(int64_t duration)
  {
    mPacket->duration = duration;
  }
  
  int64_t
  MediaPacketImpl::getPosition()
  {
    return mPacket->pos;
  }
  
  void
  MediaPacketImpl::setPosition(int64_t position)
  {
    mPacket->pos = position;
  }
  
  Buffer *
  MediaPacketImpl::getData()
  {
    if (!mPacket->data || !mPacket->buf)
      return 0;

    return AVBufferSupport::wrapAVBuffer(this, mPacket->buf);
  }
  
  void
  MediaPacketImpl::wrapAVPacket(AVPacket* pkt)
  {
    if (!pkt) {
      VS_THROW(HumbleInvalidArgument("no packet"));
    }
    av_free_packet(mPacket);
    av_init_packet(mPacket);
    
    int32_t retval = av_copy_packet(mPacket, pkt);
    FfmpegException::check(retval, "Failed to copy packet ");
    
    // And assume we're now complete.
    setComplete(true, mPacket->size);
  }

  MediaPacketImpl*
  MediaPacketImpl::make (int32_t payloadSize)
  {
    RefPointer<MediaPacketImpl> retval;
    retval = MediaPacketImpl::make();
    FfmpegException::check(av_new_packet(retval->mPacket, payloadSize), "could not allocate packet ");
    return retval.get();
  }
  
  MediaPacketImpl*
  MediaPacketImpl::make (Buffer* buffer)
  {
    RefPointer<MediaPacketImpl> retval;
    retval = MediaPacketImpl::make();
    retval->wrapBuffer(buffer);
    return retval.get();
  }
  
  MediaPacketImpl*
  MediaPacketImpl::make (MediaPacketImpl* packet, bool copyData)
  {
    RefPointer<MediaPacketImpl> retval;
    RefPointer<Rational> timeBase;
    if (!packet) {
      VS_THROW(HumbleInvalidArgument("no packet"));
    }

      // use the nice copy method.
      retval = make();
      int32_t r = av_copy_packet(retval->mPacket, packet->mPacket);
      FfmpegException::check(r, "could not copy packet ");
      int32_t numBytes = packet->getSize();
      if (copyData && numBytes > 0)
      {
        if (!retval || !retval->mPacket || !retval->mPacket->data) {
          VS_THROW(HumbleBadAlloc());
        }

        // we don't just want to reference count the data -- we want
        // to copy it. So we're going to create a new copy.
        RefPointer<Buffer> copy = Buffer::make(retval.value(), numBytes + FF_INPUT_BUFFER_PADDING_SIZE);
        uint8_t* data = (uint8_t*)copy->getBytes(0, numBytes);

        // copy the data into our Buffer backed data
        memcpy(data, packet->mPacket->data,
            numBytes);

        // now, release the reference currently in the packet
        if (retval->mPacket->buf)
          av_buffer_unref(&retval->mPacket->buf);
        retval->mPacket->buf = AVBufferSupport::wrapBuffer(copy.value());
        // and set the data member to the copy
        retval->mPacket->data = retval->mPacket->buf->data;
        retval->mPacket->size = numBytes;

      }
      // separate here to catch addRef()
      timeBase = packet->getTimeBase();
      retval->setTimeBase(timeBase.value());

      retval->setComplete(retval->mPacket->size > 0,
          retval->mPacket->size);
    
    return retval.get();
  }
  
  MediaPacketImpl*
  MediaPacketImpl::make(AVPacket* packet) {
    RefPointer<MediaPacketImpl> retval = make();
    retval->wrapAVPacket(packet);
    return retval.get();
  }


  void
  MediaPacketImpl::reset(int32_t payloadSize)
  {
    int32_t e=-1;
    av_free_packet(mPacket);
    if (payloadSize > 0)
      e = av_new_packet(mPacket, payloadSize);
    else {
      av_init_packet(mPacket);
      e = 0;
    }
    FfmpegException::check(e, "Could not reset packet.");

    // set this to -1 if it's not set.
    mPacket->stream_index = -1;
  }

  void
  MediaPacketImpl::setData(Buffer* buffer)
  {
    wrapBuffer(buffer);
  }
  
  void
  MediaPacketImpl::wrapBuffer(Buffer *buffer)
  {
    if (!buffer)
      return;

    // let's create a av buffer reference
    if (mPacket->buf)
      av_buffer_unref(&mPacket->buf);
    mPacket->buf = AVBufferSupport::wrapBuffer(buffer);
    if (mPacket->buf) {
      mPacket->size = mPacket->buf->size;
      mPacket->data = mPacket->buf->data;;
      // And assume we're now complete.
      setComplete(true, mPacket->size);
    } else {
      VS_LOG_ERROR("Error wrapping buffer");
      throw std::bad_alloc();
    }
  }
  bool
  MediaPacketImpl::isComplete()
  {
    return mIsComplete && mPacket->data && mPacket->size;
  }

  int64_t
  MediaPacketImpl::getConvergenceDuration()
  {
    return mPacket->convergence_duration;
  }
  
  void
  MediaPacketImpl::setConvergenceDuration(int64_t duration)
  {
    mPacket->convergence_duration = duration;
  }

  int32_t
  MediaPacketImpl::getNumSideDataElems()
  {
    return mPacket->side_data_elems;
  }

  static void
  MediaPacketImpl_freeSideData(void *, void* closure) {
    MediaPacketImpl* packet = (MediaPacketImpl*)closure;
    VS_REF_RELEASE(packet);
  }
  Buffer*
  MediaPacketImpl::getSideData(int32_t n) {
    if(n < 0 || n > getNumSideDataElems()) {
      throw HumbleInvalidArgument("n outside of bounds");
    }
    if (!mPacket->side_data)
      throw HumbleRuntimeError("no data where data expected");

    // now wrap the data in an Buffer, but also reference the
    // containing packet so the data doesn't get freed before
    // we're done.
    return Buffer::make(this, mPacket->side_data[n].data, mPacket->side_data[n].size,
        MediaPacketImpl_freeSideData, this);
  }
  MediaPacket::SideDataType
  MediaPacketImpl::getSideDataType(int32_t n) {
    if(n < 0 || n > getNumSideDataElems()) {
      throw HumbleInvalidArgument("n outside of bounds");
    }
    if (!mPacket->side_data)
      throw HumbleRuntimeError("no data where data expected");
    return (MediaPacket::SideDataType)mPacket->side_data[n].type;
  }
  Coder*
  MediaPacketImpl::getCoder() {
    return mCoder.get();
  }
  void
  MediaPacketImpl::setCoder(Coder* coder) {
    mCoder.reset(coder, true);
  }

}}}
