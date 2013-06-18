/*******************************************************************************
 * Copyright (c) 2008, 2010 Xuggle Inc.  All rights reserved.
 *  
 * This file is part of Xuggle-Xuggler-Main.
 *
 * Xuggle-Xuggler-Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xuggle-Xuggler-Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Xuggle-Xuggler-Main.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef __FERRY_H__
#define __FERRY_H__

#include <jni.h>
#include <com/xuggle/Xuggle.h>

namespace com
{
namespace xuggle
{
/**
 * This library contains routines used by Xuggle libraries for
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
