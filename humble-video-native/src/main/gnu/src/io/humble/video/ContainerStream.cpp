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

#include "ContainerStream.h"

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>

#include <io/humble/video/Global.h>
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Container.h>
#include <io/humble/video/KeyValueBagImpl.h>
#include <io/humble/video/MediaPacketImpl.h>
#include <io/humble/video/IndexEntry.h>

#include "FfmpegIncludes.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

ContainerStream::ContainerStream(Container *container, int32_t index) {
  if ((!index) < 0) {
    VS_THROW(HumbleInvalidArgument("no stream"));
  }
  if (!container) {
    VS_THROW(HumbleInvalidArgument("no container"));
  }
  mIndex = index;
  mContainer.reset(container, true); // acquire it so that this object lives.
}

ContainerStream::~ContainerStream() {
  // nothing should be required.
}

Container*
ContainerStream::getContainer() {
  return mContainer.get();
}

AVStream*
ContainerStream::getCtx() {
  Container::Stream* stream = mContainer->getStream(mIndex);
  AVStream* avStream = stream ? stream->getCtx() : 0;
  if (!avStream) {
    // this exception allows us avoid lots of null checking logic below.
    VS_THROW(
        HumbleRuntimeError::make(
            "attempt to get information from stream %d in un-open container.",
            mIndex));
  }
  return avStream;
}
ContainerStream::Disposition
ContainerStream::getDisposition() {
  return (ContainerStream::Disposition) getCtx()->disposition;
}

MediaPacket*
ContainerStream::getAttachedPic() {
  RefPointer<MediaPacketImpl> retval;
  AVStream* stream = getCtx();

  if (stream && stream->attached_pic.size > 0) {
    retval = MediaPacketImpl::make();
    retval->wrapAVPacket(&stream->attached_pic);
  }
  return retval.get();

}

int
ContainerStream::getIndex() {
  return getCtx()->index;
}

int
ContainerStream::getId() {
  return getCtx()->id;
}

Rational *
ContainerStream::getFrameRate() {
  Rational * result = 0;
  AVStream* stream = getCtx();

  if (stream->avg_frame_rate.den != 0) {
    result = Rational::make(stream->avg_frame_rate.num,
        stream->avg_frame_rate.den);
  }
  return result;
}

Rational *
ContainerStream::getTimeBase() {
  AVStream* stream = getCtx();
  return Rational::make(stream->time_base.num, stream->time_base.den);
}
void
ContainerStream::setTimeBase(Rational* src) {
  AVStream* stream = getCtx();

  if (src) {
    stream->time_base.den = src->getDenominator();
    stream->time_base.num = src->getNumerator();
  }
}
void
ContainerStream::setFrameRate(Rational* src) {
  AVStream* stream = getCtx();

  if (src) {
    stream->r_frame_rate.den = src->getDenominator();
    stream->r_frame_rate.num = src->getNumerator();
  }
}

int64_t
ContainerStream::getStartTime() {
  return getCtx()->start_time;
}

int64_t
ContainerStream::getDuration() {
  return getCtx()->duration;
}

int64_t
ContainerStream::getCurrentDts() {
  return getCtx()->cur_dts;
}

int64_t
ContainerStream::getNumFrames() {
  return getCtx()->nb_frames;
}

int
ContainerStream::getNumIndexEntries() {
  return getCtx()->nb_index_entries;
}

Rational*
ContainerStream::getSampleAspectRatio() {
  AVStream* stream = getCtx();

  return Rational::make(stream->sample_aspect_ratio.num,
      stream->sample_aspect_ratio.den);
}

void
ContainerStream::setSampleAspectRatio(Rational* aNewValue) {
  AVStream* stream = getCtx();

  if (aNewValue) {
    stream->sample_aspect_ratio.num = aNewValue->getNumerator();
    stream->sample_aspect_ratio.den = aNewValue->getDenominator();
  }
  return;
}

ContainerStream::ParseType
ContainerStream::getParseType() {
  return (ContainerStream::ParseType) getCtx()->need_parsing;
}

void
ContainerStream::setParseType(ContainerStream::ParseType type) {
  getCtx()->need_parsing = (enum AVStreamParseType) type;
}

KeyValueBag*
ContainerStream::getMetaData() {
  // make a read-only copy so when libav deletes the
  // input version we don't delete our copy
  return KeyValueBagImpl::make(getCtx()->metadata);
}

void
ContainerStream::setMetaData(KeyValueBag * copy) {
  KeyValueBagImpl* data = dynamic_cast<KeyValueBagImpl*>(getMetaData());
  if (data) {
    data->copy(copy);
    // release for the get above
    data->release();
  }
  return;
}

IndexEntry*
ContainerStream::findTimeStampEntryInIndex(int64_t wantedTimeStamp,
    int32_t flags) {
  int32_t index = findTimeStampPositionInIndex(wantedTimeStamp, flags);
  // getIndexEntry will check for a negative index and return null if so
  return getIndexEntry(index);
}
int32_t
ContainerStream::findTimeStampPositionInIndex(int64_t wantedTimeStamp,
    int32_t flags) {
  return av_index_search_timestamp(getCtx(), wantedTimeStamp, flags);
}

IndexEntry*
ContainerStream::getIndexEntry(int32_t index) {
  AVStream* stream = getCtx();

  IndexEntry* retval = 0;
  if (stream->index_entries && index >= 0 && index < stream->nb_index_entries) {
    AVIndexEntry* entry = &(stream->index_entries[index]);
    if (entry) {
      retval = IndexEntry::make(entry->pos, entry->timestamp, entry->flags,
          entry->size, entry->min_distance);
    }
  }
  return retval;
}
int32_t
ContainerStream::addIndexEntry(IndexEntry* entry) {
  if (!entry) return -1;
  return av_add_index_entry(getCtx(), entry->getPosition(),
      entry->getTimeStamp(), entry->getSize(), entry->getMinDistance(),
      entry->getFlags());
}

void
ContainerStream::setId(int32_t aId) {
  getCtx()->id = aId;
}

}
}
}
