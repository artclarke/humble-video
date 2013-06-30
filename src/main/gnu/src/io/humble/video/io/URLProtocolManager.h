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

#ifndef URLPROTOCOLMANAGER_H_
#define URLPROTOCOLMANAGER_H_

#include <io/humble/video/io/URLProtocolHandler.h>

namespace io { namespace humble { namespace video { namespace io
  {
  /**
   * A class for managing custom io protocols.
   */
  class VS_API_HUMBLE_VIDEO_IO URLProtocolManager
  {
  public:
    /**
     * Registers the protocol.
     *
     * @param mgr Protocol to register
     * @return Return NULL on error, else mgr.
     */
    static URLProtocolManager* registerProtocol(URLProtocolManager* mgr);

    /**
     * Unregisters the given protocol.  Caller is responsible for deleting the returned value.
     * @param mgr Protocol to unregister
     * @return The manager value after it's been removed from the list, or null
     *   if mgr is not in the list.
     */
    static URLProtocolManager* unregisterProtocol(URLProtocolManager* mgr);

    /**
     * Returns the next registered protocol after the previous one.
     *
     * @param previous The previous protocol returned by {@link #getNextProtocol} or null to
     *   get the first protocol
     *
     * @return The next protocol
     */
    static URLProtocolManager* getNextProtocol(URLProtocolManager* previous);

    /**
     * Convenience method to unregister all protocols.
     * @return Total number of protocols unregistered and deleted.
     */
    static int unregisterAllProtocols();

    /**
     * Find a protocol manager for the requested protocol.  If multiple protocols are
     * registered with the same name, the earliest registered one is used.
     *
     * @param protocol The protocol to use
     * @param url If protocol is null or "", then the URL string will be parsed to find the protocol specifier
     * @param flags Reservered for future use.
     * @param previous If non null, the search will start at the next protocol AFTER this.  Otherwise
     *   we will search from the beginning.
     *
     * @return The {@link URLProtocolManager} that can be used for this protocol, or NULL if none is found.
     */
    static URLProtocolManager *findProtocol(const char* protocol,
        const char* url,
        int flags,
        URLProtocolManager *previous);
    static URLProtocolHandler* findHandler(const char* url,
        int flags,
        URLProtocolManager *previous);

    /**
     * Returns a URLProtocol handler for the given url and flags.
     *
     * @return a {@link URLProtocolHandler} or NULL if none can be created.
     */
    virtual URLProtocolHandler* getHandler(const char* url, int flags)=0;

    /**
     * Returns the protocol name.
     * @return The protocol name.  The string will surive as long as this object.
     */
    virtual const char* getProtocolName() { return mProtoName; }

    /**
     * Parses the protocol portion from a URL and returns it.
     *
     * @dest This is the buffer we will copy the protocol into
     * @dest-len The size of dest in bytes (must include space for a null terminator)
     * @url The URL to parse
     *
     * @return The protocol string (a pointer to dest) or NULL if we could not determine
     *   the protocol.
     */
    static const char * parseProtocol(char * dest, int32_t dest_len, const char * url);
  protected:
    URLProtocolManager(const char *aProtocolName);
    virtual ~URLProtocolManager();

  private:
    // Convenience function used by following url_* functions
    static URLProtocolHandler* getHandler(void *h);
    // Wrapper functions that are used by FFMPEG
    static int url_open(void* h, const char* url, int flags);
    static int url_close(void *h);
    static int url_read(void *h, unsigned char* buf, int size);
    static int url_write(void *h, const unsigned char* buf, int size);
    static int64_t url_seek(void *h, int64_t position, int whence);
    URLProtocolHandler::SeekableFlags url_seekflags(void *h, const char* url, int flags);
    char* mProtoName;
    URLProtocolManager *mNext;
    static URLProtocolManager* mFirstProtocol;
  };
  }}}}
#endif /*URLPROTOCOLMANAGER_H_*/
