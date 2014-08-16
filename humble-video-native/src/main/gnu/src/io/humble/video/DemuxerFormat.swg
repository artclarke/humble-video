/*
 * Copyright (c) 2014-Forward, Andrew "Art" Clarke
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
 *  Created on: Jun 29, 2013
 *      Author: aclarke
 */

%javamethodmodifiers getNumSupportedCodecs "protected";
%javamethodmodifiers getSupportedCodecId "protected";
%javamethodmodifiers getSupportedCodecTag "protected";
%javamethodmodifiers getNumFormats "protected";
%javamethodmodifiers getFormat "protected"

%typemap (javacode) io::humble::video::DemuxerFormat,io::humble::video::DemuxerFormat*,io::humble::video::DemuxerFormat& %{
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("name:" + getName() + ";");
    result.append("description:"+getLongName() + ";");
    result.append("]");
    return result.toString();
  }

  /**
   * Return a collection of all input formats installed on this system.
   * @return the list.
   */
  public static java.util.Collection<DemuxerFormat>
  getFormats()
  {
    java.util.Collection<DemuxerFormat> retval =
      new java.util.HashSet<DemuxerFormat>();
    int count = getNumFormats();
    for(int i = 0; i< count;++i)
    {
      DemuxerFormat fmt = getFormat(i);
      if (fmt != null)
        retval.add(fmt);
    }
    return retval;
  }

%}

%include <io/humble/video/DemuxerFormat.h>
