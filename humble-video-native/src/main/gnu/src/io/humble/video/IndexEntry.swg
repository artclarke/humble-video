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

 %typemap (javacode) io::humble::video::IndexEntry,io::humble::video::IndexEntry*,io::humble::video::IndexEntry& %{
   /**
   * info about this entry.
   * @return information about this entry.
   */
   
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("position:"+getPosition()+";");
    result.append("timestamp:"+getTimeStamp()+";");
    result.append("flags:"+getFlags()+";");
    result.append("size:"+getSize()+";");
    result.append("min-distance:"+getMinDistance()+";");
    result.append("]");
    return result.toString();
  }

 
%}
%include <io/humble/video/IndexEntry.h>
 
