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

%typemap (javacode) io::humble::video::VideoPicture,io::humble::video::VideoPicture*,io::humble::video::VideoPicture& %{

  /**
   * info about this packet
   * @return information about this packet
   */
   
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("pixel type:"+getPixelType()+";");
    result.append("width:"+getWidth()+";");
    result.append("height:"+getHeight()+";");
    result.append("time stamp:"+getTimeStamp()+";");
    result.append("complete:"+isComplete()+";");
    result.append("size:"+getSize()+";");
    result.append("key:"+isKey()+";");
    IRational timeBase = IRational.make(1,(int)Global.DEFAULT_PTS_PER_SECOND);
    result.append("time base:"+timeBase+";");
    if (timeBase != null) timeBase.delete();
    result.append("]");
    return result.toString();
  }

%}

%include <io/humble/video/VideoPicture.h>
