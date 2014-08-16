/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
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
#ifndef KEYVALUEBAGIMPL_H_
#define KEYVALUEBAGIMPL_H_

#include <io/humble/video/KeyValueBag.h>
namespace io { namespace humble { namespace video
{

class KeyValueBagImpl : public KeyValueBag
{
  VS_JNIUTILS_REFCOUNTED_OBJECT(KeyValueBagImpl);
public:
  virtual int32_t getNumKeys();
  virtual const char* getKey(int32_t position);

  virtual const char *getValue(const char* key, Flags flag);
  virtual int32_t setValue(const char* key, const char* value);
  virtual int32_t setValue(const char* key, const char* value, Flags flag);
  
  /**
   * Create a KeyValueBag object using keyValueBagToReference.
   * Once this is done, this KeyValueBag object is responsible
   * for calling av_dict_free(*keyValueBagToReference),
   * so take care.
   * 
   */
  static KeyValueBagImpl* make(AVDictionary ** keyValueBagToReference);
  
  /**
   * Copies all meta data currently in keyValueBagToCopy
   * and returns a new object.
   */
  static KeyValueBagImpl* make(AVDictionary* keyValueBagToCopy);
  
  /**
   * Destroys the current data, and copies all data
   * from copy.
   */
  int32_t copy(KeyValueBag* copy);

  /**
   * Destroys the current data and copies all data from copy.
   */
  int32_t copy(AVDictionary* copy);

  /**
   * Returns the AVDictionary for passing to FFmpeg calls
   *
   * @return The underlying AVDictionary
   */
  AVDictionary* getDictionary() { return (mBag ? *mBag : 0); }

protected:
  KeyValueBagImpl();
  virtual
  ~KeyValueBagImpl();
private:
  AVDictionary** mBag;
  AVDictionary* mLocalBag;
};

}

}

}

#endif /* KEYVALUEBAGIMPL_H_ */
