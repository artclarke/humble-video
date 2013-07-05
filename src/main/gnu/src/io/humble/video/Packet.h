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

#ifndef PACKET_H_
#define PACKET_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefPointer.h>

namespace io {
namespace humble {
namespace video {

/**
 * A packet of data that was read from a {@link Source} or
 * will be written to a {@link Sink}.
 */
class VS_API_HUMBLEVIDEO Packet : public io::humble::ferry::RefCounted
{
  /*
   * Note: This class is a pure-virtual interface. Actual
   * implementation should be in PacketImpl
   */
public:

  /**
   * Create a new {@link Source}
   */
  static Packet*
  make();

protected:
  Packet();
  virtual
  ~Packet();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* PACKET_H_ */
