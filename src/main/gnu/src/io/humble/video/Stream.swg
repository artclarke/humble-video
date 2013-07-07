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

%typemap (javacode) io::humble::video::Stream,io::humble::video::Stream*,io::humble::video::Stream& %{

  /**
   * info about this stream
   * @return information about this stream
   */
   
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("index:"+getIndex()+";");
    result.append("id:"+getId()+";");
    result.append("streamcoder:"+getStreamCoder()+";");
    result.append("framerate:"+getFrameRate()+";");
    result.append("timebase:"+getTimeBase()+";");
    result.append("direction:"+getDirection()+";");
    result.append("]");
    return result.toString();
  }

  /**
   * Get an ordered sequence of index entries in this {@link Stream}.
   * 
   * @return A list of entries.  Will always return a non-null
   *   list, but if there are no entries the list size will be zero.
   */
  public java.util.List<IndexEntry> getIndexEntries()
  {
    final int numEntries = getNumIndexEntries();
    java.util.List<IndexEntry> retval = new java.util.ArrayList<IndexEntry>(Math.max(numEntries, 10));
    for(int i = 0; i < numEntries; i++) {
      final IndexEntry entry = getIndexEntry(i);
      if (entry != null) {
       retval.add(entry); 
      }
    }
    
    return retval;
  }

%}

%include <io/humble/video/Stream.h>
