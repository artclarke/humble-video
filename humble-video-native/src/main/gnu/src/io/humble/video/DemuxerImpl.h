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
 * DemuxerImpl.h
 *
 *  Created on: Jul 2, 2013
 *      Author: aclarke
 */

#ifndef DEMUXERIMPL_H_
#define DEMUXERIMPL_H_

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Demuxer.h>
#include <io/humble/video/DemuxerStream.h>
#include <io/humble/video/customio/URLProtocolHandler.h>

#include <vector>

namespace io {
namespace humble {
namespace video {

class DemuxerImpl : public io::humble::video::Demuxer
{
public:
  static DemuxerImpl* make();

  virtual State
  getState() { return mState; }

  virtual DemuxerFormat *
  getFormat() { return mFormat.get(); }

  virtual void
  setInputBufferLength(int32_t size);

  virtual int32_t
  getInputBufferLength();

  virtual void
  open(const char *url, DemuxerFormat* format, bool streamsCanBeAddedDynamically,
      bool queryStreamMetaData, KeyValueBag* options,
      KeyValueBag* optionsNotSet);

  virtual void
  close();

  virtual int32_t
  getNumStreams();

  virtual DemuxerStream*
  getStream(int32_t streamIndex);

  virtual int32_t
  read(MediaPacket *packet);

  virtual void
  queryStreamMetaData();

  virtual int64_t
  getDuration();

  virtual int64_t
  getStartTime();

  virtual int64_t
  getFileSize();

  virtual int32_t
  getBitRate();

  virtual int32_t
  getFlags();

  virtual void
  setFlags(int32_t newFlags);

  virtual bool
  getFlag(Flag flag);

  virtual void
  setFlag(Flag flag, bool value);

  virtual const char*
  getURL();

  virtual int32_t
  getReadRetryCount();

  virtual void
  setReadRetryCount(int32_t count);

  virtual bool
  canStreamsBeAddedDynamically();

  virtual KeyValueBag*
  getMetaData();

  virtual int32_t
  setForcedAudioCodec(Codec::ID id);

  virtual int32_t
  setForcedVideoCodec(Codec::ID id);

  virtual int32_t
  setForcedSubtitleCodec(Codec::ID id);

  virtual int32_t
  getMaxDelay();

  virtual int32_t
  seek(int32_t stream_index, int64_t min_ts, int64_t ts,
      int64_t max_ts, int32_t flags);

  virtual void
  play();

  virtual void
  pause();

protected:
  DemuxerImpl();
  virtual
  ~DemuxerImpl();
  virtual AVFormatContext* getFormatCtx();

private:
  int32_t doOpen(const char*, AVDictionary**);
  int32_t doCloseFileHandles(AVIOContext* pb);
  State mState;
  bool mStreamInfoGotten;
  AVFormatContext* mCtx;
  int32_t mReadRetryMax;
  int32_t mInputBufferLength;
  io::humble::video::customio::URLProtocolHandler* mIOHandler;
  io::humble::ferry::RefPointer<DemuxerFormat> mFormat;
  io::humble::ferry::RefPointer<KeyValueBag> mMetaData;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* DEMUXERIMPL_H_ */
