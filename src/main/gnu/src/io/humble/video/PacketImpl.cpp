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

#include "PacketImpl.h"

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/Buffer.h>

// for memset
#include <cstring>
#include <stdexcept>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;
  
namespace io { namespace humble { namespace video {

  PacketImpl::PacketImpl()
  {
    mPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    if (!mPacket)
      throw std::bad_alloc();
    
    av_init_packet(mPacket);
    // initialize because ffmpeg doesn't
    mPacket->data = 0;
    mPacket->size = 0;
    mIsComplete = false;
  }

  PacketImpl::~PacketImpl()
  {
    av_free_packet(mPacket);
    av_freep(&mPacket);
  }

  int64_t
  PacketImpl::getPts()
  {
    return mPacket->pts;
  }
  
  void
  PacketImpl::setPts(int64_t aPts)
  {
    mPacket->pts = aPts;
  }
  
  int64_t
  PacketImpl::getDts()
  {
    return mPacket->dts;
  }
  
  void
  PacketImpl::setDts(int64_t aDts)
  {
    mPacket->dts = aDts;
  }
  
  int32_t
  PacketImpl::getSize()
  {
    return mPacket->size;
  }
  int32_t
  PacketImpl::getMaxSize()
  {
    return (mPacket->buf ? mPacket->buf->size : -1);
  }
  int32_t
  PacketImpl::getStreamIndex()
  {
    return mPacket->stream_index;
  }
  int32_t
  PacketImpl::getFlags()
  {
    return mPacket->flags;
  }
  bool
  PacketImpl::isKeyPacket()
  {
    return mPacket->flags & AV_PKT_FLAG_KEY;
  }

  void
  PacketImpl::setKeyPacket(bool bKeyPacket)
  {
    if (bKeyPacket)
      mPacket->flags |= AV_PKT_FLAG_KEY;
    else
      mPacket->flags = 0;
  }

  void
  PacketImpl::setFlags(int32_t flags)
  {
    mPacket->flags = flags;
  }

  void
  PacketImpl::setComplete(bool complete, int32_t size)
  {
    mIsComplete = complete;
    if (mIsComplete)
    {
      mPacket->size = size;
    }
  }
  
  void
  PacketImpl::setStreamIndex(int32_t streamIndex)
  {
    mPacket->stream_index = streamIndex;
  }
  int64_t
  PacketImpl::getDuration()
  {
    return mPacket->duration;
  }
  
  void
  PacketImpl::setDuration(int64_t duration)
  {
    mPacket->duration = duration;
  }
  
  int64_t
  PacketImpl::getPosition()
  {
    return mPacket->pos;
  }
  
  void
  PacketImpl::setPosition(int64_t position)
  {
    mPacket->pos = position;
  }
  
  IBuffer *
  PacketImpl::getData()
  {
    if (!mPacket->data || !mPacket->buf)
      return 0;

    // create a new reference
    AVBufferRef * b = av_buffer_ref(mPacket->buf);
    // and create an IBuffer wrapping it.
    return IBuffer::make(this, mPacket->data, mPacket->size,
        IBufferFreeFunc, b);
  }
  
  void
  PacketImpl::wrapAVPacket(AVPacket* pkt)
  {
    VS_ASSERT(mPacket, "No packet?");
    av_free_packet(mPacket);
    av_init_packet(mPacket);
    
    int32_t retval = av_copy_packet(mPacket, pkt);
    VS_ASSERT(retval >= 0, "Failed to copy packet");
    if (retval < 0) {
      VS_LOG_ERROR("Failed to copy packet");
      throw std::bad_alloc();
    }
    
    // And assume we're now complete.
    setComplete(true, mPacket->size);
  }

  PacketImpl*
  PacketImpl::make (int32_t payloadSize)
  {
    PacketImpl* retval= 0;
    try {
      retval = PacketImpl::make();
      if (av_new_packet(retval->mPacket, payloadSize) < 0)
      {
        throw std::bad_alloc();
      }
    }
    catch (std::bad_alloc & e)
    {
      VS_REF_RELEASE(retval);
      throw e;
    }

    return retval;
  }
  
