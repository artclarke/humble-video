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
 * InputFormat.h
 *
 *  Created on: Jun 29, 2013
 *      Author: aclarke
 */

#ifndef INPUTFORMAT_H_
#define INPUTFORMAT_H_

#include <io/humble/video/ContainerFormat.h>
#include <io/humble/video/Codec.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO SourceFormat : public io::humble::video::ContainerFormat
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(SourceFormat)
public:
  /**
   * Name for format.
   */
  virtual const char*
  getName()
  {
    return mFormat->name;
  }

  /**
   * Descriptive name for the format, meant to be more human-readable
   * than name.
   */
  virtual const char*
  getLongName()
  {
    return mFormat->long_name;
  }

  /** A comma-separated list of supported filename extensions */
  virtual const char*
  getExtensions()
  {
    return mFormat->extensions;
  }

  /**
   * Flags that tell you what capabilities this format supports.
   *
   * @return a bitmask of {@link Flags}
   */
  virtual int32_t
  getFlags()
  {
    return mFormat->flags;
  }
  /**
   * Get total number of different codecs this container can output.
   */
  virtual int32_t
  getNumSupportedCodecs()
  {
    return ContainerFormat::getNumSupportedCodecs(mFormat->codec_tag);
  }
  /**
   * Get the CodecId for the n'th codec supported by this container.
   *
   * @param n The n'th codec supported by this codec. Lower n are higher priority.
   *   n must be < {@link #getNumSupportedCodecs()}
   * @return the {@link CodecId} at the n'th slot, or {@link CodecId.ID_NONE} if none.
   */
  virtual Codec::ID
  getSupportedCodecId(int32_t n)
  {
    return ContainerFormat::getSupportedCodecId(mFormat->codec_tag, n);
  }
  /**
   * Get the 32-bit Codec Tag for the n'th codec supported by this container.
   *
   * @param n The n'th codec supported by this codec. Lower n are higher priority.
   *   n must be < {@link #getNumSupportedCodecs()}
   * @return the codec tag at the n'th slot, or 0 if none.
   */
  virtual uint32_t
  getSupportedCodecTag(int32_t n)
  {
    return ContainerFormat::getSupportedCodecTag(mFormat->codec_tag, n);
  }
  /**
   * Find {@link InputFormat} based on the short name of the input format.
   * @return An {@link InputFormat} or null if none found.
   */
  static
  SourceFormat *findFormat(const char *shortName);

  /**
   * Get the number of input formats this install can demultiplex (read)
   * from.
   *
   * @return the number of formats
   */
  static int32_t getNumFormats();

  /**
   * Return an object for the input format at the given index.
   *
   * @param index an index for the input format list we maintain
   *
   * @return a format object for that input or null if
   *   unknown, index < 0 or index >= {@link #getNumInstalledInputFormats()}
   */
  static SourceFormat* getFormat(int32_t index);

#ifndef SWIG
  virtual AVInputFormat* getCtx() { return mFormat; }
  static SourceFormat*
  make(AVInputFormat* format);
#endif // ! SWIG

protected:
private:
  SourceFormat();
  virtual
  ~SourceFormat();

private:
  AVInputFormat *mFormat;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* INPUTFORMAT_H_ */
