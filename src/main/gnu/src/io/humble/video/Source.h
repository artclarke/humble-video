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

#ifndef SOURCE_H_
#define SOURCE_H_

#include <io/humble/video/Container.h>
#include <io/humble/video/InputFormat.h>

#include <io/humble/ferry/RefPointer.h>

namespace io {
namespace humble {
namespace video {

class Source : public io::humble::video::Container
{
VS_JNIUTILS_REFCOUNTED_OBJECT(Source)
public:

  /** {@inheritDoc} */
  virtual State
  getState() {
    return mState;
  }

  /** {@inheritDoc} */
  virtual ContainerFormat *
  getFormat() {
    return mFormat.get();
  }
  /** {@inheritDoc} */
  virtual int32_t
  close();

  /** {@inheritDoc} */
  virtual KeyValueBag*
  getMetaData();
  /** {@inheritDoc} */
  virtual int64_t
  getFileSize();
protected:
#ifndef SWIG
  virtual void*
  getCtx() {
    return mCtx;
  }
#endif // ! SWIG
  Source();
  virtual
  ~Source();
  AVFormatContext *mCtx;
  State mState;
  io::humble::ferry::RefPointer<InputFormat> mFormat;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* SOURCE_H_ */
