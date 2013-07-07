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

#include <io/humble/ferry/Logger.h>
#include <io/humble/video/IndexEntryImpl.h>

#include <cstring>
#include <stdexcept>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video
{

IndexEntryImpl :: IndexEntryImpl()
{
}

IndexEntryImpl :: ~IndexEntryImpl()
{
  
}

int64_t
IndexEntryImpl::getPosition()
{
  return mEntry.pos;
}

int64_t
IndexEntryImpl::getTimeStamp()
{
  return mEntry.timestamp;
}

int32_t
IndexEntryImpl::getFlags()
{
  return mEntry.flags;
}

int32_t
IndexEntryImpl::getSize()
{
  return mEntry.size;
}

int32_t
IndexEntryImpl::getMinDistance()
{
  return mEntry.min_distance;
}

bool
IndexEntryImpl::isKeyFrame()
{
  return mEntry.flags & AVINDEX_KEYFRAME;
}

IndexEntryImpl*
IndexEntryImpl::make(int64_t position, int64_t timeStamp,
    int32_t flags, int32_t size, int32_t minDistance)
{
  IndexEntryImpl* retval = make();
  if (retval) {
    retval->mEntry.pos = position;
    retval->mEntry.timestamp = timeStamp;
    retval->mEntry.flags = flags;
    retval->mEntry.size = size;
    retval->mEntry.min_distance = minDistance;
  }
  return retval;
}
}}}
