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

/**
 * An API for extending Humble Video to read and write from arbitrary data sources.
 * <p>
 * Humble Video needs to process raw media file data in order to decode and encode
 * those files. What's worse, is the only input Humble Video can give to FFMPEG is a
 * URL.
 * </p>
 * <p>
 * By default, Humble Video can read any "file:" URL. In fact if you don't specify a
 * protocol, we will assume "file:". But what happens if you want to read data
 * from an arbitrary source (like FMS, Red5, Wowza, etc)?
 * </p>
 * <p>
 * That's where this package comes in. Using the IO package you can implement a
 * {@link io.humble.video.io.IURLProtocolHandler} that can respond to FFMPEG
 * requests to read or write buffers. From there any type of integration is
 * possible.
 * </p>
 * <p>
 * To begin with this class, see
 * {@link io.humble.video.io.URLProtocolManager} and take a look at the
 * example source code for {@link io.humble.video.io.FileProtocolHandler} and
 * {@link io.humble.video.io.FileProtocolHandlerFactory} classes.
 * </p>
 */
package io.humble.video.io;

