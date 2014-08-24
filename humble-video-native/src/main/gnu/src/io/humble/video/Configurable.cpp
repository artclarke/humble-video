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
/*
 * Configurable.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: aclarke
 */

#include "Configurable.h"
#include "PropertyImpl.h"

namespace io {
namespace humble {
namespace video {

Configurable::Configurable() {
}

Configurable::~Configurable() {
}


int32_t
Configurable::getNumProperties()
{
  return PropertyImpl::getNumProperties(getCtx());
}

Property*
Configurable::getPropertyMetaData(int32_t propertyNo)
{
  return PropertyImpl::getPropertyMetaData(getCtx(), propertyNo);
}

Property*
Configurable::getPropertyMetaData(const char *name)
{
  return PropertyImpl::getPropertyMetaData(getCtx(), name);
}

void
Configurable::setProperty(KeyValueBag* valuesToSet, KeyValueBag* valuesNotFound)
{
  PropertyImpl::setProperty(getCtx(), valuesToSet, valuesNotFound);
}

void
Configurable::setProperty(const char* aName, const char *aValue)
{
  PropertyImpl::setProperty(getCtx(), aName, aValue);
}

void
Configurable::setProperty(const char* aName, double aValue)
{
  PropertyImpl::setProperty(getCtx(), aName, aValue);
}

void
Configurable::setProperty(const char* aName, int64_t aValue)
{
  PropertyImpl::setProperty(getCtx(), aName, aValue);
}

void
Configurable::setProperty(const char* aName, bool aValue)
{
  PropertyImpl::setProperty(getCtx(), aName, aValue);
}

void
Configurable::setProperty(const char* aName, Rational *aValue)
{
  PropertyImpl::setProperty(getCtx(), aName, aValue);
}

char*
Configurable::getPropertyAsString(const char *aName)
{
  return PropertyImpl::getPropertyAsString(getCtx(), aName);
}

double
Configurable::getPropertyAsDouble(const char *aName)
{
  return PropertyImpl::getPropertyAsDouble(getCtx(), aName);
}

int64_t
Configurable::getPropertyAsLong(const char *aName)
{
  return PropertyImpl::getPropertyAsLong(getCtx(), aName);
}

int32_t
Configurable::getPropertyAsInt(const char *aName)
{
  return PropertyImpl::getPropertyAsInt(getCtx(), aName);
}
Rational*
Configurable::getPropertyAsRational(const char *aName)
{
  return PropertyImpl::getPropertyAsRational(getCtx(), aName);
}

bool
Configurable::getPropertyAsBoolean(const char *aName)
{
  return PropertyImpl::getPropertyAsBoolean(getCtx(), aName);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
