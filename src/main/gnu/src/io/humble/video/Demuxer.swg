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

%typemap (javacode) io::humble::video::Demuxer,io::humble::video::Demuxer*,io::humble::video::Demuxer& %{
  @Override
  public String toString() {
    final StringBuilder b = new StringBuilder(super.toString());
    b.append("[")
    .append("url="+getURL()+";")
    .append("format:"+getFormat()+";")
    .append("]");
    return b.toString();
  }
%}

/**
 * Tell users which methods can block (and hence, can be interrupted).
 */
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::open);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::read);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::seek);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::play);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::pause);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::close);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::getNumStreams);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::getStream);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Demuxer::queryStreamMetaData);

%include <io/humble/video/Demuxer.h>
