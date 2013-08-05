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
 * FilterLink.h
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#ifndef FILTERLINK_H_
#define FILTERLINK_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/Filter.h>

namespace io {
namespace humble {
namespace video {

/**
 * A link between two {@link Filter}s in a {@link FilterGraph}.
 */
class FilterLink : public io::humble::ferry::RefCounted
{
public:
  /**
   * Get the {@link FilterGraph} this {@link FilterLink} belongs to.
   */
  virtual FilterGraph* getFilterGraph();
  /**
   * @return the input filter for this link.
   */
  virtual Filter* getInputFilter();

  /**
   * @return the input pad name for this link.
   */
  virtual const char* getInputPadName();

  /**
   * @return the input pad type for this link. Should be the same as {@link #getMediaType()}.
   */
  virtual MediaDescriptor::Type getInputPadType();

  /**
   * @return the output filter for this link.
   */
  virtual Filter* getOutputFilter();
  /**
   * @return the output pad name for this link.
   */
  virtual const char* getOutputPadName();
  /**
   * @return the output pad type for this link. Should be the same as {@link #getMediaType()}.
   */
  virtual MediaDescriptor::Type getOutputPadType();

  /**
   * @return the {@link MediaDescriptor.Type} for this link.
   */
  virtual MediaDescriptor::Type getMediaType();

  /**
   * @return width of video agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_VIDEO}.
   */
  virtual int32_t getWidth() { return mCtx->w; };
  /**
   * @return height of video agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_VIDEO}.
   */
  virtual int32_t getHeight() { return mCtx->h; }

  /**
   * @return pixel format of video agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_VIDEO}.
   */
  virtual PixelFormat::Type getPixelFormat() { return (PixelFormat::Type) mCtx->format; }
  /**
   * @return pixel aspect ratio of video agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_VIDEO}.
   */
  virtual Rational* getPixelAspectRatio();

  /**
   * @return channel layout agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_AUDIO}.
   */
  virtual AudioChannel::Layout getChannelLayout() { return (AudioChannel::Layout) mCtx->channel_layout; }
  /**
   * @return sample rate agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_AUDIO}.
   */
  virtual int32_t getSampleRate() { return mCtx->sample_rate; }

  /**
   * @return sample format agreed between the input filter and output filter if {@link MediaDescriptor.Type} = {@link MediaDescriptor.Type.MEDIA_AUDIO}.
   */
  virtual AudioFormat::Type getSampleFormat() { return (AudioFormat::Type) mCtx->format; }

  /**
   * Define the time base used by the PTS of the frames/samples
   * which will pass through this link.
   * During the configuration stage, each filter is supposed to
   * change only the output timebase, while the timebase of the
   * input link is assumed to be an unchangeable property.
   *
   * @return the timebase
   */
  virtual Rational* getTimeBase();

#ifndef SWIG
  virtual AVFilterLink* getCtx() { return mCtx; }
  static FilterLink* make(FilterGraph*, AVFilterLink*);
#endif // ! SWIG
protected:
  FilterLink(FilterGraph* graph, AVFilterLink *link);
  virtual
  ~FilterLink();
private:
  AVFilterLink* mCtx;
  io::humble::ferry::RefPointer<FilterGraph> mGraph;
  io::humble::ferry::RefPointer<Filter> mInputFilter;
  io::humble::ferry::RefPointer<Filter> mOutputFilter;
  io::humble::ferry::RefPointer<Rational> mTimeBase;
  io::humble::ferry::RefPointer<Rational> mPixelAspectRatio;

};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* FILTERLINK_H_ */
