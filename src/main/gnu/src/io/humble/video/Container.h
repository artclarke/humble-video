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
 *
 * Container.h
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/ContainerFormat.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO Stream : public io::humble::ferry::RefCounted
{
public:
  virtual void noop() {}

protected:
  Stream() {}
  virtual ~Stream() {}
};

class Property;
class MetaData;
class Rational;

/**
 * A Container for Media data. This is an abstract class and
 * cannot be instantiated on its own.
 */
class VS_API_HUMBLEVIDEO Container : public io::humble::ferry::RefCounted
{
public:
  /**
   * Do not set these flags -- several are used by the internals of Humble Video.
   */
  typedef enum Flag
  {
    /**  Generate missing pts even if it requires parsing future frames. **/
    FLAG_GENPTS = AVFMT_FLAG_GENPTS,
    /**  Ignore index. **/
    FLAG_IGNIDX = AVFMT_FLAG_IGNIDX,
    /**  Do not block when reading packets from input. **/
    FLAG_NONBLOCK = AVFMT_FLAG_NONBLOCK,
    /**  Ignore DTS on frames that contain both DTS & PTS **/
    FLAG_IGNDTS = AVFMT_FLAG_IGNDTS,
    /**  Do not infer any values from other values, just return what is stored in the container **/
    FLAG_NOFILLIN = AVFMT_FLAG_NOFILLIN,
    /**  Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled **/
    FLAG_NOPARSE = AVFMT_FLAG_NOPARSE,
    /**  Do not buffer frames when possible **/
    FLAG_NOBUFFER = AVFMT_FLAG_NOBUFFER,
    /**  The caller has supplied a custom AVIOContext, don't avio_close() it. **/
    FLAG_CUSTOM_IO = AVFMT_FLAG_CUSTOM_IO,
    /**  Discard frames marked corrupted **/
    FLAG_DISCARD_CORRUPT = AVFMT_FLAG_DISCARD_CORRUPT,
    /**  Enable RTP MP4A-LATM payload **/
    FLAG_MP4A_LATM = AVFMT_FLAG_MP4A_LATM,
    /**  try to interleave outputted packets by dts (using this flag can slow demuxing down) **/
    FLAG_SORT_DTS = AVFMT_FLAG_SORT_DTS,
    /**  Enable use of private options by delaying codec open (this could be made default once all code is converted) **/
    FLAG_PRIV_OPT = AVFMT_FLAG_PRIV_OPT,
    /**  Don't merge side data but keep it separate. **/
    FLAG_KEEP_SIDE_DATA = AVFMT_FLAG_KEEP_SIDE_DATA,

  } Flag;
  /**
   * Is this container opened?
   * @return true if opened; false if not.
   */
  virtual bool
  isOpened()=0;

  /**
   * Returns the ContainerFormat object being used for this Container,
   * or null if the {@link Container} doesn't yet know.
   *
   * @return the ContainerFormat object, or null.
   */
  virtual ContainerFormat *
  getFormat()=0;

  /**
   * Close the container.  open() must have been called first, or
   * else an error is returned.<p>If the current thread is interrupted while this blocking method
   * is running the method will return with a negative value.
   * To check if the method exited because of an interruption
   * pass the return value to {@link Error#make(int)} and then
   * check {@link Error#getType()} to see if it is
   * {@link Error.Type#ERROR_INTERRUPTED}.
   * </p>
   * <p>
   * If this method exits because of an interruption,
   * all resources will be closed anyway.
   * </p>
   *
   * @return >= 0 on success; < 0 on error.
   */
  virtual int32_t
  close()=0;

  /**
   * The number of streams in this container.
   * <p>If opened in {@link Container.Type#READ} mode, this will query the stream and find out
   * how many streams are in it.</p><p>If opened in
   * {@link Container.Type#WRITE} mode, this will return the number of streams
   * the caller has added to date.</p><p>If the current thread is interrupted while this blocking method
   * is running the method will return with a negative value.
   * To check if the method exited because of an interruption
   * pass the return value to {@link Error#make(int)} and then
   * check {@link Error#getType()} to see if it is
   * {@link Error.Type#ERROR_INTERRUPTED}.
   * </p>
   *
   * @return The number of streams in this container.
   */
  virtual int32_t
  getNumStreams()=0;

  /**
   * Get the stream at the given position.
   *
   * @param streamIndex the index of this stream in the container
   * @return The stream at that position in the container, or null if none there.
   */
  virtual Stream*
  getStream(uint32_t streamIndex)=0;

  /**
   * Gets the duration, if known, of this container.
   *
   * This will only work for non-streamable containers where Container
   * can calculate the container size.
   *
   * @return The duration, or {@link Global#NO_PTS} if not known.
   */
  virtual int64_t
  getDuration()=0;

