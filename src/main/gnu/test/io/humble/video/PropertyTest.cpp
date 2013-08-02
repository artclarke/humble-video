/*
 * PropertyTest.cpp
 *
 *  Created on: Feb 2, 2012
 *      Author: aclarke
 */

#include <cstdlib>
#include <cstring>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include "PropertyTest.h"
#include <io/humble/video/Property.h>
#include <io/humble/video/VideoExceptions.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Source.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

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
PropertyTest::testValgrindStrlenIssue()
{
  // This is a bug in FFmpeg which I fixed in our
  // captive build. The error crops up for BINARY
  // option types that have no data in them.
  // This test tries to ensure we have a patched FFmpeg.
  RefPointer<Configurable> c = Source::make();

  {
    LoggerStack stack;
    stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

    char* value = c->getPropertyAsString("cryptokey");
    if (value) free(value);
  }
}
void
PropertyTest :: testCreation()
{
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);
  RefPointer<Configurable> c = Source::make();
  RefPointer<Property> property =  c->getPropertyMetaData("packetsize");
  VS_LOG_DEBUG("Name: %s", property->getName());
  VS_LOG_DEBUG("Description: %s", property->getHelp());
  TSM_ASSERT("should exist", property);
}

void
PropertyTest :: testIteration()
{
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);

  RefPointer<Configurable> c = Source::make();

  int32_t numProperties = c->getNumProperties();
  TSM_ASSERT("", numProperties > 0);

  for(int32_t i = 0; i < numProperties; i++)
  {
    RefPointer <Property> property =  c->getPropertyMetaData(i);
    const char* name = property->getName();
    VS_LOG_DEBUG("Name: %s", name);
    VS_LOG_DEBUG("Description: %s", property->getHelp());
    VS_LOG_DEBUG("Default: %lld", property->getDefault());
    if (strcmp(name, "cryptokey")==0)
      continue;
    VS_LOG_DEBUG("Current value (boolean) : %d", (int32_t)c->getPropertyAsBoolean(name));
    VS_LOG_DEBUG("Current value (double)  : %f", c->getPropertyAsDouble(name));
    VS_LOG_DEBUG("Current value (long)    : %lld", c->getPropertyAsLong(name));
    RefPointer<Rational> rational = c->getPropertyAsRational(name);
    VS_LOG_DEBUG("Current value (rational): %f", rational->getValue());
    char* value=c->getPropertyAsString(name);
    VS_LOG_DEBUG("Current value (string)  : %s", value);
    if (value) free(value);
  }
}

void
PropertyTest :: testSetMetaData()
{
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_ERROR, false);

  RefPointer<Configurable> c = Source::make();
  RefPointer<KeyValueBag> dict = KeyValueBag::make();
  RefPointer<KeyValueBag> unset = KeyValueBag::make();
  const char* realKey = "packetsize";
  const char* fakeKey = "not-a-valid-key-no-way-all-hail-zod";
  const char* realValue = "1000";
  const char* fakeValue = "1025";
  dict->setValue(realKey, realValue);
  dict->setValue(fakeKey, fakeValue);

  TSM_ASSERT("", dict->getNumKeys() == 2);
  TSM_ASSERT("", unset->getNumKeys() == 0);

  c->setProperty(dict.value(), unset.value());

  TSM_ASSERT("", c->getPropertyAsLong(realKey) == 1000);

  // make sure the fake isn't there.
  TS_ASSERT_THROWS(c->getPropertyMetaData(fakeKey), PropertyNotFoundException);

  // now make sure the returned dictionary only had the fake in it.
  TSM_ASSERT("", unset->getNumKeys() == 1);

  TSM_ASSERT("", strcmp(unset->getValue(fakeKey, KeyValueBag::KVB_NONE), fakeValue) == 0);
}

