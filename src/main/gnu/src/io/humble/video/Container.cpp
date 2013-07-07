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

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::video::customio;

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
}

int
Container::url_read(void*h, unsigned char* buf, int size)
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
Container::url_write(void*h, unsigned char* buf, int size)
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
Container::url_seek(void*h, int64_t position, int whence)
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

} /* namespace video */
} /* namespace humble */
} /* namespace io */
