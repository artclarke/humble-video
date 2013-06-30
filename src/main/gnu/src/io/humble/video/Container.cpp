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

namespace io {
namespace humble {
namespace video {

Container::Container() :
    mCtx(0)
{
}

Container::~Container()
{
}

int32_t
Container :: getNumProperties()
{
  return PropertyImpl::getNumProperties(mCtx);
}

Property*
Container :: getPropertyMetaData(int32_t propertyNo)
{
  return PropertyImpl::getPropertyMetaData(mCtx, propertyNo);
}

Property*
Container :: getPropertyMetaData(const char *name)
{
  return PropertyImpl::getPropertyMetaData(mCtx, name);
}

int32_t
Container :: setProperty(MetaData* valuesToSet, MetaData* valuesNotFound)
{
  return PropertyImpl::setProperty(mCtx, valuesToSet, valuesNotFound);
}

int32_t
Container :: setProperty(const char* aName, const char *aValue)
{
  return PropertyImpl::setProperty(mCtx, aName, aValue);
}

int32_t
Container :: setProperty(const char* aName, double aValue)
{
  return PropertyImpl::setProperty(mCtx, aName, aValue);
}

int32_t
Container :: setProperty(const char* aName, int64_t aValue)
{
  return PropertyImpl::setProperty(mCtx, aName, aValue);
}

int32_t
Container :: setProperty(const char* aName, bool aValue)
{
  return PropertyImpl::setProperty(mCtx, aName, aValue);
}


int32_t
Container :: setProperty(const char* aName, Rational *aValue)
{
  return PropertyImpl::setProperty(mCtx, aName, aValue);
}


char*
Container :: getPropertyAsString(const char *aName)
{
  return PropertyImpl::getPropertyAsString(mCtx, aName);
}

double
Container :: getPropertyAsDouble(const char *aName)
{
  return PropertyImpl::getPropertyAsDouble(mCtx, aName);
}

int64_t
Container :: getPropertyAsLong(const char *aName)
{
  return PropertyImpl::getPropertyAsLong(mCtx, aName);
}

Rational*
Container :: getPropertyAsRational(const char *aName)
{
  return PropertyImpl::getPropertyAsRational(mCtx, aName);
}

bool
Container :: getPropertyAsBoolean(const char *aName)
{
  return PropertyImpl::getPropertyAsBoolean(mCtx, aName);
}

int32_t
Container :: getFlags()
{
  int32_t flags = (mCtx ? mCtx->flags : 0);
  // remove custom io if set
  flags &= ~(AVFMT_FLAG_CUSTOM_IO);
  return flags;
}

void
Container :: setFlags(int32_t newFlags)
{
  if (mCtx) {
    mCtx->flags = newFlags;
    // force custom io
    mCtx->flags |= AVFMT_FLAG_CUSTOM_IO;
  }
}

bool
Container::getFlag(Container::Flag flag)
{
  bool result = false;
  if (mCtx)
    result = mCtx->flags& flag;
  return result;
}

void
Container::setFlag(Container::Flag flag, bool value)
{
  if (mCtx)
  {
    if (value)
    {
      mCtx->flags |= flag;
    }
    else
    {
      mCtx->flags &= (~flag);
    }
  }

}

const char*
Container :: getURL()
{
  return mCtx && *mCtx->filename ? mCtx->filename : 0;
}


} /* namespace video */
} /* namespace humble */
} /* namespace io */
