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
package io.humble.video.io;

import io.humble.video.io.URLProtocolManager;

/**
 * For Internal Use Only.
 * 
 * A signature interface for objects that want to register protocols for
 * Humble Video to use.
 * 
 * @author aclarke
 *
 */
public interface IURLProtocolRegistrar
{
  /**
   * Tells Humble Video that any requests for a given protocol should be
   * redirected to the given manager.
   * <p>
   * NOTE: Protocol can only be alpha-characters; no numbers or punctuation.
   * </p>
   * @param protocol The protocol (alpha-only) without ":" 
   * @param manager The manager object you want to use.
   */
  void registerProtocol(String protocol, URLProtocolManager manager);
}
