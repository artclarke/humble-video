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
 * Container.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#include "Container.h"
#include "PropertyImpl.h"
#include <io/humble/ferry/Logger.h>

using namespace io::humble::video::customio;

VS_LOG_SETUP(VS_CPP_PACKAGE);

extern "C"
{
/** Some static functions used by custom IO
 */
int
Container_url_read(void*h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_read(buf, size);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_read(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}
int
Container_url_write(void*h, unsigned char* buf, int size)
{
  int retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_write(buf, size);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_write(%p, %d) ==> %d", h, buf, size,
      retval);
  return retval;
}

int64_t
Container_url_seek(void*h, int64_t position, int whence)
{
  int64_t retval = -1;
  try
  {
    URLProtocolHandler* handler = (URLProtocolHandler*) h;
    if (handler)
      retval = handler->url_seek(position, whence);
  } catch (...)
  {
    retval = -1;
  }
  VS_LOG_TRACE("URLProtocolHandler[%p]->url_seek(%p, %lld) ==> %d", h, position,
      whence, retval);
  return retval;
}

}
namespace io
{
namespace humble
{
namespace video
{

Container::Container()
{
}

Container::~Container()
{
  VS_LOG_DEBUG("Container destroyed");
}

int32_t
Container::getNumProperties()
{
  return PropertyImpl::getNumProperties(getCtx());
}

Property*
Container::getPropertyMetaData(int32_t propertyNo)
{
  return PropertyImpl::getPropertyMetaData(getCtx(), propertyNo);
}

Property*
Container::getPropertyMetaData(const char *name)
{
  return PropertyImpl::getPropertyMetaData(getCtx(), name);
}

int32_t
Container::setProperty(MetaData* valuesToSet, MetaData* valuesNotFound)
{
  return PropertyImpl::setProperty(getCtx(), valuesToSet, valuesNotFound);
}

int32_t
Container::setProperty(const char* aName, const char *aValue)
{
  return PropertyImpl::setProperty(getCtx(), aName, aValue);
}

int32_t
Container::setProperty(const char* aName, double aValue)
{
  return PropertyImpl::setProperty(getCtx(), aName, aValue);
}

int32_t
Container::setProperty(const char* aName, int64_t aValue)
{
  return PropertyImpl::setProperty(getCtx(), aName, aValue);
}

int32_t
Container::setProperty(const char* aName, bool aValue)
{
  return PropertyImpl::setProperty(getCtx(), aName, aValue);
}

int32_t
Container::setProperty(const char* aName, Rational *aValue)
{
  return PropertyImpl::setProperty(getCtx(), aName, aValue);
}

char*
Container::getPropertyAsString(const char *aName)
{
  return PropertyImpl::getPropertyAsString(getCtx(), aName);
}

double
Container::getPropertyAsDouble(const char *aName)
{
  return PropertyImpl::getPropertyAsDouble(getCtx(), aName);
}

int64_t
Container::getPropertyAsLong(const char *aName)
{
  return PropertyImpl::getPropertyAsLong(getCtx(), aName);
}

Rational*
Container::getPropertyAsRational(const char *aName)
{
  return PropertyImpl::getPropertyAsRational(getCtx(), aName);
}

bool
Container::getPropertyAsBoolean(const char *aName)
{
  return PropertyImpl::getPropertyAsBoolean(getCtx(), aName);
}

int32_t
Container::getFlags()
{
  int32_t flags = getCtx()->flags;
  // remove custom io if set
  flags &= ~(AVFMT_FLAG_CUSTOM_IO);
  return flags;
}

void
Container::setFlags(int32_t newFlags)
{
  getCtx()->flags = newFlags;
  // force custom io
  getCtx()->flags |= AVFMT_FLAG_CUSTOM_IO;
}

bool
Container::getFlag(Container::Flag flag)
{
  return getCtx()->flags & flag;
}

void
Container::setFlag(Container::Flag flag, bool value)
{
  if (value)
  {
    getCtx()->flags |= flag;
  } else
  {
    getCtx()->flags &= (~flag);
  }

}

const char*
Container::getURL()
{
  return getCtx()->filename;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
