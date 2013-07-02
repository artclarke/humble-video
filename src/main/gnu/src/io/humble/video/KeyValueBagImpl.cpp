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
#include "KeyValueBagImpl.h"

namespace io { namespace humble { namespace video
{

KeyValueBagImpl :: KeyValueBagImpl()
{
  mLocalBag = 0;
  mBag = &mLocalBag;
}

KeyValueBagImpl :: ~KeyValueBagImpl()
{
  if (mBag && *mBag)
    av_dict_free(mBag);
}

int32_t
KeyValueBagImpl :: getNumKeys()
{
  if (!mBag || !*mBag)
    return 0;
  
  AVDictionaryEntry* tag=0;
  int32_t retval=0;
  do
  {
    tag = av_dict_get(*mBag, "", tag, AV_DICT_IGNORE_SUFFIX);
    if (tag)
      retval++;
  } while(tag);
  return retval;
}

const char*
KeyValueBagImpl :: getKey(int32_t index)
{
  if (!mBag || !*mBag || index < 0)
    return 0;

  AVDictionaryEntry* tag=0;
  int32_t position=-1;
  do
  {
    tag = av_dict_get(*mBag, "", tag, AV_DICT_IGNORE_SUFFIX);
    if (tag) {
      position++;
      if (position == index)
        return tag->key;
    }
  } while(tag);
  return 0;
}
const char*
KeyValueBagImpl :: getValue(const char *key, Flags flag)
{
   if (!mBag || !*mBag || !key || !*key)
     return 0;
   AVDictionaryEntry* tag = av_dict_get(*mBag, key, 0, (int)flag);
   if (tag)
     return tag->value;
   else
     return 0;
}

int32_t
KeyValueBagImpl :: setValue(const char* key, const char* value)
{
  return setValue(key, value, KVB_NONE);
}

int32_t
KeyValueBagImpl :: setValue(const char* key, const char* value, Flags flag)
{
  if (!key || !*key || !mBag)
    return -1;
  return (int32_t)av_dict_set(mBag, key, value, (int)flag);
}

KeyValueBagImpl*
KeyValueBagImpl :: make(AVDictionary** metaToUse)
{
  if (!metaToUse)
    return 0;
  
  KeyValueBagImpl* retval = make();
  
  if (retval)
    retval->mBag = metaToUse;

  return retval;
}

KeyValueBagImpl*
KeyValueBagImpl :: make(AVDictionary* metaDataToCopy)
{
  KeyValueBagImpl* retval = make();
  if (retval && metaDataToCopy)
  {
    AVDictionaryEntry* tag = 0;
    do {
      tag = av_dict_get(metaDataToCopy, "", tag,
          AV_DICT_IGNORE_SUFFIX);
      if (tag)
        if (av_dict_set(retval->mBag, tag->key, tag->value,0) < 0)
        {
          VS_REF_RELEASE(retval);
          break;
        }
    } while(tag);
  }
  return retval;
}

int32_t
KeyValueBagImpl :: copy(AVDictionary *data)
{
  if (!data)
    return -1;

  if (mBag) {
    if (data == *mBag)
      // copy the current data; just return
      return 0;
    av_dict_free(mBag);
    *mBag = 0;
  }
  av_dict_copy(mBag, data, 0);
  return 0;
}

int32_t
KeyValueBagImpl :: copy(KeyValueBag* dataToCopy)
{
  KeyValueBagImpl* data = dynamic_cast<KeyValueBagImpl*>(dataToCopy);
  if (!data)
    return -1;
  
  return copy(data->getDictionary());
}
}}}
