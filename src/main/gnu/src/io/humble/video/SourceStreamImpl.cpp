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

#include <cstring>
#include <stdexcept>

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>

#include <io/humble/video/Global.h>
#include <io/humble/video/SourceStreamImpl.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Container.h>
#include <io/humble/video/KeyValueBagImpl.h>
#include <io/humble/video/MediaPacketImpl.h>
#include <io/humble/video/IndexEntry.h>
#include <io/humble/video/Decoder.h>

#include "FfmpegIncludes.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

DemuxerStreamImpl::DemuxerStreamImpl() {
  mStream = 0;
  mContainer = 0;
  mLastDts = Global::NO_PTS;
}

DemuxerStreamImpl::~DemuxerStreamImpl() {
  reset();
}

MediaPacket*
DemuxerStreamImpl::getAttachedPic() {
  MediaPacketImpl* retval = 0;

  if (mStream && mStream->attached_pic.size > 0) {
    retval = MediaPacketImpl::make();
    if (retval) {
      retval->wrapAVPacket(&mStream->attached_pic);
    }
  }
  return retval;

}

ContainerStream::Disposition
DemuxerStreamImpl::getDisposition() {
  return
      mStream ?
          (ContainerStream::Disposition) mStream->disposition :
          ContainerStream::DISPOSITION_DEFAULT;
}

void
DemuxerStreamImpl::reset() {
  VS_ASSERT("should be a valid object", getCurrentRefCount() >= 1);
  mMetaData.reset();
  // As of recent (March 2011) builds of FFmpeg, Stream objects
  // are cleaned up by the new avformat_free_context method in the
  // Container, so the outbound check for freeing memory is no
  // longer required.
//    VS_REF_RELEASE(mCoder);
  mStream = 0;
  // We don't keep a reference to the container to avoid a ref-count loop
  // and so we don't release.
  mContainer = 0;
}

DemuxerStreamImpl*
DemuxerStreamImpl::make(Container *container, AVStream * aStream,
    AVCodec* /* avCodec */) {
  // note: make will acquire this for us.
  DemuxerStreamImpl *newStream = 0;
  if (aStream) {
    try {
      newStream = make();
      newStream->mStream = aStream;

#if 0
      newStream->mCoder = StreamCoder::make(
          aStream->codec,
          avCodec,
          newStream);
#endif
      newStream->mContainer = container;
    } catch (std::bad_alloc & e) {
      VS_REF_RELEASE(newStream);
      throw e;
    }
  }
  return newStream;
}

int
DemuxerStreamImpl::getIndex() {
  return (mStream ? mStream->index : -1);
}

int
DemuxerStreamImpl::getId() {
  return (mStream ? mStream->id : -1);
}

#if 0
StreamCoder *
DemuxerStreamImpl::getStreamCoder()
{
  StreamCoder *retval = 0;

  // acquire a reference for the caller
  retval = mCoder;
  VS_REF_ACQUIRE(retval);

  return retval;
}
#endif

Rational *
DemuxerStreamImpl::getFrameRate() {
  Rational * result = 0;
  if (mStream && mStream->avg_frame_rate.den != 0) {
    result = Rational::make(mStream->avg_frame_rate.num,
        mStream->avg_frame_rate.den);
  }
  return result;
}

Rational *
DemuxerStreamImpl::getTimeBase() {
  Rational * result = 0;
  if (mStream) {
    result = Rational::make(mStream->time_base.num, mStream->time_base.den);
  }
  return result;
}
void
DemuxerStreamImpl::setTimeBase(Rational* src) {
  if (mStream && src) {
    mStream->time_base.den = src->getDenominator();
    mStream->time_base.num = src->getNumerator();
  }
}
void
DemuxerStreamImpl::setFrameRate(Rational* src) {
  if (mStream && src) {
    mStream->r_frame_rate.den = src->getDenominator();
    mStream->r_frame_rate.num = src->getNumerator();
  }
}

int64_t
DemuxerStreamImpl::getStartTime() {
  return (mStream ? mStream->start_time : Global::NO_PTS);
}

int64_t
DemuxerStreamImpl::getDuration() {
  return (mStream ? mStream->duration : Global::NO_PTS);
}

int64_t
DemuxerStreamImpl::getCurrentDts() {
  return (mStream ? mStream->cur_dts : Global::NO_PTS);
}

