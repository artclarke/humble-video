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
 * Codec.swg
 *
 *  Created on: Jun 29, 2013
 *      Author: aclarke
 */

%typemap (javacode) io::humble::video::CodecDescriptor,io::humble::video::CodecDescriptor*,io::humble::video::CodecDescriptor& %{

  /**
    * info about this {@link CodecDescriptor} object
    * @return information about this object
    */
    
   @Override
   public String toString()
   {
     StringBuilder result = new StringBuilder();
     
     result.append(this.getClass().getName()+"@"+hashCode()+"[");
     result.append("name:"+getName()+";");
     result.append("longname:"+getName()+";");
     result.append("properties:"+getProperties()+";");
     result.append("]");
     return result.toString();
   }

%}

%typemap (javacode) io::humble::video::Codec,io::humble::video::Codec*,io::humble::video::Codec& %{
  /**
   * Prints the type and id of this codec
   * @return the type and id
   */
   
  @Override
  public String toString()
  {
    StringBuilder result = new StringBuilder();
    
    result.append(this.getClass().getName()+"@"+hashCode()+"[");
    result.append("type="+getType()+";");
    result.append("id="+getID()+";");
    result.append("name="+getName()+";");
    result.append("]");
    return result.toString();
  }

  /**
   * Gets a collection of ALL codecs installed on this
   * system. 
   * 
   * @return A collection of all installed codecs.
   */

  public static java.util.Collection<Codec>
  getInstalledCodecs()
  {
    java.util.Collection<Codec> retval = new java.util.HashSet<Codec>();
    int count = getNumInstalledCodecs();
    for(int i=0;i<count;i++)
    {
      Codec codec = getInstalledCodec(i);
      if (codec != null)
        retval.add(codec);
    }
    return retval;
  }

  /**
   * Returns a list of supported frame-rates this codec can encode
   * video to.
   * 
   * <p>
   * 
   * Not all codecs support reporting this, in which case the returned list
   * will be empty.
   * 
   * </p>
   * 
   * @return a list
   */

  public java.util.List<Rational>
  getSupportedVideoFrameRates()
  {
    java.util.List<Rational> retval =
      new java.util.LinkedList<Rational>();
    int count = getNumSupportedVideoFrameRates();
    for(int i=0;i<count;i++)
    {
      Rational rate = getSupportedVideoFrameRate(i);
      if (rate != null)
        retval.add(rate);
    }
    return retval;
  }

  /**
   * Returns a list of supported pixel formats this codec can encode
   * video in.
   * 
   * <p>
   * 
   * Not all codecs support reporting this, in which case the returned list
   * will be empty.
   * 
   * </p>
   * 
   * @return a list
   */

  public java.util.List<PixelFormat.Type>
  getSupportedVideoPixelFormats()
  {
    java.util.List<PixelFormat.Type> retval =
      new java.util.LinkedList<PixelFormat.Type>();
    int count = getNumSupportedVideoPixelFormats();
    for(int i=0;i<count;i++)
    {
      PixelFormat.Type type = getSupportedVideoPixelFormat(i);
      if (type != null && type != PixelFormat.Type.FMT_NONE)
        retval.add(type);
    }
    return retval;
  }

  /**
   * Returns a list of supported audio sample rates this codec can encode
   * audio in.
   * 
   * <p>
   * 
   * Not all codecs support reporting this, in which case the returned list
   * will be empty.
   * 
   * </p>
   * 
   * @return a list
   */

  public java.util.List<Integer>
  getSupportedAudioSampleRates()
  {
    java.util.List<Integer> retval =
      new java.util.LinkedList<Integer>();
    int count = getNumSupportedAudioSampleRates();
    for(int i=0;i<count;i++)
    {
      int rate = getSupportedAudioSampleRate(i);
      if (rate != 0)
        retval.add(rate);
    }
    return retval;
  }

  /**
   * Returns a list of supported audio sample formats this codec can encode
   * audio in.
   * 
   * <p>
   * 
   * Not all codecs support reporting this, in which case the returned list
   * will be empty.
   * 
   * </p>
   * 
   * @return a list
   */

  public java.util.List<AudioFormat.Type>
  getSupportedAudioFormats()
  {
    java.util.List<AudioFormat.Type> retval =
      new java.util.LinkedList<AudioFormat.Type>();
    int count = getNumSupportedAudioFormats();
    for(int i=0;i<count;i++)
    {
      AudioFormat.Type fmt = getSupportedAudioFormat(i);
      if (fmt != null && fmt != AudioFormat.Type.SAMPLE_FMT_NONE)
        retval.add(fmt);
    }
    return retval;
  }

  /**
   * Returns a list of supported audio channel layouts this codec can encode
   * audio in.
   * 
   * <p>
   * 
   * Not all codecs support reporting this, in which case the returned list
   * will be empty.
   * 
   * </p>
   * 
   * @return a list
   */

  public java.util.List<Long>
  getSupportedAudioChannelLayouts()
  {
    java.util.List<Long> retval =
      new java.util.LinkedList<Long>();
    int count = getNumSupportedAudioChannelLayouts();
    for(int i=0;i<count;i++)
    {
      long layout = getSupportedAudioChannelLayout(i);
      if (layout != 0)
        retval.add(layout);
    }
    return retval;
  }

%}

%include <io/humble/video/Codec.h>
