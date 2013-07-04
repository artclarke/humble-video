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
/*
 * Configurable.h
 *
 *  Created on: Jul 1, 2013
 *      Author: aclarke
 */

#ifndef CONFIGURABLE_H_
#define CONFIGURABLE_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/Property.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/KeyValueBag.h>

namespace io {
namespace humble {
namespace video {

/**
 * Methods that implement this class can be configured
 * using setProperty and getProperty methods, and expose
 * {@link IProperty} meta data about their properties.
 * <p>
 * You can use {@link Configuration#configure(java.util.Properties, Configurable)}
 * to easily configure these objects from Java properties or
 * from a FFmpeg preset file.
 * </p>
 * @see Configuration#configure(java.util.Properties, Configurable)
 *
 * @author aclarke
 *
 */
class Configurable : public ::io::humble::ferry::RefCounted
{
public:
  /**
   * Returns the total number of settable properties on this object
   *
   * @return total number of options (not including constant definitions)
   */
  virtual int32_t
  getNumProperties();

  /**
   * Returns the name of the numbered property.
   *
   * @param propertyNo The property number in the options list.
   *
   * @return a Property value for this properties meta-data
   */
  virtual Property *
  getPropertyMetaData(int32_t propertyNo);

  /**
   * Returns the name of the named property.
   *
   * @param name  The property name.
   *
   * @return a Property value for this properties meta-data
   */
  virtual Property *
  getPropertyMetaData(const char *name);

  /**
   * Sets a property on this Object.
   *
   * All AVOptions supported by the underlying AVClass are supported.
   *
   * @param name The property name.  For example "b" for bit-rate.
   * @param value The value of the property.
   *
   * @return >= 0 if the property was successfully set; <0 on error
   */
  virtual int32_t
  setProperty(const char *name, const char* value);

  /**
   * Looks up the property 'name' and sets the
   * value of the property to 'value'.
   *
   * @param name name of option
   * @param value Value of option
   *
   * @return >= 0 on success; <0 on error.
   */
  virtual int32_t
  setProperty(const char* name, double value);

  /**
   * Looks up the property 'name' and sets the
   * value of the property to 'value'.
   *
   * @param name name of option
   * @param value Value of option
   *
   * @return >= 0 on success; <0 on error.
   */
  virtual int32_t
  setProperty(const char* name, int64_t value);

  /**
   * Looks up the property 'name' and sets the
   * value of the property to 'value'.
   *
   * @param name name of option
   * @param value Value of option
   *
   * @return >= 0 on success; <0 on error.
   */
  virtual int32_t
  setProperty(const char* name, bool value);

  /**
   * Looks up the property 'name' and sets the
   * value of the property to 'value'.
   *
   * @param name name of option
   * @param value Value of option
   *
   * @return >= 0 on success; <0 on error.
   */
  virtual int32_t
  setProperty(const char* name, Rational *value);

#ifdef SWIG
  %newobject getPropertyAsString(const char*);
  %typemap(newfree) char * "free($1);";
#endif
  /**
   * Gets a property on this Object.
   *
   * <p>
   * Note for C++ callers; you must free the returned array with
   * delete[] in order to avoid a memory leak.  If you call
   * from Java or any other language, you don't need to worry
   * about this.
   * </p>
   *
   * @param name property name
   *
   * @return an string copy of the option value, or null if the option doesn't exist.
   */
  virtual char *
  getPropertyAsString(const char* name);

  /**
   * Gets the value of this property, and returns as a double;
   *
   * @param name name of option
   *
   * @return double value of property, or 0 on error.
   */
  virtual double
  getPropertyAsDouble(const char* name);

  /**
   * Gets the value of this property, and returns as an long;
   *
   * @param name name of option
   *
   * @return long value of property, or 0 on error.
   */
  virtual int64_t
  getPropertyAsLong(const char* name);

  /**
   * Gets the value of this property, and returns as an Rational;
   *
   * @param name name of option
   *
   * @return long value of property, or 0 on error.
   */
  virtual Rational *
  getPropertyAsRational(const char* name);

  /**
   * Gets the value of this property, and returns as a boolean
   *
   * @param name name of option
   *
   * @return boolean value of property, or false on error.
   */
  virtual bool
  getPropertyAsBoolean(const char* name);

  /**
   * Sets all properties in valuesToSet on this {@link Configurable} object.
   *
   * @param valuesToSet The set of key-value pairs to try to set
   * @param valuesNotFound If non null will contain all key-values pairs in valuesToSet
   *                       that were not found in context.
   *
   * @return 0 on success; <0 on failure
   */
  virtual int32_t
  setProperty(KeyValueBag* valuesToSet, KeyValueBag* valuesNotFound);
protected:
#ifndef SWIG
  virtual void *getCtx() { return 0; }
#endif // ! SWIG
  Configurable();
  virtual
  ~Configurable();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CONFIGURABLE_H_ */