  PacketImpl*
  PacketImpl::make (IBuffer* buffer)
  {
    PacketImpl *retval= 0;
    retval = PacketImpl::make();
    if (retval)
    {
      retval->wrapBuffer(buffer);
    }
    return retval;
  }
  
  PacketImpl*
  PacketImpl::make (PacketImpl* packet, bool copyData)
  {
    PacketImpl* retval=0;
    RefPointer<Rational> timeBase;
    try
    {
      if (!packet)
        throw std::runtime_error("need packet to copy");

      // use the nice copy method.
      retval = make();
      int32_t r = av_copy_packet(retval->mPacket, packet->mPacket);
      if (r < 0) {
        VS_LOG_ERROR("Could not copy packet");
        throw std::bad_alloc();
      }
      int32_t numBytes = packet->getSize();
      if (copyData && numBytes > 0)
      {
        if (!retval || !retval->mPacket || !retval->mPacket->data)
          throw std::bad_alloc();

        // we don't just want to reference count the data -- we want
        // to copy it. So we're going to create a new copy.
        RefPointer<IBuffer> copy = IBuffer::make(retval, numBytes + FF_INPUT_BUFFER_PADDING_SIZE);
        if (!copy)
          throw std::bad_alloc();
        uint8_t* data = (uint8_t*)copy->getBytes(0, numBytes);

        // copy the data into our IBuffer backed data
        memcpy(data, packet->mPacket->data,
            numBytes);

        // now, release the reference currently in the packet
        if (retval->mPacket->buf)
          av_buffer_unref(&retval->mPacket->buf);
        // create a new reference wrapping our IBuffer (and create a reference
        // to delete
        copy->acquire();
        retval->mPacket->buf = av_buffer_create(data, copy->getBufferSize(),
            AVBufferRefFreeFunc, copy.value(), 0);
        // and set the data member to the copy
        retval->mPacket->data = data;

      }
      // separate here to catch addRef()
      timeBase = packet->getTimeBase();
      retval->setTimeBase(timeBase.value());

      retval->setComplete(retval->mPacket->size > 0,
          retval->mPacket->size);
    }
    catch (std::exception &e)
    {
      VS_REF_RELEASE(retval);
    }
    
    return retval;
  }
  

  int32_t
  PacketImpl::allocateNewPayload(int32_t payloadSize)
  {
    av_free_packet(mPacket);
    return av_new_packet(mPacket, payloadSize);
  }

  void
  PacketImpl::setData(IBuffer* buffer)
  {
    wrapBuffer(buffer);
  }
  
  void
  PacketImpl::wrapBuffer(IBuffer *buffer)
  {
    if (!buffer)
      return;
    int32_t size = buffer->getBufferSize()-FF_INPUT_BUFFER_PADDING_SIZE;
    if (size <= 0)
      throw std::bad_alloc();

    uint8_t* data = (uint8_t*)buffer->getBytes(0, size);

    buffer->acquire(); // reference for av_buffer_create to manage.
    // let's create a av buffer reference
    if (mPacket->buf)
      av_buffer_unref(&mPacket->buf);
    mPacket->buf = av_buffer_create(
        data,
        size,
        PacketImpl::AVBufferRefFreeFunc,
        buffer,
        0);
    mPacket->size = size;
    mPacket->data = data;
    // And assume we're now complete.
    setComplete(true, size);
  }
  bool
  PacketImpl::isComplete()
  {
    return mIsComplete && mPacket->data;
  }
  
  void
  PacketImpl::AVBufferRefFreeFunc(void * closure, uint8_t * buf)
  {
    IBuffer* b = (IBuffer*)closure;
    // We know that FFMPEG allocated this with av_malloc, but
    // that might change in future versions; so this is
    // inherently somewhat dangerous.
    (void) buf;
    if (b)
      b->release();
  }
  void
  PacketImpl::IBufferFreeFunc(void * buf, void * closure)
  {
    AVBufferRef *b = (AVBufferRef*) closure;
    (void) buf;
    av_buffer_unref(&b);
  }

  int64_t
  PacketImpl::getConvergenceDuration()
  {
    return mPacket->convergence_duration;
  }
  
  void
  PacketImpl::setConvergenceDuration(int64_t duration)
  {
    mPacket->convergence_duration = duration;
  }
}}}
