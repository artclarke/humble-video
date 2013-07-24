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
 * MediaSubtitleImpl.h
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#ifndef MEDIASUBTITLEIMPL_H_
#define MEDIASUBTITLEIMPL_H_

#include "MediaSubtitle.h"

namespace io {
namespace humble {
namespace video {

class MediaSubtitleImpl : public io::humble::video::MediaSubtitle
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaSubtitleImpl);
protected:
  MediaSubtitleImpl();
  virtual
  ~MediaSubtitleImpl();
  virtual bool isComplete() { return mComplete; }
  virtual AVFrame* getCtx() { return 0; }
private:
  bool mComplete;
  AVSubtitle* mCtx;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIASUBTITLEIMPL_H_ */
