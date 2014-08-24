/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef INDEXENTRYIMPL_H_
#define INDEXENTRYIMPL_H_

#include <io/humble/video/FfmpegIncludes.h>
#include <io/humble/video/IndexEntry.h>

namespace io { namespace humble { namespace video
{

class IndexEntryImpl : public IndexEntry
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(IndexEntryImpl);
public:
  static IndexEntryImpl* make(int64_t position, int64_t timeStamp,
      int32_t flags, int32_t size, int32_t minDistance);
  virtual int64_t getPosition();
  virtual int64_t getTimeStamp();
  virtual int32_t getFlags();
  virtual int32_t getSize();
  virtual int32_t getMinDistance();
  virtual bool    isKeyFrame();

protected:
  IndexEntryImpl();
  virtual ~IndexEntryImpl();
  
private:
  AVIndexEntry mEntry;
};

}}}

#endif /* INDEXENTRYIMPL_H_ */
