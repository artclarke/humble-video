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
 * Audio.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: aclarke
 */

#include "MediaAudio.h"

namespace io {
namespace humble {
namespace video {

MediaAudio::MediaAudio() {

}

MediaAudio::~MediaAudio() {

}

MediaAudio*
MediaAudio::make(int32_t maxSamples, int32_t channels,
    AudioFormat::Type format) {
  (void) maxSamples;
  (void) channels;
  (void) format;
  return 0;
}

/**
 * Create a MediaAudio using the given buffer.
 *
 * Note: that the {@link IBuffer.getBufferSize()} constraints the max number
 * of samples we can place in here, and HumbleVideo needs to reserve some
 * of the buffer for, um, stuff (assume at least 64 bytes). So {@link #getMaxNumSamples()}
 * may not return as many as you think you can fit in here.
 *
 * @return A {@link MediaAudio} object, or null on failure.
 */
MediaAudio*
MediaAudio::make(io::humble::ferry::IBuffer *buffer, int32_t channels,
    AudioFormat::Type format) {
  (void) buffer;
  (void) channels;
  (void) format;
  return 0;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
