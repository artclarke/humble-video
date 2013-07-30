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

#include "Global.h"
#include "MediaPacket.h"
#include "MediaPacketImpl.h"

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

MediaPacket::MediaPacket() {
}

MediaPacket::~MediaPacket() {
}

MediaPacket*
MediaPacket :: make()
{
  Global::init();
  return MediaPacketImpl::make();
}

MediaPacket*
MediaPacket::make(Buffer* buffer)
{
  Global::init();
  if (!buffer)
    throw std::invalid_argument("no buffer");

  return MediaPacketImpl::make(buffer);
}

MediaPacket*
MediaPacket :: make(MediaPacket* packet, bool copyData)
{
  Global::init();
  return MediaPacketImpl::make(dynamic_cast<MediaPacketImpl*>(packet), copyData);
}

MediaPacket*
MediaPacket :: make(int32_t size)
{
  Global::init();
  MediaPacket* retval = MediaPacketImpl::make(size);
  return retval;
}



} /* namespace video */
} /* namespace humble */
} /* namespace io */
