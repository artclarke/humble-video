/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
/*
 * BitStreamFilter.h
 *
 *  Created on: Sep 28, 2014
 *      Author: aclarke
 */

#ifndef BITSTREAMFILTER_H_
#define BITSTREAMFILTER_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/Buffer.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Coder.h>
#include <io/humble/video/MediaPacket.h>


namespace io { namespace humble { namespace video {

  /**
   * A descriptor for the different kinds of BitStreamFilterTypes supported within Humble.
   *
   * @see BitStreamFilter
   */
  class BitStreamFilterType : public io::humble::ferry::RefCounted
  {
  public:
    virtual const char* getName() { return mCtx->name; }
    static int32_t getNumBitStreamFilterTypes();
    static BitStreamFilterType* getBitStreamFilterType(int32_t i);
    static BitStreamFilterType* getBitStreamFilterType(const char* name);
#ifndef SWIG
    static BitStreamFilterType* make(AVBitStreamFilter* c);
#endif
  private:
    virtual ~BitStreamFilterType() {}

    BitStreamFilterType(AVBitStreamFilter*ctx) : mCtx(ctx) {}
    AVBitStreamFilter* mCtx;
  };

  /**
   * Takes a bit stream and filters bytes passed
   * in to add, remove, or modify the bit-stream.
   *
   */
  class BitStreamFilter : public io::humble::ferry::RefCounted
  {
  public:
    /**
     * Create a filter given the name.
     *
     * @param filtername The name of the filter.
     *
     * @see BitStreamFilterType
     */
    static BitStreamFilter* make(const char* filtername);

    /**
     * Create a filter given the type.
     *
     * @param type The type of the filter.
     *
     * @see BitStreamFilterType
     */
    static BitStreamFilter* make(BitStreamFilterType* type);

#ifndef SWIG
    static BitStreamFilter* make(AVBitStreamFilterContext *c,
                                 BitStreamFilterType* t);
#endif

    /**
     * Get the type of this filter.
     */
    virtual BitStreamFilterType* getType() { return mType.get(); }

    /**
     * Get the name of this filter.
     */
    virtual const char* getName() { return mType->getName(); }

    /**
     * Filter the input buffer into the output buffer.
     *
     * @param output The output buffer to copy filtered bytes to.
     * @param outputOffset The offset in output to copy data into.
     * @param input The input buffer to filter.
     * @param inputOffset The number of bytes into input to start filtering at.
     * @param inputSize The number of bytes (from inputOffset) to filter.
     * @param coder The Coder context belong to the Stream that this data will eventually get outputted to.
     * @param args String arguments for the filter call. See the FFmpeg documentation or source code.
     * @param isKey Does this data represent a key packet.
     *
     * @return The number of bytes copied into output once filtering is done.
     *
     * @throw InvalidArgument if any parameters are invalid.
     * @throw FfmpegException if the filtering fails for any reason.
     *
     */
    virtual int32_t filter(io::humble::ferry::Buffer* output,
           int32_t outputOffset,
           io::humble::ferry::Buffer* input,
           int32_t inputOffset,
           int32_t inputSize,
           Coder* coder,
           const char* args,
           bool isKey);
    /*
    virtual void filter(MediaPacket* output, MediaPacket* input, const char* args);

*/
  private:
    virtual ~BitStreamFilter ();
    BitStreamFilter (AVBitStreamFilterContext* ctx, BitStreamFilterType *type);
    AVBitStreamFilterContext* mCtx;
    io::humble::ferry::RefPointer<BitStreamFilterType> mType;
  };

} /* namespace video */
} /* namespace humble */
} /* namespace io */

#endif /* BITSTREAMFILTER_H_ */