  /**
   * Get the starting timestamp in microseconds of the first packet of the earliest stream in this container.
   * <p>
   * This will only return value values either either (a) for non-streamable
   * containers where Container can calculate the container size or
   * (b) after Container has actually read the
   * first packet from a streamable source.
   * </p>
   *
   * @return The starting timestamp in microseconds, or {@link Global#NO_PTS} if not known.
   */
  virtual int64_t
  getStartTime()=0;

  /**
   * Get the file size in bytes of this container.
   *
   * This will only return a valid value if the container is non-streamed and supports seek.
   *
   * @return The file size in bytes, or <0 on error.
   */
  virtual int64_t
  getFileSize()=0;

  /**
   * Get the calculated overall bit rate of this file.
   * <p>
   * This will only return a valid value if the container is non-streamed and supports seek.
   * </p>
   * @return The overall bit rate in bytes per second, or <0 on error.
   */
  virtual int32_t
  getBitRate()=0;

  /**
   * Returns the total number of settable properties on this object
   *
   * @return total number of options (not including constant definitions)
   */
  virtual int32_t
  getNumProperties()=0;

  /**
   * Returns the name of the numbered property.
   *
   * @param propertyNo The property number in the options list.
   *
   * @return an Property value for this properties meta-data
   */
  virtual Property *
  getPropertyMetaData(int32_t propertyNo)=0;

  /**
   * Returns the name of the numbered property.
   *
   * @param name  The property name.
   *
   * @return an Property value for this properties meta-data
   */
  virtual Property *
  getPropertyMetaData(const char *name)=0;

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
  setProperty(const char *name, const char* value)=0;

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
  setProperty(const char* name, double value)=0;

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
  setProperty(const char* name, int64_t value)=0;

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
  setProperty(const char* name, bool value)=0;

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
  setProperty(const char* name, Rational *value)=0;

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
  getPropertyAsString(const char* name)=0;

  /**
   * Gets the value of this property, and returns as a double;
   *
   * @param name name of option
   *
   * @return double value of property, or 0 on error.
   */
  virtual double
  getPropertyAsDouble(const char* name)=0;

  /**
   * Gets the value of this property, and returns as an long;
   *
   * @param name name of option
   *
   * @return long value of property, or 0 on error.
   */
  virtual int64_t
  getPropertyAsLong(const char* name)=0;

  /**
   * Gets the value of this property, and returns as an Rational;
   *
   * @param name name of option
   *
   * @return long value of property, or 0 on error.
   */
  virtual Rational *
  getPropertyAsRational(const char* name)=0;

  /**
   * Gets the value of this property, and returns as a boolean
   *
   * @param name name of option
   *
   * @return boolean value of property, or false on error.
   */
  virtual bool
  getPropertyAsBoolean(const char* name)=0;

  /**
   * Get the flags associated with this object.
   *
   * @return The (compacted) value of all flags set.
   */
  virtual int32_t
  getFlags()=0;

  /**
   * Get the setting for the specified flag
   *
   * @param flag The flag you want to find the setting for
   *
   * @return 0 for false; non-zero for true
   */
  virtual bool
  getFlag(Flag flag) = 0;

  /**
   * Set the flag.
   *
   * @param flag The flag to set
   * @param value The value to set it to (true or false)
   *
   */
  virtual void
  setFlag(Flag flag, bool value) = 0;

  /**
   * Set the flags to use with this object.  All values
   * must be ORed (|) together.
   *
   * @see Flags
   *
   * @param newFlags The new set flags for this codec.
   */
  virtual void setFlags(int32_t newFlags) = 0;


  /**
   * Get the URL the Container was opened with.
   * May return null if unknown.
   * @return the URL opened, or null.
   */
  virtual const char*
  getURL()=0;

  /**
   * Get the {@link IMetaData} for this object,
   * or null if none.
   * <p>
   * If the {@link Container} or {@link Stream} object
   * that this {@link IMetaData} came from was opened
   * for reading, then changes via {@link IMetaData#setValue(String, String)}
   * will have no effect on the underlying media.
   * </p>
   * <p>
   * If the {@link Container} or {@link Stream} object
   * that this {@link IMetaData} came from was opened
   * for writing, then changes via {@link IMetaData#setValue(String, String)}
   * will have no effect after {@link Sink#writeHeader()}
   * is called.
   * </p>
   * @return the {@link IMetaData}.
   */
  virtual MetaData*
  getMetaData()=0;


  /**
   * {@inheritDoc}
   */
  virtual int32_t
  setProperty(MetaData* valuesToSet, MetaData* valuesNotFound)=0;

protected:
  Container();
  virtual
  ~Container();
protected:
  AVFormatContext *mCtx;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CONTAINER_H_ */