int64_t
DemuxerStreamImpl::getNumFrames() {
  return (mStream ? mStream->nb_frames : 0);
}
int
DemuxerStreamImpl::getNumIndexEntries() {
  return (mStream ? mStream->nb_index_entries : 0);
}
int
DemuxerStreamImpl::containerClosed(Container *) {
  // let the coder know we're closed.
//    if (mCoder)
//      mCoder->streamClosed(this);
  reset();
  return 0;
}

int32_t
DemuxerStreamImpl::acquire() {
  int retval = 0;
  retval = RefCounted::acquire();
  VS_LOG_TRACE("Acquired %p: %d", this, retval);
  return retval;
}

int32_t
DemuxerStreamImpl::release() {
  int retval = 0;
  retval = RefCounted::release();
  VS_LOG_TRACE("Released %p: %d", this, retval);
  return retval;
}

Rational*
DemuxerStreamImpl::getSampleAspectRatio() {
  Rational* retval = 0;
  if (mStream) {
    retval = Rational::make(mStream->sample_aspect_ratio.num,
        mStream->sample_aspect_ratio.den);
  }
  return retval;
}

void
DemuxerStreamImpl::setSampleAspectRatio(Rational* aNewValue) {
  if (aNewValue && mStream) {
    mStream->sample_aspect_ratio.num = aNewValue->getNumerator();
    mStream->sample_aspect_ratio.den = aNewValue->getDenominator();
  }
  return;
}

Container*
DemuxerStreamImpl::getContainer() {
  // add ref for caller
  VS_REF_ACQUIRE(mContainer);
  return mContainer;
}

#if 0
int32_t
DemuxerStreamImpl::setStreamCoder(StreamCoder *coder)
{
  return setStreamCoder(coder, true);
}
int32_t
DemuxerStreamImpl::setStreamCoder(StreamCoder *aCoder, bool assumeOnlyStream)
{
  int32_t retval = -1;
  try
  {
    if (mCoder && mCoder->isOpen())
    throw std::runtime_error("cannot call setStreamCoder when current coder is open");

    if (!aCoder)
    throw std::runtime_error("cannot set to a null stream coder");

    StreamCoder *coder = dynamic_cast<StreamCoder*>(aCoder);
    if (!coder)
    throw std::runtime_error("StreamCoder is not of expected underlying C++ type");

    // Close the old stream coder
    if (mCoder)
    {
      mCoder->streamClosed(this);
    }

    if (coder->setStream(this, assumeOnlyStream) < 0)
    throw std::runtime_error("StreamCoder doesn't like this stream");

    VS_REF_RELEASE(mCoder);
    mCoder = coder;
    VS_REF_ACQUIRE(mCoder);
    retval = 0;
  }
  catch (std::exception & e)
  {
    VS_LOG_ERROR("Error: %s", e.what());
    retval = -1;
  }
  return retval;
}
#endif

ContainerStream::ParseType
DemuxerStreamImpl::getParseType() {
  if (mStream) {
    return (ContainerStream::ParseType) mStream->need_parsing;
  } else {
    return ContainerStream::PARSE_NONE;
  }
}

void
DemuxerStreamImpl::setParseType(ContainerStream::ParseType type) {
  if (mStream) {
    mStream->need_parsing = (enum AVStreamParseType) type;
  }
}

KeyValueBag*
DemuxerStreamImpl::getMetaData() {
  if (!mMetaData && mStream) {
#if 0
    if (mDirection == DemuxerStreamImpl::OUTBOUND)
    mMetaData = MetaData::make(&mStream->metadata);
    else
#endif
    // make a read-only copy so when libav deletes the
    // input version we don't delete our copy
    mMetaData = KeyValueBagImpl::make(mStream->metadata);
  }
  return mMetaData.get();
}

void
DemuxerStreamImpl::setMetaData(KeyValueBag * copy) {
  KeyValueBagImpl* data = dynamic_cast<KeyValueBagImpl*>(getMetaData());
  if (data) {
    data->copy(copy);
    // release for the get above
    data->release();
  }
  return;
}

