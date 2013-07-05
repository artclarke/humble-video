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
#include <io/humble/video/Packet.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO PacketImpl : public io::humble::video::Packet
{
public:

  /**
   * Create a new {@link PacketImpl}
   */
  static PacketImpl*
  make();

  virtual int64_t getPts();

  virtual void setPts(int64_t aPts);

  virtual int64_t getDts();

  virtual void setDts(int64_t aDts);

  virtual int32_t getSize()=0;

  virtual int32_t getMaxSize()=0;

  virtual int32_t getStreamIndex()=0;

  virtual int32_t getFlags()=0;

  virtual bool isKeyPacket()=0;

  virtual int64_t getDuration()=0;

  virtual int64_t getPosition()=0;

  virtual void setKeyPacket(bool keyPacket)=0;

  virtual void setFlags(int32_t flags)=0;

  virtual void setStreamIndex(int32_t streamIndex)=0;

  virtual void setDuration(int64_t duration)=0;

  virtual void setPosition(int64_t position)=0;

  virtual int64_t getConvergenceDuration()=0;

  virtual void setConvergenceDuration(int64_t duration)=0;

#ifndef SWIG
  AVPacket* getCtx() { return mPacket; }
#endif
protected:
  PacketImpl();
  virtual
  ~PacketImpl();
private:
  AVPacket* mPacket;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* PACKETIMPL_H_ */
