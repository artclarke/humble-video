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

#include <io/humble/ferry/Logger.h>
#include <io/humble/video/PropertyImpl.h>
extern "C" {
#include "FfmpegIncludes.h"
#include <libavutil/log.h>
}

#include <stdexcept>
#include <cstring>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video {

  PropertyImpl :: PropertyImpl()
  {
    mOption=0;
    mOptionStart=0;
  }

  PropertyImpl :: ~PropertyImpl()
  {
  }

  PropertyImpl*
  PropertyImpl :: make(const AVOption *start, const AVOption *option)
  {
    PropertyImpl *retval = 0;
    try
    {
      if (!start)
        throw std::bad_alloc();
      if (!option)
        throw std::bad_alloc();
      retval = PropertyImpl::make();
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
  PropertyImpl :: getName()
  {
    return mOption ? mOption->name : 0;
  }
  
  const char *
  PropertyImpl :: getHelp()
  {
    return mOption ? mOption->help : 0;
  }
  
  const char *
  PropertyImpl :: getUnit()
  {
    return mOption ? mOption->unit : 0;
  }
  
  Property::Type
  PropertyImpl :: getType()
  {
    return mOption ? (Property::Type) mOption->type : Property::PROPERTY_UNKNOWN;
  }

  int64_t
  PropertyImpl :: getDefault()
  {
    return (int64_t)getDefaultAsDouble();
  }
    
  double
  PropertyImpl :: getDefaultAsDouble()
  {
    return mOption ? mOption->default_val.dbl: 0.0;
  }
  
  int32_t
  PropertyImpl :: getFlags()
  {
    return mOption ? mOption->flags : 0;
  }
  
  int32_t
  PropertyImpl :: getNumProperties(void *aContext)
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
  
  Property*
  PropertyImpl :: getPropertyMetaData(void *aContext, int32_t aPropertyNo)
  {
    Property *retval = 0;
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
              retval = PropertyImpl::make(av_opt_next(aContext, 0), last);
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

  Property*
  PropertyImpl :: getPropertyMetaData(void *aContext, const char *aName)
  {
    Property *retval = 0;
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
          retval = PropertyImpl::make(av_opt_next(aContext, 0), last);
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
  PropertyImpl :: setProperty(void *aContext, const char* aName, const char *aValue)
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
  PropertyImpl :: getPropertyAsString(void *aContext, const char *aName)
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
  PropertyImpl :: getNumFlagSettings()
  {
    int32_t retval = 0;
    try {
      if (getType() != Property::PROPERTY_FLAGS)
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

  Property *
  PropertyImpl :: getFlagConstant(int32_t position)
  {
    Property *retval = 0;
    try
    {
      if (getType() != Property::PROPERTY_FLAGS)
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
            retval = PropertyImpl::make(av_opt_next(&fakeClassPtr, 0), last);
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

  Property *
  PropertyImpl :: getFlagConstant(const char* aName)
  {
    Property *retval = 0;
    try
    {
      if (getType() != Property::PROPERTY_FLAGS)
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
          retval = PropertyImpl::make(av_opt_next(&fakeClass, 0), last);
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
  PropertyImpl :: getPropertyAsDouble(void *aContext, const char* aName)
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
  PropertyImpl :: getPropertyAsLong(void *aContext, const char* aName)
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
  
  Rational*
  PropertyImpl :: getPropertyAsRational(void *aContext, const char* aName)
  {
    Rational *retval = 0;
    try
    {
      if (!aContext)
        throw std::runtime_error("no context passed in");
      
      if (!aName  || !*aName)
        throw std::runtime_error("empty property name passed to setProperty");
      
      AVRational value = av_get_q(aContext, aName, 0);
      retval = Rational::make(value.num, value.den);
    }
    catch (std::exception &e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      VS_REF_RELEASE(retval);
    }
    return retval;
  }
  

  bool
  PropertyImpl :: getPropertyAsBoolean(void *aContext, const char* aName)
  {
    return (bool) getPropertyAsLong(aContext, aName);
  }
  
  int32_t
  PropertyImpl :: setProperty(void *aContext, const char* aName, double value)
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
  PropertyImpl :: setProperty(void *aContext, const char* aName, int64_t value)
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
  PropertyImpl :: setProperty(void *aContext, const char* aName, bool value)
  {
    return setProperty(aContext, aName, (int64_t)value);
  }
  
  int32_t
  PropertyImpl :: setProperty(void *aContext, const char* aName, Rational* value)
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
  
#if 0
  int32_t
  PropertyImpl :: setProperty(void *aContext, IMetaData* aValuesToSet, IMetaData* aValuesNotFound)
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
#endif // 0
  
}}}
