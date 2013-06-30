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
#ifndef METADATA_H_
#define METADATA_H_

#include <io/humble/video/MetaData.h>
namespace io { namespace humble { namespace video
{

class MetaDataImpl : public MetaData
{
  VS_JNIUTILS_REFCOUNTED_OBJECT(MetaDataImpl);
public:
  virtual int32_t getNumKeys();
  virtual const char* getKey(int32_t position);

  virtual const char *getValue(const char* key, Flags flag);
  virtual int32_t setValue(const char* key, const char* value);
  virtual int32_t setValue(const char* key, const char* value, Flags flag);
  
  /**
   * Create a MetaData object using metaDataToReference.
   * Once this is done, this MetaData object is responsible
   * for calling av_dict_free(*metaDataToReference),
   * so take care.
   * 
   */
  static MetaDataImpl* make(AVDictionary ** metaDataToReference);
  
  /**
   * Copies all meta data currently in metaDataToCopy
   * and returns a new object.
   */
  static MetaDataImpl* make(AVDictionary* metaDataToCopy);
  
  /**
   * Destroys the current data, and copies all data
   * from copy.
   */
  int32_t copy(MetaData* copy);

  /**
   * Destroys the current data and copies all data from copy.
   */
  int32_t copy(AVDictionary* copy);

  /**
   * Returns the AVDictionary for passing to FFmpeg calls
   *
   * @return The underlying AVDictionary
   */
  AVDictionary* getDictionary() { return (mMetaData ? *mMetaData : 0); }

protected:
  MetaDataImpl();
  virtual
  ~MetaDataImpl();
private:
  AVDictionary** mMetaData;
  AVDictionary* mLocalMeta;
};

}

}

}

#endif /* METADATA_H_ */
