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
 */

%typemap (javacode) io::humble::video::MediaPictureResampler,io::humble::video::MediaPictureResampler*,io::humble::video::MediaPictureResampler& %{
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("input width:"+getInputWidth()+";");
    result.append("input height:"+getInputHeight()+";");
    result.append("input format:"+getInputFormat()+";");
    result.append("output width:"+getOutputWidth()+";");
    result.append("output height:"+getOutputHeight()+";");
    result.append("output format:"+getOutputFormat()+";");
    result.append("]");
    return result.toString();
  }

%}

%include <io/humble/video/MediaPictureResampler.h>
