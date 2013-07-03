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
 * SourceImpl.h
 *
 *  Created on: Jul 2, 2013
 *      Author: aclarke
 */

#ifndef SOURCEIMPL_H_
#define SOURCEIMPL_H_

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Source.h>
namespace io {
namespace humble {
namespace video {

class SourceImpl : public io::humble::video::Source
{
public:
  static SourceImpl* make();
  virtual State
  getState() = 0;

  virtual InputFormat *
  getInputFormat() = 0;

  virtual int32_t
  setInputBufferLength(int32_t size)=0;

  virtual int32_t
  getInputBufferLength()=0;

  virtual int32_t
  open(const char *url, InputFormat* format, bool streamsCanBeAddedDynamically,
      bool queryStreamMetaData, KeyValueBag* options,
      KeyValueBag* optionsNotSet);

  virtual int32_t
  close()=0;

  virtual int32_t
  getNumStreams()=0;

  virtual Stream*
  getStream(int32_t streamIndex)=0;

  virtual int32_t
  read(Packet *packet)=0;

  virtual int32_t
  queryStreamMetaData()=0;

  virtual int64_t
  getDuration()=0;

  virtual int64_t
  getStartTime()=0;

  virtual int64_t
  getFileSize()=0;

  virtual int32_t
  getBitRate()=0;

  virtual int32_t
  getFlags()=0;

  virtual void
  setFlags(int32_t newFlags) = 0;

  virtual bool
  getFlag(Flag flag) = 0;

  virtual void
  setFlag(Flag flag, bool value) = 0;

  virtual const char*
  getURL()=0;

  virtual int32_t
  getReadRetryCount()=0;

  virtual void
  setReadRetryCount(int32_t count)=0;

  virtual bool
  canStreamsBeAddedDynamically()=0;

  virtual KeyValueBag*
  getMetaData()=0;

  virtual int32_t
  setForcedAudioCodec(Codec::ID id)=0;

  virtual int32_t
  setForcedVideoCodec(Codec::ID id)=0;

  virtual int32_t
  setForcedSubtitleCodec(Codec::ID id)=0;

  virtual int32_t
  getMaxDelay()=0;

  virtual int32_t
  seek(int32_t stream_index, int64_t min_ts, int64_t ts,
      int64_t max_ts, int32_t flags)=0;

  virtual int32_t
  play()=0;

  virtual int32_t
  pause()=0;

protected:
  SourceImpl();
  virtual
  ~SourceImpl();
  virtual void* getCtx() { return mCtx; }
private:
  Container::State mState;
  AVFormatContext* mCtx;
  int32_t mReadRetryMax;
  int32_t mInputBufferLength;
  io::humble::video::customio::URLProtocolHandler* mIOHandler;
  io::humble::ferry::RefPointer<InputFormat> mFormat;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* SOURCEIMPL_H_ */
