/*
 * PropertyTest.cpp
 *
 *  Created on: Feb 2, 2012
 *      Author: aclarke
 */

#include <cstdlib>
#include <cstring>
#include "PropertyTest.h"
#include <io/humble/video/Property.h>
#if 0
#include <io/humble/video/ICodec.h>
#include <io/humble/video/IStreamCoder.h>
#endif

using namespace io::humble::video;

//VS_LOG_SETUP(VS_CPP_PACKAGE);


PropertyTest :: PropertyTest()
{

}

PropertyTest :: ~PropertyTest()
{
}

void
PropertyTest :: setUp()
{
  
}

void
PropertyTest :: tearDown()
{
  
}

void
PropertyTest :: testCreation()
{
#if 0
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);
  RefPointer<IStreamCoder> coder = IStreamCoder::make(IStreamCoder::ENCODING,
    ICodec::CODEC_ID_H264);
  RefPointer <Property> property =  coder->getPropertyMetaData("b");
  VS_LOG_DEBUG("Name: %s", property->getName());
  VS_LOG_DEBUG("Description: %s", property->getHelp());
  VS_TUT_ENSURE("should exist", property);
#endif
}

void
PropertyTest :: testIteration()
{
#if 0
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);

  RefPointer<IStreamCoder> coder = IStreamCoder::make(IStreamCoder::ENCODING,
    ICodec::CODEC_ID_H264);

  int32_t numProperties = coder->getNumProperties();
  VS_TUT_ENSURE("", numProperties > 0);

  for(int32_t i = 0; i < numProperties; i++)
  {
    RefPointer <Property> property =  coder->getPropertyMetaData(i);
    VS_LOG_DEBUG("Name: %s", property->getName());
    VS_LOG_DEBUG("Description: %s", property->getHelp());
    VS_LOG_DEBUG("Default: %lld", property->getDefault());
    VS_LOG_DEBUG("Current value (boolean) : %d", (int32_t)coder->getPropertyAsBoolean(property->getName()));
    VS_LOG_DEBUG("Current value (double)  : %f", coder->getPropertyAsDouble(property->getName()));
    VS_LOG_DEBUG("Current value (long)    : %lld", coder->getPropertyAsLong(property->getName()));
    RefPointer<IRational> rational = coder->getPropertyAsRational(property->getName());
    VS_LOG_DEBUG("Current value (rational): %f", rational->getValue());
    char* value=coder->getPropertyAsString(property->getName());
    VS_LOG_DEBUG("Current value (string)  : %s", value);
    if (value) free(value);
  }
#endif
}

void
PropertyTest :: testSetMetaData()
{
#if 0
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);

  RefPointer<IStreamCoder> coder = IStreamCoder::make(IStreamCoder::ENCODING,
    ICodec::CODEC_ID_H264);
  RefPointer<IMetaData> dict = IMetaData::make();
  RefPointer<IMetaData> unset = IMetaData::make();
  const char* realKey = "b";
  const char* fakeKey = "not-a-valid-key-no-way-all-hail-zod";
  const char* realValue = "1000";
  const char* fakeValue = "1025";
  dict->setValue(realKey, realValue);
  dict->setValue(fakeKey, fakeValue);

  VS_TUT_ENSURE("", dict->getNumKeys() == 2);
  VS_TUT_ENSURE("", unset->getNumKeys() == 0);

  VS_TUT_ENSURE("", coder->setProperty(dict.value(), unset.value()) == 0);

  VS_TUT_ENSURE("", coder->getPropertyAsLong(realKey) == 1000);

  // make sure the fake isn't there.
  RefPointer<Property> fakeProperty = coder->getPropertyMetaData(fakeKey);
  VS_TUT_ENSURE("", !fakeProperty);

  // now make sure the returned dictionary only had the fake in it.
  VS_TUT_ENSURE("", unset->getNumKeys() == 1);

  VS_TUT_ENSURE("", strcmp(unset->getValue(fakeKey, IMetaData::METADATA_NONE), fakeValue) == 0);
#endif
}

