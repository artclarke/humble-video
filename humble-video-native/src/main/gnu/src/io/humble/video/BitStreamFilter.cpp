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
#include <io/humble/video/MediaPacketImpl.h>
#include <io/humble/video/KeyValueBagImpl.h>

#include "BitStreamFilter.h"

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE.BitStreamFilter);

namespace io { namespace humble { namespace video {

  BitStreamFilterType*
  BitStreamFilterType::make(const AVBitStreamFilter* f)
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

    const AVBitStreamFilter* f = 0;
    void *iterator=0;
    int32_t i = 0;
    while ((f = av_bsf_iterate(&iterator)) != 0) {
      ++i;
    }
    return i;
  }

  BitStreamFilterType*
  BitStreamFilterType::getBitStreamFilterType(int32_t index) {
    Global::init();

    const AVBitStreamFilter* f = 0;
    void *iterator = 0;
    int32_t i = 0;
    while ((f = av_bsf_iterate(&iterator)) != 0) {
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
    const AVBitStreamFilter* f = av_bsf_get_by_name(name);
    return f ? make(f) : 0;
  }


  BitStreamFilter::~BitStreamFilter ()
  {
    if (mCtx)
      av_bsf_free(&mCtx);
    mCtx = 0;
  }

  BitStreamFilter::BitStreamFilter (AVBSFContext* ctx,
                                    BitStreamFilterType *type)
  {
    mCtx = ctx;
    if (type) {
      mType.reset(type, true);
    } else {
      mType = BitStreamFilterType::make(mCtx->filter);
    }
    mState = STATE_INITED;
  }

  BitStreamFilter*
  BitStreamFilter::make(const char* filtername) {
    Global::init();
    if (!filtername || !*filtername)
      throw HumbleInvalidArgument("no filtername passed in");
    RefPointer<BitStreamFilterType> type = BitStreamFilterType::getBitStreamFilterType(filtername);
    if (!type)
      VS_THROW(HumbleInvalidArgument::make("Could not find filtername: %s", filtername));
    return make(type.value());
  }

  BitStreamFilter*
  BitStreamFilter::make(BitStreamFilterType *type) {
    if (!type)
      throw HumbleInvalidArgument("no filter type");
    const AVBitStreamFilter *filter = type->getCtx();
    AVBSFContext* b = 0;

    int e = av_bsf_alloc(filter, &b);
    FfmpegException::check(e, "could not initialize bitstream filter: %s", filter->name);
    return make(b, type);
  }

  BitStreamFilter*
  BitStreamFilter::make(AVBSFContext*c,
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

  void
  BitStreamFilter::open(KeyValueBag* inputOptions, KeyValueBag* aUnsetOptions) {
    AVDictionary* tmp=0;
    int retval = 0;

    try {
      if (mState != STATE_INITED)
        VS_THROW(HumbleRuntimeError("BitStreamFilter is not initialized correctly"));

      if (inputOptions) {
        KeyValueBagImpl* options = dynamic_cast<KeyValueBagImpl*>(inputOptions);
        // make a copy of the data returned.
        av_dict_copy(&tmp, options->getDictionary(), 0);
      }

      // first set any on the filter itself
      // first we're going to set options (and we'll set them again later)
      retval = av_opt_set_dict(mCtx, &tmp);
      FfmpegException::check(retval, "could not set options on coder");

      // then let's look for filter specific options
      if (mCtx->filter && mCtx->filter->priv_class) {
        // this filter has filter specific options, try that
        retval = av_opt_set_dict(mCtx->priv_data, &tmp);
      }

      // then initialize the filter
      retval = av_bsf_init(mCtx);
      FfmpegException::check(retval, "could not initialize BitStreamFilter");

      if (aUnsetOptions)
      {
        KeyValueBagImpl* unsetOptions = dynamic_cast<KeyValueBagImpl*>(aUnsetOptions);
        unsetOptions->copy(tmp);
      }
      mState = STATE_OPENED;
      if (tmp)
        av_dict_free(&tmp);

    } catch(...) {
      mState = STATE_ERROR;
      if (tmp)
        av_dict_free(&tmp);
      throw;
    }
  }

  MediaParameters*
  BitStreamFilter::getMediaParameters() {
    AVCodecParameters* p = mCtx->par_out;
    if (!p)
      VS_THROW(HumbleRuntimeError("output parameters unexpectedly missing"));

    RefPointer<Rational> timebase = Rational::make(
        mCtx->time_base_out.num,
        mCtx->time_base_out.den);

    return MediaParameters::make(p, timebase.value());
  }

  void
  BitStreamFilter::setMediaParameters(MediaParameters *p) {
    if (!p)
      VS_THROW(HumbleInvalidArgument("missing parameters passed in"));

    if (mState != STATE_OPENED)
      VS_THROW(HumbleRuntimeError("cannot set parameters once filter is open"));

    // copy the timebase
    RefPointer<Rational> timebase = p->getTimeBase();
    if (timebase) {
      mCtx->time_base_in.num = timebase->getNumerator();
      mCtx->time_base_in.den = timebase->getDenominator();
    }
    // copy the parameters
    int e = avcodec_parameters_copy(mCtx->par_in, p->getCtx());
    FfmpegException::check(e, "could not set input parameters");
  }


  ProcessorResult
  BitStreamFilter::sendPacket(MediaPacket* packet)
  {
    if (packet && !packet->isComplete())
      VS_THROW(HumbleRuntimeError("complete packet required"));

    // copy the packet... the bit stream filter takes ownership
    // of the underlying data
    AVPacket *pkt = 0;
    if (packet) {
      pkt = av_packet_clone((dynamic_cast<MediaPacketImpl*>(packet))->getCtx());
      if (!pkt)
        VS_THROW(HumbleBadAlloc());
    }
    int e = av_bsf_send_packet(mCtx, pkt);
    if (e != AVERROR(EAGAIN) && e != AVERROR_EOF)
      FfmpegException::check(e, "error sending bit stream packet");
    return (ProcessorResult)e;
  }

  ProcessorResult
  BitStreamFilter::receivePacket(MediaPacket* packet)
  {
    if (!packet)
      VS_THROW(HumbleRuntimeError("non-null packet required"));

    // reset all data.
    packet->reset(0);
    int e = av_bsf_receive_packet(mCtx, (dynamic_cast<MediaPacketImpl*>(packet))->getCtx());
    if (e != AVERROR(EAGAIN) && e != AVERROR_EOF)
      FfmpegException::check(e, "error sending bit stream packet");
    return (ProcessorResult)e;
    return RESULT_SUCCESS;
  }

} /* namespace video */
} /* namespace humble */
} /* namespace io */
