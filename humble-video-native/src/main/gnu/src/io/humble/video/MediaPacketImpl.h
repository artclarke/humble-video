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

#ifndef MEDIAPACKETIMPL_H_
#define MEDIAPACKETIMPL_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Buffer.h>
#include <io/humble/video/MediaPacket.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO MediaPacketImpl : public io::humble::video::MediaPacket
{
  public:
    /* The default make() method doesn't add a payload */
    VS_JNIUTILS_REFCOUNTED_OBJECT(MediaPacketImpl);
  public:
    /* This make allocates a default payload of size payloadSize */
    static MediaPacketImpl* make(int32_t payloadSize);
    /* This make a packet that just wraps a given Buffer */
    static MediaPacketImpl* make(io::humble::ferry::Buffer* buffer);
    /* This makes a packet wrapping the buffer in another packet and copying
     * it's settings
     */
    static MediaPacketImpl* make(MediaPacketImpl* packet, bool);
#ifndef SWIG
    static MediaPacketImpl* make(AVPacket *packet);
#endif // ! SWIG
  public:

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
    virtual io::humble::ferry::Buffer* getData();
    virtual void reset(int32_t payloadSize);
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
    virtual void setData(io::humble::ferry::Buffer* buffer);

    virtual int32_t getNumSideDataElems();
    virtual io::humble::ferry::Buffer* getSideData(int32_t n);
    virtual SideDataType getSideDataType(int32_t n);

    virtual Coder* getCoder();
    virtual void setCoder(Coder*);
#ifndef SWIG
    AVPacket *getCtx() { return mPacket; }
    /*
     * Unfortunately people can do a getCtx() and have
     * FFMPEG update the buffers without us knowing.  When
     * that happens, we need them to tell us so we can update
     * our own buffer state.
     */
    void wrapAVPacket(AVPacket* pkt);
    void wrapBuffer(io::humble::ferry::Buffer *buffer);
  virtual int64_t logMetadata(char* buffer, size_t len);

#endif // ! SWIG

  protected:
    MediaPacketImpl();
    virtual ~MediaPacketImpl();
  private:
    AVPacket* mPacket;
    io::humble::ferry::RefPointer<Rational> mTimeBase;
    io::humble::ferry::RefPointer<Coder> mCoder;
    bool mIsComplete;

};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAPACKETIMPL_H_ */
