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
#include <io/humble/video/SourceStreamImpl.h>
#include <io/humble/video/customio/URLProtocolHandler.h>

#include <vector>

namespace io {
namespace humble {
namespace video {

class SourceImpl : public io::humble::video::Source
{
public:
  static SourceImpl* make();

  virtual State
  getState() { return mState; }

  virtual SourceFormat *
  getSourceFormat() { return mFormat.get(); }

  virtual int32_t
  setInputBufferLength(int32_t size);

  virtual int32_t
  getInputBufferLength();

  virtual int32_t
  open(const char *url, SourceFormat* format, bool streamsCanBeAddedDynamically,
      bool queryStreamMetaData, KeyValueBag* options,
      KeyValueBag* optionsNotSet);

  virtual int32_t
  close();

  virtual int32_t
  getNumStreams();

  virtual ContainerStream* getStream(int32_t i) { return getSourceStream(i); }
  virtual SourceStream*
  getSourceStream(int32_t streamIndex);

  virtual int32_t
  read(Packet *packet);

  virtual int32_t
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

  virtual int32_t
  play();

  virtual int32_t
  pause();

protected:
  SourceImpl();
  virtual
  ~SourceImpl();
  virtual AVFormatContext* getFormatCtx();

private:
  int32_t doOpen(const char*, AVDictionary**);
  int32_t doCloseFileHandles(AVIOContext* pb);
  int32_t doSetupSourceStreams();
  Container::State mState;
  bool mStreamInfoGotten;
  AVFormatContext* mCtx;
  int32_t mReadRetryMax;
  int32_t mInputBufferLength;
  io::humble::video::customio::URLProtocolHandler* mIOHandler;
  io::humble::ferry::RefPointer<SourceFormat> mFormat;
  // We do pointer to RefPointers to avoid too many
  // acquire() / release() cycles as the vector manages
  // itself.
  std::vector<
    io::humble::ferry::RefPointer<SourceStreamImpl>*
    > mStreams;
  int32_t mNumStreams;
  io::humble::ferry::RefPointer<KeyValueBag> mMetaData;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* SOURCEIMPL_H_ */
