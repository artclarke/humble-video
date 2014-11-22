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
 * BitStreamFilter.cpp
 *
 *  Created on: Sep 28, 2014
 *      Author: aclarke
 */

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/Global.h>

#include "BitStreamFilter.h"

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.BitStreamFilter);

namespace io { namespace humble { namespace video {

  BitStreamFilterType*
  BitStreamFilterType::make(AVBitStreamFilter* f)
  {
    if (!f)
      throw HumbleInvalidArgument("no filter passed in");
    BitStreamFilterType* r = new BitStreamFilterType(f);
    r->acquire();
    return r;
  }

  int32_t
  BitStreamFilterType::getNumBitStreamFilterTypes() {
    Global::init();

    AVBitStreamFilter* f = 0;
    int32_t i = 0;
    while ((f = av_bitstream_filter_next(f)) != 0) {
      ++i;
    }
    return i;
  }

  BitStreamFilterType*
  BitStreamFilterType::getBitStreamFilterType(int32_t index) {
    Global::init();

    AVBitStreamFilter* f = 0;
    int32_t i = 0;
    while ((f = av_bitstream_filter_next(f)) != 0) {
      if (i == index) {
        VS_LOG_TRACE("Found filter \"%s\" at position %d",
                     f->name,
                     i);
        // this is the one we want
        break;
      }
      ++i;
    }
    return f ? make(f) : 0;
  }

  BitStreamFilterType*
  BitStreamFilterType::getBitStreamFilterType(const char* name) {
    Global::init();

    AVBitStreamFilter* f = 0;
    int32_t i = 0;
    while ((f = av_bitstream_filter_next(f)) != 0) {
      if (strcmp(name, f->name)==0) {
        VS_LOG_TRACE("Found filter \"%s\" at position %d",
                     f->name,
                     i);
        // this is the one we want
        break;
      }
      ++i;
    }
    return f ? make(f) : 0;
  }


  BitStreamFilter::~BitStreamFilter ()
  {
    if (mCtx)
      av_bitstream_filter_close(mCtx);
    mCtx = 0;
  }
  BitStreamFilter::BitStreamFilter (AVBitStreamFilterContext* ctx,
                                    BitStreamFilterType *type)
  {
    mCtx = ctx;
    if (type) {
      mType.reset(type, true);
    } else {
      mType = BitStreamFilterType::make(mCtx->filter);
    }
  }

  BitStreamFilter*
  BitStreamFilter::make(const char* filtername) {
    Global::init();

    if (!filtername || !*filtername)
      throw HumbleInvalidArgument("no filtername passed in");
    AVBitStreamFilterContext* b = av_bitstream_filter_init(filtername);
    if (!b)
      throw HumbleBadAlloc();
    return make(b, 0);
  }

  BitStreamFilter*
  BitStreamFilter::make(BitStreamFilterType *type) {
    if (!type)
      throw HumbleInvalidArgument("no filter type");
    AVBitStreamFilterContext* b = av_bitstream_filter_init(type->getName());
    if (!b)
      throw HumbleBadAlloc();
    return make(b, type);
  }

  BitStreamFilter*
  BitStreamFilter::make(AVBitStreamFilterContext*c,
                        BitStreamFilterType* t)
  {
    Global::init();
    if (!c)
      throw HumbleInvalidArgument("no filter context");

    BitStreamFilter* r = new BitStreamFilter(c, t);
    if (!r)
      throw HumbleBadAlloc();
    r->acquire();
    return r;
  }

  int32_t
  BitStreamFilter::filter(Buffer* output,
                          int32_t outputOffset,
                          Buffer* input,
                          int32_t inputOffset,
                          int32_t inputSize,
                          Coder* coder,
                          const char* args,
                          bool isKey)
  {
    if (!output)
      throw HumbleInvalidArgument("no output");
    if (outputOffset < 0)
      throw HumbleInvalidArgument("output offset < 0");
    if (!input)
      throw HumbleInvalidArgument("no input");
    if (inputSize <= 0)
      throw HumbleInvalidArgument("inputSize <= 0");
    if (inputOffset < 0)
      throw HumbleInvalidArgument("input offset < 0");


    // get the raw bytes for input and output
    int32_t outputSize = inputSize;
    uint8_t* out = 0;
    int e = -1;
    uint8_t* in = static_cast<uint8_t*>(input->getBytes(inputOffset, inputSize));

    try {
      if (!in)
        throw HumbleRuntimeError("could not get input bytes");

      AVCodecContext* avctx = coder ? coder->getCodecCtx() : 0;
      e = av_bitstream_filter_filter(mCtx, avctx, args,
                                     &out, &outputSize,
                                     in, inputSize, isKey);
      FfmpegException::check(e, "could not filter buffer");
      if (e == 0)
        // see documentation of av_bitstream_filter_filter
        if (!out) out = in;

      // we ALWAYS copy the buffers when filtering.
      uint8_t* outB = static_cast<uint8_t*>(output->getBytes(outputOffset, outputSize));
      if (!outB)
        throw HumbleRuntimeError("could not get output bytes; buffer may not be large enough");
      memcpy(outB, out, outputSize);
      if (e){
        // the output buffer was allocated.
        av_free(out);
      }
    } catch (...) {
      if (e){
        // the output buffer was allocated.
        av_free(out);
      }
    }
    return outputSize;
  }

} /* namespace video */
} /* namespace humble */
} /* namespace io */
