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
 * MediaAudioImpl.h
 *
 *  Created on: Jul 12, 2013
 *      Author: aclarke
 */

#ifndef MEDIAAUDIOIMPL_H_
#define MEDIAAUDIOIMPL_H_

#include "MediaAudio.h"

namespace io {
namespace humble {
namespace video {

class MediaAudioImpl : public io::humble::video::MediaAudio
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaAudioImpl)
public:
  static MediaAudioImpl*
  make(int32_t numSamples, int32_t sampleRate, int32_t channels, AudioChannel::Layout channelLayout,
        AudioFormat::Type format);

  static MediaAudioImpl*
  make(io::humble::ferry::Buffer *buffer, int32_t numSamples, int32_t sampleRate, int32_t channels,
      AudioChannel::Layout channelLayout, AudioFormat::Type format);

  static MediaAudioImpl*
  make(MediaAudioImpl* src, bool copy);

  virtual io::humble::ferry::Buffer*
  getData(int32_t plane);

  virtual int32_t
  getDataPlaneSize(int32_t plane);

  virtual int32_t
  getNumDataPlanes();

  virtual int32_t
  getMaxNumSamples();

  virtual int32_t
  getBytesPerSample();

  virtual void
  setComplete(bool);

  virtual int32_t
  getSampleRate();

  virtual int32_t
  getChannels();

  virtual AudioFormat::Type
  getFormat();

  virtual bool
  isComplete();

  virtual bool
  isKey();

  virtual bool
  isPlanar();

  virtual AudioChannel::Layout getChannelLayout();

  virtual int32_t
  getNumSamples();

  virtual void
  setNumSamples(int32_t numSamples);

  virtual AVFrame* getCtx();
  // Copies data from src into this context, first releasing any memory we have.
  virtual void copy(AVFrame* src, bool complete);
#ifndef SWIG
  virtual int64_t logMetadata(char *, size_t);
#endif
protected:
  MediaAudioImpl();
  virtual
  ~MediaAudioImpl();
private:
  static void
  setBufferType(AudioFormat::Type format,
      io::humble::ferry::Buffer* buffer);

  AVFrame* mFrame;
  bool     mComplete;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAAUDIOIMPL_H_ */
