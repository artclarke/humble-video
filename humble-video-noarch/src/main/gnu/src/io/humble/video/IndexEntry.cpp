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

#include <io/humble/video/IndexEntry.h>
#include <io/humble/video/IndexEntryImpl.h>

namespace io { namespace humble { namespace video
{

IndexEntry :: IndexEntry()
{
  
}

IndexEntry :: ~IndexEntry()
{

}

IndexEntry*
IndexEntry::make(int64_t position, int64_t timeStamp,
    int32_t flags, int32_t size, int32_t minDistance)
{
  return IndexEntryImpl::make(position, timeStamp,
      flags, size, minDistance);
}

}}}
