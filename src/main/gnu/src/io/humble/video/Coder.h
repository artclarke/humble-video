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
 * Coder.h
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#ifndef CODER_H_
#define CODER_H_

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/Rational.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO Coder : public io::humble::video::Configurable
{
public:
  typedef enum State {
    STATE_INITED,
    STATE_OPENED,
    STATE_ERROR,
  } State;

  typedef enum Flag {
    FLAG_UNALIGNED = CODEC_FLAG_UNALIGNED,
    /** Use fixed qscale. */
    FLAG_QSCALE = CODEC_FLAG_QSCALE,
    /** 4 MV per MB allowed / advanced prediction for H.263. */
    FLAG_4MV = CODEC_FLAG_4MV,
    /** Use qpel MC. */
    FLAG_QPEL = CODEC_FLAG_QPEL,
    /** Use GMC. */
    FLAG_GMC = CODEC_FLAG_GMC,
    /** Always try a MB with MV=<0,0>. */
    FLAG_MV0 = CODEC_FLAG_MV0,
    /**
     * The parent program guarantees that the input for B-frames containing
     * streams is not written to for at least s->max_b_frames+1 frames, if
     * this is not set the input will be copied.
     */
    FLAG_INPUT_PRESERVED = CODEC_FLAG_INPUT_PRESERVED,
    /** Use internal 2pass ratecontrol in first pass mode. */
    FLAG_PASS1 = CODEC_FLAG_PASS1,
    /** Use internal 2pass ratecontrol in second pass mode. */
    FLAG_PASS2 = CODEC_FLAG_PASS2,
    /** Only decode/encode grayscale. */
    FLAG_GRAY = CODEC_FLAG_GRAY,
    /** Don't draw edges. */
    FLAG_EMU_EDGE = CODEC_FLAG_EMU_EDGE,
    /** error[?] variables will be set during encoding. */
    FLAG_PSNR = CODEC_FLAG_PSNR,
              /** Input bitstream might be truncated at a random
                location instead of only at frame boundaries. */
    FLAG_TRUNCATED = CODEC_FLAG_TRUNCATED,
    /** Normalize adaptive quantization. */
    FLAG_NORMALIZE_AQP = CODEC_FLAG_NORMALIZE_AQP,
    /** Use interlaced DCT. */
    FLAG_INTERLACED_DCT = CODEC_FLAG_INTERLACED_DCT,
    /** Force low delay. */
    FLAG_LOW_DELAY = CODEC_FLAG_LOW_DELAY,
    /** Place global headers in extradata instead of every keyframe. */
    FLAG_GLOBAL_HEADER = CODEC_FLAG_GLOBAL_HEADER,
    /** Use only bitexact stuff (except (I)DCT). */
    FLAG_BITEXACT = CODEC_FLAG_BITEXACT,
    /* Fx : Flag for h263+ extra options */
    /** H.263 advanced intra coding / MPEG-4 AC prediction */
    FLAG_AC_PRED = CODEC_FLAG_AC_PRED,
    /** loop filter */
    FLAG_LOOP_FILTER = CODEC_FLAG_LOOP_FILTER,
    /** interlaced motion estimation */
    FLAG_INTERLACED_ME = CODEC_FLAG_INTERLACED_ME,
    FLAG_CLOSED_GOP = CODEC_FLAG_CLOSED_GOP,
  } Flag;

  typedef enum Flag2 {
    /** Allow non spec compliant speedup tricks. */
    FLAG2_FAST = CODEC_FLAG2_FAST,
    /** Skip bitstream encoding. */
    FLAG2_NO_OUTPUT = CODEC_FLAG2_NO_OUTPUT,
    /** Place global headers at every keyframe instead of in extradata. */
    FLAG2_LOCAL_HEADER = CODEC_FLAG2_LOCAL_HEADER,
    /** timecode is in drop frame format. DEPRECATED!!!! */
    FLAG2_DROP_FRAME_TIMECODE = CODEC_FLAG2_DROP_FRAME_TIMECODE,
    /** Discard cropping information from SPS. */
    FLAG2_IGNORE_CROP = CODEC_FLAG2_IGNORE_CROP,

    /** Input bitstream might be truncated at a packet boundaries instead of only at frame boundaries. */
    FLAG2_CHUNKS = CODEC_FLAG2_CHUNKS,
    /** Show all frames before the first keyframe */
    FLAG2_SHOW_ALL = CODEC_FLAG2_SHOW_ALL,
  } Flag2;

  /**
   * Open this Coder, using the given bag of Codec-specific options.
   *
   * @param inputOptions If non-null, a bag of codec-specific options.
   * @param unsetOptions If non-null, the bag will be emptied and then filled with
   *                     the options in <code>inputOptions</code> that were not set.
   *
   */
  virtual void open(KeyValueBag* inputOptions, KeyValueBag* unsetOptions);

  /**
   * The Codec this StreamCoder will use.
   *
   * @return The Codec used by this StreamCoder, or 0 (null) if none.
   */
  virtual Codec* getCodec() {
    return mCodec.get();
  }

  /**
   * A short hand for getCodec().getType().
   *
   * @return The Type of the Codec we'll use.
   */
  virtual MediaDescriptor::Type getCodecType() {
    return mCodec->getType();
  }

  /**
   *
   * A short hand for getCodec().getID().
   *
   * @return The ID of the Codec we'll use.
   */
  virtual Codec::ID getCodecID() {
    return mCodec->getID();
  }

  /**
   * The height, in pixels.
   *
   * @return The height of the video frames in the attached stream
   *   or -1 if an audio stream, or we cannot determine the height.
   */
  virtual int32_t getHeight() { return mCtx->height; }

  /**
   * Set the height, in pixels.
   *
   * @see #getHeight()
   *
   * @param height Sets the height of video frames we'll encode.  No-op when DECODING.
   */
  virtual void setHeight(int32_t height) { mCtx->height = height; }

  /**
   * The width, in pixels.
   *
   * @return The width of the video frames in the attached stream
   *   or -1 if an audio stream, or we cannot determine the width.
   */
  virtual int32_t getWidth() { return mCtx->width; }

  /**
   * Set the width, in pixels
   *
   * @see #getWidth()
   *
   * @param width Sets the width of video frames we'll encode.  No-op when DECODING.
   */
  virtual void setWidth(int32_t width) { mCtx->width = width; }

  /**
   * For Video streams, get the Pixel Format in use by the stream.
   *
   * @return the Pixel format, or PixelFormat::PIX_FMT_NONE if audio.
   */
  virtual PixelFormat::Type getPixelFormat() { return (PixelFormat::Type)mCtx->pix_fmt; }

  /**
   * Set the pixel format to ENCODE with.  Ignored if audio or
   * DECODING.
   *
   * @param pixelFmt Pixel format to use.
   */
  virtual void setPixelType(PixelFormat::Type pixelFmt) { mCtx->pix_fmt = (enum AVPixelFormat)pixelFmt; }

  /**
   * Get the sample rate we use for this coder.
   *
   * @return The sample rate (in Hz) we use for this stream, or -1 if unknown or video.
   */
  virtual int32_t getSampleRate() { return mCtx->sample_rate; }

  /**
   * Set the sample rate to use when ENCODING.  Ignored if DECODING
   * or a non-audio stream.
   *
   * @param sampleRate New sample rate (in Hz) to use.
   */
  virtual void setSampleRate(int32_t sampleRate) { mCtx->sample_rate=sampleRate; }

  /**
   * Get the audio sample format.
   *
   * @return The sample format of samples for encoding/decoding.
   */
  virtual AudioFormat::Type getSampleFormat() { return (AudioFormat::Type)mCtx->sample_fmt; }

  /**
   * Set the sample format when ENCODING.  Ignored if DECODING
   * or if the coder is already open.
   *
   * @param format The sample format.
   */
  virtual void setSampleFormat(AudioFormat::Type format) { mCtx->sample_fmt = (enum AVSampleFormat)format; }

  /**
   * Get the number of channels in this audio stream
   *
   * @return The sample rate (in Hz) we use for this stream, or 0 if unknown.
   */
  virtual int32_t getChannels() { return mCtx->channels; }

  /**
   * Set the number of channels to use when ENCODING.  Ignored if a
   * non audio stream, or if DECODING.
   *
   * @param channels The number of channels we'll encode with.
   */
  virtual void setChannels(int32_t channels) { mCtx->channels = channels; }

  /**
   * Get the channel layout.
   */
  virtual AudioChannel::Layout getChannelLayout() { return (AudioChannel::Layout)mCtx->channel_layout; }

  /** Set the channel layout */
  virtual void setChannelLayout(AudioChannel::Layout layout) {
    mCtx->channel_layout = layout;
  }

  /**
   * Get the time base this stream will ENCODE in, or the time base we
   * detect while DECODING.
   *
   * @return The time base this StreamCoder is using.
   */
  virtual Rational* getTimeBase();

  /**
   * Set the time base we'll use to ENCODE with.  A no-op when DECODING.
   *
   * As a convenience, we forward this call to the Stream#setTimeBase()
   * method.
   *
   * @see #getTimeBase()
   *
   * @param newTimeBase The new time base to use.
   */
  virtual void setTimeBase(Rational* newTimeBase);

  /**
   * Get the state of this coder.
   */
  virtual State getState() { return mState; }


#ifndef SWIG
  virtual void* getCtx() { return getCodecCtx(); }
  virtual AVCodecContext* getCodecCtx() { return mCtx; }
#endif

protected:
  virtual void setState(State state) { mState = state; }
  /*
   * Override to make a more specific allocator for frames.
   */
  virtual int prepareFrame(AVFrame* frame, int flags) {
    return avcodec_default_get_buffer2(mCtx, frame, flags);
  }
  Coder(Codec* codec, const AVCodecContext* src);
  virtual
  ~Coder();
  AVCodecContext *mCtx;
private:
  static int getBuffer(struct AVCodecContext *s, AVFrame *frame, int flags);

  io::humble::ferry::RefPointer<Codec> mCodec;
  io::humble::ferry::RefPointer<Rational> mTimebase;

  State mState;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CODER_H_ */
