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

%typemap (javacode) io::humble::video::MediaPicture,io::humble::video::MediaPicture*,io::humble::video::MediaPicture& %{
  /**
   * info about this picture
   * @return information about this picture
   */
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("width:"+getWidth()+";");
    result.append("height:"+getHeight()+";");
    result.append("format:"+getFormat()+";");
    result.append("time stamp:"+getTimeStamp()+";");
    result.append("complete:"+isComplete()+";");
    result.append("key:"+isKey()+";");
    Rational timeBase = Global.getDefaultTimeBase();
    result.append("time base:"+timeBase+";");
    for(int i = 0; i < getNumDataPlanes(); i++)
      result.append("plane["+i+"] size: " + getDataPlaneSize(i) +";"); 
    if (timeBase != null) timeBase.delete();
    result.append("]");
    return result.toString();
  }

%}

%include <io/humble/video/MediaPicture.h>
