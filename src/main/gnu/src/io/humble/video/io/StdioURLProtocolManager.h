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

#ifndef STDIOURLPROTOCOLMANAGER_H_
#define STDIOURLPROTOCOLMANAGER_H_

#include <io/humble/video/io/URLProtocolManager.h>
#include <io/humble/video/io/StdioURLProtocolHandler.h>

namespace io { namespace humble { namespace video { namespace io
{
  /**
   * A class for managing custom io protocols.
   */
  class VS_API_HUMBLE_VIDEO_IO StdioURLProtocolManager : public URLProtocolManager
  {
  public:
    /**
     * Returns a URLProtocol handler for the given url and flags
     *
     * @return a {@link URLProtocolHandler} or NULL if none can be created.
     */
    StdioURLProtocolHandler* getHandler(const char* url, int flags);

    /**
     * Convenience method that creats a StdioURLProtocolManager and registers with the
     * URLProtocolManager global methods.
     */
    static StdioURLProtocolManager* registerProtocol(const char *aProtocolName);

  protected:
    StdioURLProtocolManager(const char *aProtocolName);
    virtual ~StdioURLProtocolManager();

  private:
  };
}}}}
#endif /*STDIOURLPROTOCOLMANAGER_H_*/
