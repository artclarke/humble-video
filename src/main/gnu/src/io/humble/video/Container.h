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
 *
 * Container.h
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/ContainerFormat.h>

namespace io {
namespace humble {
namespace video {

/**
 * A Container for Media data. This is an abstract class and
 * cannot be instantiated on its own.
 */
class Stream;
class VS_API_HUMBLEVIDEO Container : public io::humble::ferry::RefCounted
{
public:
  virtual int32_t getNumStreams()=0;
  virtual Stream* getStream(int32_t i) = 0;
  virtual ContainerFormat* getFormat() = 0;

  virtual int32_t getNumProperties();
  virtual Property* getPropertyMetaData(int32_t propertyNo);
  virtual Property* getPropertyMetaData(const char *name);

  virtual int32_t setProperty(const char* name, const char* value);
  virtual int32_t setProperty(const char* name, double value);
  virtual int32_t setProperty(const char* name, int64_t value);
  virtual int32_t setProperty(const char* name, bool value);
  virtual int32_t setProperty(const char* name, Rational *value);

  virtual char * getPropertyAsString(const char* name);
  virtual double getPropertyAsDouble(const char* name);
  virtual int64_t getPropertyAsLong(const char* name);
  virtual  Rational *getPropertyAsRational(const char* name);
  virtual bool getPropertyAsBoolean(const char* name);

protected:
  Container();
  virtual
  ~Container();
private:
  AVFormatContext *mCtx;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CONTAINER_H_ */
