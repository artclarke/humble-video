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

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/PropertyImpl.h>
#include <io/humble/video/KeyValueBagImpl.h>
#include <io/humble/video/Error.h>
extern "C" {
#include "FfmpegIncludes.h"
#include <libavutil/log.h>
}

#include <stdexcept>
#include <cstring>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::ferry;

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
    RefPointer<PropertyImpl> retval = 0;
    if (!start)
      VS_THROW(HumbleInvalidArgument("no start passed in"));

    if (!option)
      VS_THROW(HumbleInvalidArgument("no option passed in"));

    retval = PropertyImpl::make();
    retval ->mOptionStart = start;
    retval->mOption = option;

    return retval.get();
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
    RefPointer<Property> retval;
    const AVOption* last = 0;

    if (!aContext)
      VS_THROW(HumbleInvalidArgument("No context passed in"));

    if (aPropertyNo < 0)
      VS_THROW(HumbleInvalidArgument("Property Number must be >= 0"));
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
    return retval.get();
  }

  Property*
  PropertyImpl :: getPropertyMetaData(void *aContext, const char *aName)
  {
    RefPointer<Property> retval;
    const AVOption* last = 0;

    if (!aContext)
      VS_THROW(HumbleInvalidArgument("No context passed in"));

    if (!aName || !*aName)
      VS_THROW(HumbleInvalidArgument("No name passed in"));

    last = av_opt_find(aContext, aName, 0, 0, 0);
    if (last)
    {
      if (last->type != AV_OPT_TYPE_CONST)
      {
        retval = PropertyImpl::make(av_opt_next(aContext, 0), last);
      }
    }
    return retval.get();
  }
  
  int32_t
  PropertyImpl :: setProperty(void *aContext, const char* aName, const char *aValue)
  {
    int32_t retval = -1;

    if (!aContext)
      VS_THROW(HumbleInvalidArgument("No context passed in"));

    if (!aName  || !*aName)
      VS_THROW(HumbleInvalidArgument("No name passed in"));

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

    return retval;
  }

  char*
  PropertyImpl :: getPropertyAsString(void *aContext, const char *aName)
  {
    char* retval = 0;
    char *value = 0;

    if (!aContext)
      VS_THROW(HumbleInvalidArgument("No context passed in"));

    if (!aName  || !*aName)
      VS_THROW(HumbleInvalidArgument("No name passed in"));

    int32_t errorcode = av_opt_get(aContext, aName, 0, (uint8_t**)&value);
    if (errorcode < 0) {
      RefPointer<Error> error = Error::make(errorcode);
      VS_THROW(HumbleRuntimeError::make("Could not get property %s: %s", aName, error->getDescription()));
    }

    if (value)
    {
      // let's make a copy of the data
      int32_t valLen = strlen(value);
      if (valLen > 0)
      {
        retval = (char*)malloc((valLen+1)*sizeof(char));
        if (!retval) {
          av_freep(&value);
          VS_THROW(HumbleBadAlloc());
        }

        // copy the data
        strncpy(retval, value, valLen+1);
      }
      av_freep(&value);
    }

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
    if (getType() != Property::PROPERTY_FLAGS)
      VS_THROW(HumbleRuntimeError("flag is not of type PROPERTY_FLAGS"));

    // now, iterate through all options, counting all CONSTS that belong
    // to the same unit as this option
    const char* unit = getUnit();
    if (!unit || !*unit)
      VS_THROW(HumbleRuntimeError("flag doesn't have a unit setting, so can't tell what constants"));

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
    return retval;
  }

  Property *
  PropertyImpl :: getFlagConstant(int32_t position)
  {
    RefPointer<Property> retval;

    if (getType() != Property::PROPERTY_FLAGS)
      VS_THROW(HumbleRuntimeError("flag is not of type PROPERTY_FLAGS"));

    // now, iterate through all options, counting all CONSTS that belong
    // to the same unit as this option
    const char* unit = getUnit();
    if (!unit || !*unit)
      VS_THROW(HumbleRuntimeError("flag doesn't have a unit setting, so can't tell what constants"));

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

    return retval.get();
  }

  Property *
  PropertyImpl :: getFlagConstant(const char* aName)
  {
    RefPointer<Property> retval;
    if (getType() != Property::PROPERTY_FLAGS)
      VS_THROW(HumbleRuntimeError("flag is not of type PROPERTY_FLAGS"));

    // now, iterate through all options, counting all CONSTS that belong
    // to the same unit as this option
    const char* unit = getUnit();
    if (!unit || !*unit)
      VS_THROW(HumbleRuntimeError("flag doesn't have a unit setting, so can't tell what constants"));

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

    return retval.get();
  }
  
  double
  PropertyImpl :: getPropertyAsDouble(void *aContext, const char* aName)
  {
    double retval = 0;
    if (!aContext)
      VS_THROW(HumbleInvalidArgument("no context passed in"));

    if (!aName  || !*aName)
      VS_THROW(HumbleInvalidArgument("empty property name passed to getProperty"));

    if (av_opt_get_double(aContext, aName, 0, &retval) < 0)
      VS_THROW(HumbleRuntimeError::make("error getting property \"%s\"", aName));
    return retval;
  }
  
  int64_t
  PropertyImpl :: getPropertyAsLong(void *aContext, const char* aName)
  {
    int64_t retval = 0;
    if (!aContext)
      VS_THROW(HumbleInvalidArgument("no context passed in"));

    if (!aName  || !*aName)
      VS_THROW(HumbleInvalidArgument("empty property name passed to getProperty"));

    if (av_opt_get_int(aContext, aName, 0, &retval) < 0)
      VS_THROW(HumbleRuntimeError::make("error getting property \"%s\"", aName));

    return retval;
  }
  
  Rational*
  PropertyImpl :: getPropertyAsRational(void *aContext, const char* aName)
  {
    RefPointer<Rational> retval;
    if (!aContext)
      VS_THROW(HumbleInvalidArgument("no context passed in"));

    if (!aName  || !*aName)
      VS_THROW(HumbleInvalidArgument("empty property name passed to getProperty"));

    AVRational value;
    if (av_opt_get_q(aContext, aName, 0, &value)<0)
      VS_THROW(HumbleRuntimeError::make("error getting property \"%s\"", aName));

    retval = Rational::make(value.num, value.den);

    return retval.get();
  }
  
  int32_t
  PropertyImpl :: getPropertyAsInt(void *aContext, const char* aName)
  {
    return (int32_t) getPropertyAsLong(aContext, aName);
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
  

  int32_t
  PropertyImpl :: setProperty(void *aContext, KeyValueBag* aValuesToSet, KeyValueBag* aValuesNotFound)
  {
    int32_t retval =-1;
    AVDictionary *tmp = 0;
    KeyValueBagImpl* valuesToSet = dynamic_cast<KeyValueBagImpl*>(aValuesToSet);
    KeyValueBagImpl* valuesNotFound = dynamic_cast<KeyValueBagImpl*>(aValuesNotFound);
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
