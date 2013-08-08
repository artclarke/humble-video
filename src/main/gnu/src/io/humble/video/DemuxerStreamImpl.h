/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HUMBLESTREAMIMPL_H_
#define HUMBLESTREAMIMPL_H_

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/DemuxerStream.h>
#include <io/humble/video/FfmpegIncludes.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Decoder.h>

namespace io { namespace humble { namespace video
{
  class Container;
  class MediaPacket;

  class DemuxerStreamImpl : public DemuxerStream
  {
    VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(DemuxerStreamImpl)
  public:

    // IStream
    virtual int getIndex();
    virtual int getId();
    virtual Rational * getFrameRate();
    virtual Rational * getTimeBase();
    virtual int64_t getStartTime();
    virtual int64_t getDuration();
    virtual int64_t getCurrentDts();
    virtual int getNumIndexEntries();
    virtual int64_t getNumFrames();

    // Not for calling from Java
    static DemuxerStreamImpl * make(Container* container, AVStream *, AVCodec*);

    // The StreamCoder will call this if it needs to
    virtual void setTimeBase(Rational *);
    virtual void setFrameRate(Rational *);

    // Called by the managing container when it is closed
    // at this point this stream is no longer valid.
    virtual int containerClosed(Container* container);

    virtual int32_t acquire();
    virtual int32_t release();

    virtual Rational* getSampleAspectRatio();
    virtual void setSampleAspectRatio(Rational* newRatio);

    virtual Container* getContainer();
    virtual ContainerStream::ParseType getParseType();
    virtual void setParseType(ContainerStream::ParseType type);

    virtual AVStream* getAVStream() { return mStream; }
    
    virtual KeyValueBag* getMetaData();
    virtual void setMetaData(KeyValueBag* metaData);

    virtual int32_t stampOutputPacket(MediaPacket* packet);
    virtual IndexEntry* findTimeStampEntryInIndex(
        int64_t wantedTimeStamp, int32_t flags);
    virtual int32_t findTimeStampPositionInIndex(
        int64_t wantedTimeStamp, int32_t flags);
    virtual IndexEntry* getIndexEntry(int32_t position);
    virtual int32_t addIndexEntry(IndexEntry* entry);
    void setId(int32_t id);
    virtual MediaPacket* getAttachedPic();
    virtual ContainerStream::Disposition getDisposition();
    virtual Decoder* getDecoder();

  protected:
    DemuxerStreamImpl();
    virtual ~DemuxerStreamImpl();

  private:
    void reset();
    AVStream *mStream;
    Container* mContainer;
    io::humble::ferry::RefPointer<KeyValueBag> mMetaData;
    int64_t mLastDts;
    io::humble::ferry::RefPointer<Decoder> mDecoder;
  };

}}}

#endif /*HUMBLESTREAMIMPL_H_*/
