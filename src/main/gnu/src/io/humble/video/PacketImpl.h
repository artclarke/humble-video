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

#ifndef PACKETIMPL_H_
#define PACKETIMPL_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/IBuffer.h>
#include <io/humble/video/Packet.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO PacketImpl : public io::humble::video::Packet
{
  public:
    /* The default make() method doesn't add a payload */
    VS_JNIUTILS_REFCOUNTED_OBJECT(PacketImpl);
  public:
    /* This make allocates a default payload of size payloadSize */
    static PacketImpl* make(int32_t payloadSize);
    /* This make a packet that just wraps a given IBuffer */
    static PacketImpl* make(io::humble::ferry::IBuffer* buffer);
    /* This makes a packet wrapping the buffer in another packet and copying
     * it's settings
     */
    static PacketImpl* make(PacketImpl* packet, bool);
  public:

    // Data
    virtual int64_t getTimeStamp() { return getDts(); }
    virtual void setTimeStamp(int64_t aTimeStamp) { setDts(aTimeStamp); }
    virtual bool isKey() { return isKeyPacket(); }
    virtual Rational* getTimeBase() { return mTimeBase.get(); }
    virtual void setTimeBase(Rational *aBase) { mTimeBase.reset(aBase, true); }
    
    virtual int64_t getPts();
    virtual int64_t getDts();
    virtual int32_t getSize();
    virtual int32_t getMaxSize();

    virtual int32_t getStreamIndex();
    virtual int32_t getFlags();
    virtual bool isKeyPacket();
    virtual int64_t getDuration();
    virtual int64_t getPosition();
    virtual io::humble::ferry::IBuffer* getData();
    virtual int32_t reset(int32_t payloadSize);
    virtual bool isComplete();
    
    virtual void setKeyPacket(bool keyPacket);
    virtual void setFlags(int32_t flags);
    virtual void setPts(int64_t pts);
    virtual void setDts(int64_t dts);
    virtual void setComplete(bool complete, int32_t size);
    virtual void setStreamIndex(int32_t streamIndex);
    virtual void setDuration(int64_t duration);
    virtual void setPosition(int64_t position);
    virtual int64_t getConvergenceDuration();
    virtual void setConvergenceDuration(int64_t duration);
    virtual void setData(io::humble::ferry::IBuffer* buffer);

#ifndef SWIG
    AVPacket *getCtx() { return mPacket; }
    /*
     * Unfortunately people can do a getCtx() and have
     * FFMPEG update the buffers without us knowing.  When
     * that happens, we need them to tell us so we can update
     * our own buffer state.
     */
    void wrapAVPacket(AVPacket* pkt);
    void wrapBuffer(io::humble::ferry::IBuffer *buffer);
    // Used by FFmpeg AVBufferRefs when they think the underlying
    // buffer is to be freed. We'll just release the extra add ref we
    // added to mBuffer.
    static void AVBufferRefFreeFunc(void *closure, uint8_t *data);
    static void IBufferFreeFunc(void *buf, void *closure);
#endif // ! SWIG

  protected:
    PacketImpl();
    virtual ~PacketImpl();
  private:
    AVPacket* mPacket;
    io::humble::ferry::RefPointer<Rational> mTimeBase;
    bool mIsComplete;

};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* PACKETIMPL_H_ */
