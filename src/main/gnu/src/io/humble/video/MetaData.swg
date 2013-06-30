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
 */
%typemap (javacode) io::humble::video::MetaData,io::humble::video::MetaData*,io::humble::video::MetaData& %{
  /**
   * Return the meta-data, with all values expanded.
   */
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    result.append(this.getClass().getName()).append("@").append(hashCode());
    result.append("[");
    java.util.Collection<String> keys = getKeys();
    for(String key: keys)
    {
      String value = getValue(key, MetaData.Flags.METADATA_NONE);
      result.append(key).append("=").append(value).append(";");
    }
    result.append("]");
    return result.toString();
  }
  
  /**
   * Get the set of keys currently in this {@link IMediaData} object
   * as an unordered collection.
   * @return the keys.
   */
  public java.util.Collection<String> getKeys()
  {
    int numKeys = getNumKeys();
    
    java.util.List<String> retval = new java.util.ArrayList<String>(numKeys);
    for(int i = 0; i < getNumKeys(); i++)
    {
      String key = getKey(i);
      if (key != null && key.length() > 0)
        retval.add(key);
    }
    return retval;
  }
 
  /**
   * Return the value for this key, comparing case insensitively.
   * @param key the key
   * @return the value.
   */
  public String getValue(String key)
  {
    return getValue(key, MetaData.Flags.METADATA_NONE);
  }
%}
%javamethodmodifiers io::humble::video::MetaData::getKey(int32_t) "private"

%include <io/humble/video/MetaData.h>
