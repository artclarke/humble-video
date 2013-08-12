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

#ifndef IKEYVALUEBAG_H_
#define IKEYVALUEBAG_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefCounted.h>

namespace io { namespace humble { namespace video
{

/**
 * KeyValueBag is a bag of key/value pairs that can be embedded
 * in some Container or some Stream
 * in an Container, and are then written to
 * or read from a media file.  Keys must be unique, and
 * any attempt to set a key value replaces any previous values.
 * </p>
 * <p>
 * An example is the "title" meta-data item in an MP3 file.
 * </p>
 * <p>
 * Support for KeyValueBag differs depending upon the 
 * Container format you're using and the implementation
 * in <a href="http://www.ffmpeg.org/">FFmpeg</a>.  For example,
 * MP3 meta-data reading and writing is supported, but
 * (as of the writing of this comment) FLV meta-data writing
 * is not supported.
 * </p>
 */
class VS_API_HUMBLEVIDEO KeyValueBag : public io::humble::ferry::RefCounted
{
public:
  /**
   * Different types of flags that can be passed to KeyValueBag#getValue
   */
  typedef enum {
    /**
     * For #getValue(String) case-insensitive match of key.
     */
    KVB_NONE=0,
    /**
     * For #getValue(String) case-sensitive match of key.
     */
    KVB_MATCH_CASE=1,
    /**
     * For #setValue(String,String,Flags) do not overwrite existing value -- append another key/value pair.
     */
    KVB_DONT_OVERWRITE=16,
    /**
     * For #setValue(String,String,Flags) append to the existing value in a key (string append).
     */
    KVB_APPEND=32,
  } Flags;

  /**
   * Get the total number of keys currently in this
   * KeyValueBag object.
   * 
   * @return the number of keys.
   */
  virtual int32_t getNumKeys()=0;
  
  /**
   * Get the key at the given position, or null if no such
   * key at that position.
   * 
   * <p>
   * Note: positions of keys may change between
   * calls to #setValue(String, String) and 
   * should be requiried.
   * </p>
   * 
   * @param position The position.  Must be >=0 and < 
   * #getNumKeys().
   * 
   * @return the key, or null if not found.
   */
  virtual const char* getKey(int32_t position)=0;

  /**
   * Get the value for the given key.
   * 
   * @param key The key
   * @param flag A flag for how to search
   * 
   * @return The value, or null if none.
   */
  virtual const char *getValue(const char* key, Flags flag)=0;
  
  /**
   * Sets the value for the given key to value.  This overrides
   * any prior setting for key, or adds key to the meta-data
   * if appropriate.
   * 
   * @param key The key to set.
   * @param value The value to set.
   */
  virtual int32_t setValue(const char* key, const char* value)=0;
  
  /**
   * Create a new KeyValueBag bag of properties with
   * no values set.
   */
  static KeyValueBag* make();

  /**
   * Sets the value for the given key to value.  This overrides
   * any prior setting for key, or adds key to the meta-data
   * if appropriate.
   *
   * @param key The key to set.
   * @param value The value to set.
   * @param flag A flag on how this should be set.
   *
   * @since 5.0
   */
  virtual int32_t setValue(const char* key, const char* value, Flags flag)=0;
 
protected:
  KeyValueBag();
  virtual
  ~KeyValueBag();
};

}}}

#endif /* IKEYVALUEBAG_H_ */
