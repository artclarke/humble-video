/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include <io/humble/video/Property.h>
#include <io/humble/video/MetaData.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/Property.h>
extern "C" {
#include "FfmpegIncludes.h"
#include <libavutil/log.h>
}

#include <stdexcept>
#include <cstring>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video {

  Property :: Property()
  {
    mOption=0;
    mOptionStart=0;
  }

  Property :: ~Property()
  {
  }

  Property*
  Property :: make(const AVOption *start, const AVOption *option)
  {
    Property *retval = 0;
    try
    {
      if (!start)
        throw std::bad_alloc();
      if (!option)
        throw std::bad_alloc();
      retval = Property::make();
      if (retval)
      {
        retval ->mOptionStart = start;
        retval->mOption = option;
      }
    }
    catch (std::bad_alloc & e)
    {
      VS_REF_RELEASE(retval);
      throw e;
    }
    return retval;
  }
  
  const char *
  Property :: getName()
  {
    return mOption ? mOption->name : 0;
  }
  
  const char *
  Property :: getHelp()
  {
    return mOption ? mOption->help : 0;
  }
  
  const char *
  Property :: getUnit()
  {
    return mOption ? mOption->unit : 0;
  }
  
  IProperty::Type
  Property :: getType()
  {
    return mOption ? (IProperty::Type) mOption->type : IProperty::PROPERTY_UNKNOWN;
  }

  int64_t
  Property :: getDefault()
  {
    return (int64_t)getDefaultAsDouble();
  }
    
  double
  Property :: getDefaultAsDouble()
  {
    return mOption ? mOption->default_val.dbl: 0.0;
  }
  
  int32_t
  Property :: getFlags()
  {
    return mOption ? mOption->flags : 0;
  }
  
  int32_t
  Property :: getNumProperties(void *aContext)
  {
    int32_t retval=0;
    const AVOption* last = 0;
    if (aContext)
    {
      do {
        last = av_opt_next(aContext, last);
        if (last)
        {
          if (last->type != AV_OPT_TYPE_CONST)
            ++retval;
        }
      } while (last);
    }
    return retval;
  }
  
  IProperty*
  Property :: getPropertyMetaData(void *aContext, int32_t aPropertyNo)
  {
    IProperty *retval = 0;
    const AVOption* last = 0;

    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");

      int32_t optionNo=-1;
      do {
        last = av_opt_next(aContext, last);
        if (last)
        {
          if (last->type != AV_OPT_TYPE_CONST)
          {
            ++optionNo;
            // now see if this option position matches the property asked for
            if (optionNo == aPropertyNo)
            {
              retval = Property::make(av_opt_next(aContext, 0), last);
              break;
            }
          }
        }
      } while (last);
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      VS_REF_RELEASE(retval);
    }    
    return retval;
  }

  IProperty*
  Property :: getPropertyMetaData(void *aContext, const char *aName)
  {
    IProperty *retval = 0;
    const AVOption* last = 0;

    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName || !*aName)
        throw std::runtime_error("no property name passed in");
      
      last = av_opt_find(aContext, aName, 0, 0, 0);
      if (last)
      {
        if (last->type != AV_OPT_TYPE_CONST)
        {
          retval = Property::make(av_opt_next(aContext, 0), last);
        }
      }
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      VS_REF_RELEASE(retval);
    }    
    return retval;
  }
  
  int32_t
  Property :: setProperty(void *aContext, const char* aName, const char *aValue)
  {
    int32_t retval = -1;

    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");

      void * target=0;
      const AVOption *o = av_opt_find2(aContext, aName, 0, PROPERTY_SEARCH_CHILDREN, 1, &target);
      if (o) {
        AVClass *c = *(AVClass**)target;
        (void) c;
        VS_LOG_TRACE("Found option \"%s\" with help: %s; in unit: %s; object type: %s; instance name: %s",
          o->name,
          o->help,
          o->unit,
          c->class_name,
          c->item_name(aContext));
      }
      VS_LOG_TRACE("Setting %s to %s", aName, aValue);
      retval = av_opt_set(aContext, aName, aValue, PROPERTY_SEARCH_CHILDREN);
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = -1;
    }

    return retval;
  }

  char*
  Property :: getPropertyAsString(void *aContext, const char *aName)
  {
    char* retval = 0;
    char *value = 0;

    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");

      // we don't allow a string value longer than this.  This is
      // actually safe because this buffer is only used for non-string options
      if (av_opt_get(aContext, aName, 0, (uint8_t**)&value) < 0)
          throw std::runtime_error("could not get property");
      
      if (value)
      {
        // let's make a copy of the data
        int32_t valLen = strlen(value);
        if (valLen > 0)
        {
          retval = (char*)malloc((valLen+1)*sizeof(char));
          if (!retval)
            throw std::bad_alloc();
          
          // copy the data
          strncpy(retval, value, valLen+1);
        }
      }
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      if (retval)
        free(retval);
      retval = 0;
    }
    if (value)
      av_free(value);

    // NOTE: Caller must call free() on returned value; we mean it!
    return retval;

  }

  static const char * fakeContextToName(void*)
  {
    return "humbleVideoFake";
  }
  
  int32_t
  Property :: getNumFlagSettings()
  {
    int32_t retval = 0;
    try {
      if (getType() != IProperty::PROPERTY_FLAGS)
        throw std::runtime_error("flag is not of type PROPERTY_FLAGS");
      
      // now, iterate through all options, counting all CONSTS that belong
      // to the same unit as this option
      const char* unit = getUnit();
      if (!unit || !*unit)
        throw std::runtime_error("flag doesn't have a unit setting, so can't tell what constants");
      
      // Create a fake class 
      AVClass fakeClass;
      fakeClass.class_name="humbleVideoFakeClass";
      fakeClass.item_name = fakeContextToName;
      fakeClass.option = mOptionStart;
      AVClass *fakeClassPtr = &fakeClass;
      
      const AVOption* last = 0;
      do
      {
        last = av_opt_next(&fakeClassPtr, last);
        if (last &&
            last->unit &&
            last->type == AV_OPT_TYPE_CONST &&
            strcmp(unit, last->unit)==0)
          ++retval;
      } while(last);
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = -1;
    }
    return retval;
  }

  IProperty *
  Property :: getFlagConstant(int32_t position)
  {
    IProperty *retval = 0;
    try
    {
      if (getType() != IProperty::PROPERTY_FLAGS)
        throw std::runtime_error("flag is not of type PROPERTY_FLAGS");
      
      // now, iterate through all options, counting all CONSTS that belong
      // to the same unit as this option
      const char* unit = getUnit();
      if (!unit || !*unit)
        throw std::runtime_error("flag doesn't have a unit setting, so can't tell what constants");
      
      // Create a fake class 
      AVClass fakeClass;
      fakeClass.class_name="humbleVideoFakeClass";
      fakeClass.item_name = fakeContextToName;
      fakeClass.option = mOptionStart;
      AVClass *fakeClassPtr = &fakeClass;
      
      const AVOption* last = 0;
      int32_t constNo = -1;
      do
      {
        last = av_opt_next(&fakeClassPtr, last);
        if (last &&
            last->unit &&
            last->type == AV_OPT_TYPE_CONST &&
            strcmp(unit, last->unit)==0)
        {
          // count in the same was as getNumFlagSettings,
          // and then return if the position is equal
          ++constNo;
          if (constNo == position)
          {
            retval = Property::make(av_opt_next(&fakeClassPtr, 0), last);
          }
        }
      } while(last);
      
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      VS_REF_RELEASE(retval);
    }
    return retval;
  }

  IProperty *
  Property :: getFlagConstant(const char* aName)
  {
    IProperty *retval = 0;
    try
    {
      if (getType() != IProperty::PROPERTY_FLAGS)
        throw std::runtime_error("flag is not of type PROPERTY_FLAGS");
      
      // now, iterate through all options, counting all CONSTS that belong
      // to the same unit as this option
      const char* unit = getUnit();
      if (!unit || !*unit)
        throw std::runtime_error("flag doesn't have a unit setting, so can't tell what constants");

      AVClass fakeClass;
      fakeClass.class_name="humbleVideoFakeClass";
      fakeClass.item_name = fakeContextToName;
      fakeClass.option = mOptionStart;
      
      const AVOption* last = 0;
      last = av_opt_find(&fakeClass, aName, unit, 0, 0);
      if (last)
      {
        if (last->type == AV_OPT_TYPE_CONST)
        {
          retval = Property::make(av_opt_next(&fakeClass, 0), last);
        }
      }

    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      VS_REF_RELEASE(retval);
    }
    return retval;
  }
  
  double
  Property :: getPropertyAsDouble(void *aContext, const char* aName)
  {
    double retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
      
      if (av_opt_get_double(aContext, aName, 0, &retval) < 0)
        throw std::runtime_error("error getting property as double");

    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = 0;
    }
    return retval;
  }
  
  int64_t
  Property :: getPropertyAsLong(void *aContext, const char* aName)
  {
    int64_t retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
      
      retval = av_get_int(aContext, aName, 0);

    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = 0;
    }
    return retval;
  }
  
  IRational*
  Property :: getPropertyAsRational(void *aContext, const char* aName)
  {
    IRational *retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
      
      AVRational value = av_get_q(aContext, aName, 0);
      retval = IRational::make(value.num, value.den);
    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      VS_REF_RELEASE(retval);
    }
    return retval;
  }
  

  bool
  Property :: getPropertyAsBoolean(void *aContext, const char* aName)
  {
    return (bool) getPropertyAsLong(aContext, aName);
  }
  
  int32_t
  Property :: setProperty(void *aContext, const char* aName, double value)
  {
    int32_t retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
      retval = av_opt_set_double(aContext, aName, value, PROPERTY_SEARCH_CHILDREN);
    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = -1;
    }
    return retval;
  }
  
  int32_t
  Property :: setProperty(void *aContext, const char* aName, int64_t value)
  {
    int32_t retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
   
      retval = av_opt_set_int(aContext, aName, value, PROPERTY_SEARCH_CHILDREN);
    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = -1;
    }
    return retval;
  }

  int32_t
  Property :: setProperty(void *aContext, const char* aName, bool value)
  {
    return setProperty(aContext, aName, (int64_t)value);
  }
  
  int32_t
  Property :: setProperty(void *aContext, const char* aName, IRational* value)
  {
    int32_t retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
      
      if (!value)
        throw std::runtime_error("no rational value passed in");
   
      AVRational rational;
      rational.num = value->getNumerator();
      rational.den = value->getDenominator();
      retval = av_opt_set_q(aContext, aName, rational, PROPERTY_SEARCH_CHILDREN);
    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = -1;
    }
    return retval;
  }
  
  int32_t
  Property :: setProperty(void *aContext, IMetaData* aValuesToSet, IMetaData* aValuesNotFound)
  {
    int32_t retval =-1;
    AVDictionary *tmp = 0;
    MetaData* valuesToSet = dynamic_cast<MetaData*>(aValuesToSet);
    MetaData* valuesNotFound = dynamic_cast<MetaData*>(aValuesNotFound);
    AVDictionary *orig = valuesToSet ? valuesToSet->getDictionary() : 0;

    try {
      if (!aContext)
        throw std::runtime_error("no context passed in");

      if (orig)
        av_dict_copy(&tmp, orig, 0);

      // try setting the values.
      retval = av_opt_set_dict(aContext, &tmp);
      if (retval < 0)
        throw std::runtime_error("failed to set options on context");

      if (valuesNotFound)
        retval = valuesNotFound->copy(tmp);
      av_dict_free(&tmp);
      retval = 0;
    } catch (std::exception &e) {
      av_dict_free(&tmp);
      retval = -1;
    }
    return retval;
  }
  
}}}
