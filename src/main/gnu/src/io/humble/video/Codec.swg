/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
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
 * Codec.swg
 *
 *  Created on: Jun 29, 2013
 *      Author: aclarke
 */

%typemap (javacode) io::humble::video::CodecDescriptor,io::humble::video::CodecDescriptor*,io::humble::video::CodecDescriptor& %{

  /**
    * info about this {@link CodecDescriptor} object
    * @return information about this object
    */
    
   @Override
   public String toString()
   {
     StringBuilder result = new StringBuilder();
     
     result.append(this.getClass().getName()+"@"+hashCode()+"[");
     result.append("name:"+getName()+";");
     result.append("longname:"+getName()+";");
     result.append("properties:"+getProperties()+";");
     result.append("]");
     return result.toString();
   }
%}

%typemap (javacode) io::humble::video::Codec,io::humble::video::Codec*,io::humble::video::Codec& %{
%}

%include <io/humble/video/Codec.h>