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
#ifndef __FERRY_H__
#define __FERRY_H__

#include <jni.h>
#include <io/humble/Humble.h>

namespace io
{
namespace humble
{
/**
 * This library contains routines used by Humble libraries for
 * "ferry"ing Java objects to and from native code.
 *
 * Of particular importance is the RefCounted interface.
 */
namespace ferry
{

}
}
}
#ifndef VS_API_FERRY
#define VS_API_FERRY VS_API_EXPORT
#endif // ! VS_API_FERRY
#endif // ! __FERRY_H__
