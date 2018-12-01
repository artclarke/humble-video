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
#include <io/humble/video/Processor.h>
#include <io/humble/video/MediaPacket.h>

namespace io { namespace humble { namespace video {

  /**
   * A descriptor for the different kinds of BitStreamFilterTypes supported within Humble.
   *
   * @see BitStreamFilter
   */
  class VS_API_HUMBLEVIDEO BitStreamFilterType : public io::humble::ferry::RefCounted
  {
  public:
    virtual const char* getName() const { return mCtx->name; }
    static int32_t getNumBitStreamFilterTypes();
    static BitStreamFilterType* getBitStreamFilterType(int32_t i);
    static BitStreamFilterType* getBitStreamFilterType(const char* name);
#ifndef SWIG
    static BitStreamFilterType* make(const AVBitStreamFilter* c);
    const AVBitStreamFilter* getCtx() { return mCtx; }
#endif
  private:
    virtual ~BitStreamFilterType() {}

    BitStreamFilterType(const AVBitStreamFilter*ctx) : mCtx(ctx) {}
    const AVBitStreamFilter* mCtx;
  };

  /**
   * Takes a bit stream and filters bytes passed
   * in to add, remove, or modify the bit-stream.
   *
   */
  class VS_API_HUMBLEVIDEO BitStreamFilter : public io::humble::video::Configurable,
    public virtual ProcessorEncodedSink,
    public virtual ProcessorEncodedSource
  {
  public:

    /**
     * The state that a BitStreamFilter can be in.
     */
    typedef enum State {
      STATE_INITED,
      STATE_OPENED,
      STATE_FLUSHING,
      STATE_ERROR,
    } State;

    /**
     * Get the state this BitStreamFilter is in.
     */
    virtual State getState() const { return mState; }

    /**
     * Opens this bitstream filter. Callers are responsible for
     * calling setProperty(...) on this with the appropriate
     * properties before opening.
     *
     * @param inputOptions If non-null, a bag of filter-specific options.
     * @param unsetOptions If non-null, the bag will be emptied and then filled with
     *                     the options in <code>inputOptions</code> that were not set.
     *
     */
    virtual void open(KeyValueBag* inputOptions, KeyValueBag* unsetOptions);

    virtual MediaParameters *getMediaParameters();
    virtual void setMediaParameters(MediaParameters* parameters);

    /**
     * Submit a packet for filtering.
     *
     * After sending each packet, the filter must be completely drained by calling
     * receive() repeatedly until it returns a non RESULT_SUCCESS result.
     *
     * @param packet the packet to filter. This parameter may be NULL, which signals the end of the stream (i.e. no more
     * packets will be sent). That will cause the filter to output any packets it
     * may have buffered internally.
     *
     * @return RESULT_SUCCESS on success, or RESULT_AWAITING_DATA if the user should
     *   call #receive(Media) first.
     */
    virtual ProcessorResult sendPacket(MediaPacket* packet);
    virtual ProcessorResult sendEncoded(MediaEncoded* media) {
      MediaPacket* m = dynamic_cast<MediaPacket*>(media);
      if (media && !m)
        throw io::humble::ferry::HumbleRuntimeError("expected MediaPacket object");
      else
        return sendPacket(m);
    }

    /**
     * Retrieve a filtered packet.
     *
     * @param packet this struct will be filled with the contents of the filtered
     *                 packet. If this function returns
     *                 successfully, the contents of packet will be completely
     *                 overwritten by the returned data.
     *
     * @return RESULT_SUCCESS on success. RESULT_AWAITING_DATA if more packets need
     * to be sent to the filter to get more output. RESULT_END_OF_STREAM if there
     * will be no further output from the filter. Otherwise an exception will
     * be thrown for errors.
     *
     * @note one input packet may result in several output packets, so after sending
     * a packet with #send(Media), this function needs to be called
     * repeatedly until it stops returning RESULT_SUCCESS.
     * It is also possible for a filter to
     * output fewer packets than were sent to it, so this function may return
     * RESULT_AWAITING_DATA immediately after a successful #send(Media) call.
     */
    virtual ProcessorResult receivePacket(MediaPacket* packet);
    virtual ProcessorResult receiveEncoded(MediaEncoded* media) {
      MediaPacket* m = dynamic_cast<MediaPacket*>(media);
      if (media && !m)
        throw io::humble::ferry::HumbleRuntimeError("expected MediaPacket object");
      else
        return receivePacket(m);
    }


    /**
     * Create a filter given the name.
     *
     * @param filtername The name of the filter.
     *
     * @see BitStreamFilterType
     */
    static BitStreamFilter* make(const char* filtername);

    /**
     * Conveniece make that calls #setParameters().
     */
    static BitStreamFilter* make(const char* filtername, MediaParameters* p)
    {
      io::humble::ferry::RefPointer<BitStreamFilter> retval;
      if (!p)
        throw io::humble::ferry::HumbleInvalidArgument("expected non null parameters");
      retval = make(filtername);
      if (retval)
        retval->setMediaParameters(p);
      return retval.get();
    }

    /**
     * Create a filter given the type.
     *
     * @param type The type of the filter.
     *
     * @see BitStreamFilterType
     */
    static BitStreamFilter* make(BitStreamFilterType* type);
    /**
     * Conveniece make that calls #setParameters().
     */
    static BitStreamFilter* make(BitStreamFilterType* type, MediaParameters* p)
    {
      io::humble::ferry::RefPointer<BitStreamFilter> retval;
      if (!p)
        throw io::humble::ferry::HumbleInvalidArgument("expected non null parameters");
      retval = make(type);
      if (retval)
        retval->setMediaParameters(p);
      return retval.get();
    }


#ifndef SWIG
    static BitStreamFilter* make(AVBSFContext *c,
                                 BitStreamFilterType* t);
#endif

    /**
     * Get the type of this filter.
     */
    virtual BitStreamFilterType* getType() { return mType.get(); }

    /**
     * Get the name of this filter.
     */
    virtual const char* getName() const { return mType->getName(); }

  protected:
    virtual void* getCtx() { return mCtx; }
  private:
    virtual ~BitStreamFilter ();
    BitStreamFilter (AVBSFContext* ctx, BitStreamFilterType *type);
    AVBSFContext* mCtx;
    State mState;
    io::humble::ferry::RefPointer<BitStreamFilterType> mType;
  };

} /* namespace video */
} /* namespace humble */
} /* namespace io */

#endif /* BITSTREAMFILTER_H_ */