int32_t
DemuxerStreamImpl::stampOutputPacket(MediaPacket* packet) {
  if (!packet) return -1;

//    VS_LOG_DEBUG("input:  duration: %lld; dts: %lld; pts: %lld;",
//        packet->getDuration(), packet->getDts(), packet->getPts());

  // Always just reset this; cheaper than checking if it's
  // already set
  packet->setStreamIndex(this->getIndex());

  io::humble::ferry::RefPointer<Rational> thisBase = getTimeBase();
  io::humble::ferry::RefPointer<Rational> packetBase = packet->getTimeBase();
  if (!thisBase || !packetBase) return -1;
  if (thisBase->compareTo(packetBase.value()) == 0) {
//      VS_LOG_DEBUG("Same timebase: %d/%d vs %d/%d",
//          thisBase->getNumerator(), thisBase->getDenominator(),
//          packetBase->getNumerator(), packetBase->getDenominator());
    // it's already got the right time values
    return 0;
  }

  int64_t duration = packet->getDuration();
  int64_t dts = packet->getDts();
  int64_t pts = packet->getPts();

  if (duration >= 0) duration = thisBase->rescale(duration, packetBase.value(),
      Rational::ROUND_DOWN);

  if (pts != Global::NO_PTS) {
    pts = thisBase->rescale(pts, packetBase.value(), Rational::ROUND_DOWN);
  }
  if (dts != Global::NO_PTS) {
    dts = thisBase->rescale(dts, packetBase.value(), Rational::ROUND_DOWN);
    if (mLastDts != Global::NO_PTS && dts == mLastDts) {
      // adjust for rounding; we never want to insert a frame that
      // is not monotonically increasing.  Note we only do this if
      // we're off by one; that's because we ROUND_DOWN and then assume
      // that can be off by at most one.  If we're off by more than one
      // then it's really an error on the person muxing to this stream.
      dts = mLastDts + 1;
      // and round up pts
      if (pts != Global::NO_PTS) ++pts;
      // and if after all that adjusting, pts is less than dts
      // let dts win.
      if (pts == Global::NO_PTS || pts < dts) pts = dts;
    }
    mLastDts = dts;
  }

//    VS_LOG_DEBUG("output: duration: %lld; dts: %lld; pts: %lld;",
//        duration, dts, pts);
  packet->setDuration(duration);
  packet->setPts(pts);
  packet->setDts(dts);
  packet->setTimeBase(thisBase.value());
//    VS_LOG_DEBUG("Reset timebase: %d/%d",
//        thisBase->getNumerator(), thisBase->getDenominator());
  return 0;
}

IndexEntry*
DemuxerStreamImpl::findTimeStampEntryInIndex(int64_t wantedTimeStamp,
    int32_t flags) {
  int32_t index = findTimeStampPositionInIndex(wantedTimeStamp, flags);
  // getIndexEntry will check for a negative index and return null if so
  return getIndexEntry(index);
}
int32_t
DemuxerStreamImpl::findTimeStampPositionInIndex(int64_t wantedTimeStamp,
    int32_t flags) {
  int retval = -1;
  if (mStream) {
    retval = av_index_search_timestamp(mStream, wantedTimeStamp, flags);
  }
  return retval;
}

IndexEntry*
DemuxerStreamImpl::getIndexEntry(int32_t index) {
  IndexEntry* retval = 0;
  if (mStream->index_entries && index >= 0
      && index < mStream->nb_index_entries) {
    AVIndexEntry* entry = &(mStream->index_entries[index]);
    if (entry) {
      retval = IndexEntry::make(entry->pos, entry->timestamp, entry->flags,
          entry->size, entry->min_distance);
    }
  }
  return retval;
}
int32_t
DemuxerStreamImpl::addIndexEntry(IndexEntry* entry) {
  if (!entry) return -1;
  if (!mStream) return -1;
  return av_add_index_entry(mStream, entry->getPosition(),
      entry->getTimeStamp(), entry->getSize(), entry->getMinDistance(),
      entry->getFlags());
}

void
DemuxerStreamImpl::setId(int32_t aId) {
  if (!mStream) return;
  mStream->id = aId;
}

Decoder*
DemuxerStreamImpl::getDecoder() {
  if (!mDecoder) {
    if (mStream->codec) {
      // make a copy of the decoder so we decouple it from the container
      // completely
      RefPointer<Codec> codec = Codec::findDecodingCodec((Codec::ID)mStream->codec->codec_id);
      if (!codec)
        throw HumbleRuntimeError("could not find decoding codec");
      mDecoder = Decoder::make(codec.value(), mStream->codec);
    }
  }
  return mDecoder.get();
}
}
}
}
