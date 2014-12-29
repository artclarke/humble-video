/*******************************************************************************
 * Copyright (c) 2014, Art Clarke.  All rights reserved.
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

%typemap (javacode) io::humble::video::BitStreamFilterType,io::humble::video::BitStreamFilterType*,io::humble::video::BitStreamFilterType& %{
  /**
   * info about this filter object
   * @return information about this object
   */
   
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("name:"+getName()+";");
    result.append("]");
    return result.toString();
  }
  
    /**
   * Gets a collection of ALL codecs installed on this
   * system. 
   * 
   * @return A collection of all installed codecs.
   */
  public static java.util.Collection<BitStreamFilterType>
  getInstalledBitStreamFilterTypes()
  {
    java.util.Collection<BitStreamFilterType> retval = new java.util.HashSet<BitStreamFilterType>();
    int count = getNumBitStreamFilterTypes();
    for(int i=0;i<count;i++)
    {
      BitStreamFilterType t = getBitStreamFilterType(i);
      if (t != null)
        retval.add(t);
    }
    return retval;
  }
%}

%typemap (javacode) io::humble::video::BitStreamFilter,io::humble::video::BitStreamFilter*,io::humble::video::BitStreamFilter& %{
  /**
   * info about this filter object
   * @return information about this object
   */
   
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("name:"+getName()+";");
    result.append("]");
    return result.toString();
  }
  
%}

%include <io/humble/video/BitStreamFilter.h>
