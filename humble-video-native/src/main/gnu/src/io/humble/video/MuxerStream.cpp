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
 * MuxerStream.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#include "MuxerStream.h"
#include "Muxer.h"

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

MuxerStream::MuxerStream(Container* c, int32_t index) : ContainerStream(c, index) {

}

MuxerStream::~MuxerStream() {
}

Muxer*
MuxerStream::getMuxer() {
  return dynamic_cast<Muxer*>(getContainer());
}

Encoder*
MuxerStream::getEncoder() {
  return 0;
}

MuxerStream*
MuxerStream::make(Container* container, int32_t index) {
  RefPointer<MuxerStream> r;
  r.reset(new MuxerStream(container, index), true);
  return r.get();
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
