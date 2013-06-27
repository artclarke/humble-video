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

#ifndef IPROPERTY_H_
#define IPROPERTY_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefCounted.h>
namespace io { namespace humble { namespace video {
  /**
   * Represents settable properties that effect how Humble Video objects
   * operate.
   * <p>
   * For example, setting the &quot;b&quot; property on an
   * {@link IStreamCoder} sets the bit-rate the coder will attempt
   * to encode at.
   * </p> 
   */
  class VS_API_HUMBLEVIDEO IProperty : public io::humble::ferry::RefCounted
  {
  public:
    /**
     * The different type of options that are supported by Humble Video.
     * 
     * Well, actually by FFMPEG, but you get the idea.
     */
    typedef enum {
      AV_OPT_TYPE_FLAGS,
      AV_OPT_TYPE_INT,
      AV_OPT_TYPE_INT64,
      AV_OPT_TYPE_DOUBLE,
      AV_OPT_TYPE_FLOAT,
      AV_OPT_TYPE_STRING,
      AV_OPT_TYPE_RATIONAL,
      AV_OPT_TYPE_BINARY,
      AV_OPT_TYPE_CONST = 128,
      AV_OPT_TYPE_IMAGE_SIZE = MKBETAG('S','I','Z','E'),
      AV_OPT_TYPE_PIXEL_FMT  = MKBETAG('P','F','M','T'),
      AV_OPT_TYPE_SAMPLE_FMT = MKBETAG('S','F','M','T'),
    } Type;
    
    typedef enum {
      FLAG_ENCODING_PARAM=1,
      FLAG_DECODING_PARAM=2,
      FLAG_METADATA=4,
      FLAG_AUDIO_PARAM=8,
      FLAG_VIDEO_PARAM=16,
      FLAG_SUBTITLE_PARAM=32,
      FLAG_FILTERING_PARAM=(1<<16),
    } Flags;
    
    /**
     * How to search options when looking for different values.
     */
    typedef enum {
        /** Do not search child options */
        PROPERTY_SEARCH_DEFAULT=0x0000,
        /** Search children first */
        PROPERTY_SEARCH_CHILDREN=0x0001,
    } Search;

    /**
     * Get the name for this property.
     * 
     * @return the name.
     */
    virtual const char *getName()=0;
    
    /**
     * Get the (English) help string for this property.
     * 
     * @return the help string
     */
    virtual const char *getHelp()=0;
    
    /**
     * Get any sub-unit this option or constant belongs to.
     * 
     * @return the unit, or null if none.
     */
    virtual const char *getUnit()=0;
    
    /**
     * Get the underlying native type of this property.
     * 
     * @return the type
     */
    virtual Type getType()=0;
    
    /**
     * Get any set flags (a bitmask) for this option.
     * 
     * @return the flags
     */
    virtual int32_t getFlags()=0;

    /**
     * Get the default setting this flag would have it not set.
     * 
     * @return the default
     */
    virtual int64_t getDefault()=0;
    
    /**
     * Get the default setting this flag would have it not set.
     * 
     * @return the default
     */
    virtual double getDefaultAsDouble()=0;
    
    /**
     * If this IProperty is of the type {@link Type#AV_OPT_TYPE_FLAGS}, this method will
     * tell you how many different flag settings it takes.
     * 
     * @return Number of flag settings, or <0 if not a FLAGS value
     */
    virtual int32_t getNumFlagSettings()=0;
    
    /**
     * If this IProperty is of the type {@link Type#AV_OPT_TYPE_FLAGS}, this method will
     * give you another IProperty representing a constant setting for that flag.
     * 
     * @param position The position number for the flag;  Must be in range 0 <= position <= #getNumFlagSettings().
     * 
     * @return An IProperty object for the flag setting, or null if not available.
     */
    virtual IProperty *getFlagConstant(int32_t position)=0;

    /**
     * If this IProperty is of the type {@link Type#AV_OPT_TYPE_FLAGS}, this method will
     * give you another IProperty representing a constant setting for that flag.
     * 
     * @param name The name of the constant.
     * 
     * @return An IProperty object for the flag setting, or null if not available.
     */
    virtual IProperty *getFlagConstant(const char *name)=0;

  protected:
    IProperty();
    virtual ~IProperty();
    
  };

}}}
#endif /* IPROPERTY_H_ */
