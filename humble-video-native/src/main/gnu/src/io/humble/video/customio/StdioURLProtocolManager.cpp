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

#include <exception>
#include <stdexcept>

#include <string.h>

#include <io/humble/ferry/JNIHelper.h>

#include <io/humble/video/customio/StdioURLProtocolManager.h>

using namespace std;
using namespace io::humble::ferry;

namespace io { namespace humble { namespace video { namespace customio
{
StdioURLProtocolManager*
StdioURLProtocolManager :: registerProtocol(const char *aProtocolName)
{
  StdioURLProtocolManager* mgr = new StdioURLProtocolManager(aProtocolName);
  return dynamic_cast<StdioURLProtocolManager*>(URLProtocolManager::registerProtocol(mgr));
}


StdioURLProtocolManager :: StdioURLProtocolManager(
    const char * aProtocolName) : URLProtocolManager(aProtocolName)
{
}

StdioURLProtocolManager :: ~StdioURLProtocolManager()
{
}

StdioURLProtocolHandler *
StdioURLProtocolManager :: getHandler(const char *, int)
{
  return new StdioURLProtocolHandler(this);
}
}}}}
