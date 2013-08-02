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

%typemap (javacode) io::humble::video::Source,io::humble::video::Source*,io::humble::video::Source& %{
  @Override
  public String toString() {
    final StringBuilder b = new StringBuilder(super.toString());
    b.append("[")
    .append("url="+getURL()+";")
    .append("format:"+getSourceFormat()+";")
    .append("]");
    return b.toString();
  }
%}

/**
 * Tell users which methods can block (and hence, can be interrupted).
 */
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::open);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::read);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::seek);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::play);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::pause);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::close);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::getNumStreams);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::getStream);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::getSourceStream);
HUMBLE_JAVA_EXCEPTION("java.lang.InterruptedException, java.io.IOException", io::humble::video::Source::queryStreamMetaData);

%include <io/humble/video/Source.h>
