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
#include "Packet.h"
#include "PacketImpl.h"

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

Packet::Packet() {
}

Packet::~Packet() {
}

Packet*
Packet :: make()
{
  Global::init();
  return PacketImpl::make();
}

Packet*
Packet::make(IBuffer* buffer)
{
  Global::init();
  if (!buffer)
    throw std::invalid_argument("no buffer");

  return PacketImpl::make(buffer);
}

Packet*
Packet :: make(Packet* packet, bool copyData)
{
  Global::init();
  return PacketImpl::make(dynamic_cast<PacketImpl*>(packet), copyData);
}

Packet*
Packet :: make(int32_t size)
{
  Global::init();
  Packet* retval = make();
  if (retval) {
    if (retval->allocateNewPayload(size) < 0)
      VS_REF_RELEASE(retval);
  }
  return retval;
}



} /* namespace video */
} /* namespace humble */
} /* namespace io */
