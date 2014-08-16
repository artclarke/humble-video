/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
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

%typemap (javacode) io::humble::video::Coder,io::humble::video::Coder*,io::humble::video::Coder& %{
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    MediaDescriptor.Type type = getCodecType();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("codec="+getCodec()+";");
    result.append("time base="+getTimeBase()+";");
    switch(type)
    {
      case MEDIA_VIDEO:
        result.append("width="+getWidth()+";");
        result.append("height="+getHeight()+";");
        result.append("pixel format="+getPixelFormat()+";");
        break;
      case MEDIA_AUDIO:
        result.append("sample rate="+getSampleRate()+";");
        result.append("channels="+getChannels()+";");
        result.append("sample format="+getSampleFormat()+";");
        break;
      default:
        break;
    }
    result.append("frame size="+getFrameSize()+";");
    result.append("frame count="+getFrameCount()+";");

    result.append("]");
    return result.toString();
  }
%}

%include <io/humble/video/Coder.h>
