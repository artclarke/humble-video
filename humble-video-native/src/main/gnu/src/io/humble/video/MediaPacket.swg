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

%typemap (javacode) io::humble::video::MediaPacket,io::humble::video::MediaPacket*,io::humble::video::MediaPacket& %{
  @Override
  public String toString() {
    final StringBuilder b = new StringBuilder(super.toString());
    b.append("[")
    .append("pts="+getPts()+";")
    .append("dts="+getDts()+";")
    .append("complete="+isComplete()+";")
    .append("size="+getSize()+";")
    .append("maxSize="+getMaxSize()+";")
    .append("streamIndex="+getStreamIndex()+";")
    .append("key="+isKey()+";")
    .append("flags="+getFlags()+";")
    .append("duration="+getDuration()+";")
    .append("position="+getPosition()+";")
    .append("convergenceDuration="+getConvergenceDuration()+";")
    .append("]");
    return b.toString();
  }

%}

%include <io/humble/video/MediaPacket.h>
