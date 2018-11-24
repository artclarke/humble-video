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

#ifndef SWIG
class MediaAudio;
class MediaPicture;
#endif

/**
 * An object that either converts MediaRaw objects into MediaEncoded
 * objects (called an Encoder), or converts in the reverse direction (called
 * a Decoder).
 *
 * Although this object might seem very sparse, use the methods on Configurable
 * to see the vast array of options that can be get/set on it.
 */
class VS_API_HUMBLEVIDEO Coder : public io::humble::video::Configurable
{
public:
  /**
   * The state that a Coder can be in.
   */
  typedef enum State {
    STATE_INITED,
    STATE_OPENED,
    STATE_FLUSHING,
    STATE_ERROR,
  } State;

  typedef enum Flag {
    /**
     * Allow decoders to produce frames with data planes that are not aligned
     * to CPU requirements (e.g. due to cropping).
     */
    FLAG_UNALIGNED = AV_CODEC_FLAG_UNALIGNED,

    /**
     * Use fixed qscale.
     */
    FLAG_QSCALE = AV_CODEC_FLAG_QSCALE,

    /**
     * 4 MV per MB allowed / advanced prediction for H.263.
     */
    FLAG_4MV = AV_CODEC_FLAG_4MV,

    /**
     * Output even those frames that might be corrupted.
     */
    FLAG_OUTPUT_CORRUPT = AV_CODEC_FLAG_OUTPUT_CORRUPT,

    /**
     * Use qpel MC.
     */
    FLAG_QPEL = AV_CODEC_FLAG_QPEL,

    /**
     * Use internal 2pass ratecontrol in first pass mode.
     */
    FLAG_PASS1 = AV_CODEC_FLAG_PASS1,

    /**
     * Use internal 2pass ratecontrol in second pass mode.
     */
    FLAG_PASS2 = AV_CODEC_FLAG_PASS2,

    /**
     * loop filter.
     */
    FLAG_LOOP_FILTER= AV_CODEC_FLAG_LOOP_FILTER,

    /**
     * Only decode/encode grayscale.
     */
    FLAG_GRAY = AV_CODEC_FLAG_GRAY,

    /**
     * error[?] variables will be set during encoding.
     */
    FLAG_PSNR = AV_CODEC_FLAG_PSNR,

    /**
     * Input bitstream might be truncated at a random location
     * instead of only at frame boundaries.
     */
    FLAG_TRUNCATED = AV_CODEC_FLAG_TRUNCATED,

    /**
     * Use interlaced DCT.
     */
    FLAG_INTERLACED_DCT = AV_CODEC_FLAG_INTERLACED_DCT,

    /**
     * Force low delay.
     */
    FLAG_LOW_DELAY = AV_CODEC_FLAG_LOW_DELAY,

    /**
     * Place global headers in extradata instead of every keyframe.
     */
    FLAG_GLOBAL_HEADER = AV_CODEC_FLAG_GLOBAL_HEADER,
    /**
     * Use only bitexact stuff (except (I)DCT).
     */
    FLAG_BITEXACT = AV_CODEC_FLAG_BITEXACT,
    /**
     * H.263 advanced intra coding / MPEG-4 AC prediction
     */
    FLAG_AC_PRED = AV_CODEC_FLAG_AC_PRED,
    /**
     * interlaced motion estimation
     */
    FLAG_INTERLACED_ME = AV_CODEC_FLAG_INTERLACED_ME,
    FLAG_CLOSED_GOP = AV_CODEC_FLAG_CLOSED_GOP,

  } Flag;

  typedef enum Flag2 {
    /**
     * Allow non spec compliant speedup tricks.
     */
    FLAG2_FAST = AV_CODEC_FLAG2_FAST,
    /**
     * Skip bitstream encoding.
     */
    FLAG2_NO_OUTPUT = AV_CODEC_FLAG2_NO_OUTPUT,
    /**
     * Place global headers at every keyframe instead of in extradata.
     */
    FLAG2_LOCAL_HEADER = AV_CODEC_FLAG2_LOCAL_HEADER,

    /**
     * timecode is in drop frame format. DEPRECATED!!!!
     */
    FLAG2_DROP_FRAME_TIMECODE = AV_CODEC_FLAG2_DROP_FRAME_TIMECODE,

    /**
     * Input bitstream might be truncated at a packet boundaries
     * instead of only at frame boundaries.
     */
    FLAG2_CHUNKS = AV_CODEC_FLAG2_CHUNKS,
    /**
     * Discard cropping information from SPS.
     */
    FLAG2_IGNORE_CROP = AV_CODEC_FLAG2_IGNORE_CROP,

    /**
     * Show all frames before the first keyframe
     */
    FLAG2_SHOW_ALL = AV_CODEC_FLAG2_SHOW_ALL,
    /**
     * Export motion vectors through frame side data
     */
    FLAG2_EXPORT_MVS = AV_CODEC_FLAG2_EXPORT_MVS,
    /**
     * Do not skip samples and export skip information as frame side data
     */
    FLAG2_SKIP_MANUAL = AV_CODEC_FLAG2_SKIP_MANUAL,
    /**
     * Do not reset ASS ReadOrder field on flush (subtitles decoding)
     */
    FLAG2_RO_FLUSH_NOOP = AV_CODEC_FLAG2_RO_FLUSH_NOOP,
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
  virtual void setPixelFormat(PixelFormat::Type pixelFmt) { mCtx->pix_fmt = (enum AVPixelFormat)pixelFmt; }

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

  /**
   * Get the number of frames encoded/decoded so far.
   * If encoding or decoding has an error, this count is <b>not</b> incremented.
   */
  virtual int32_t getFrameCount() { return mCtx->frame_number; }
  /**
   * Number of samples per channel in an audio frame.
   *
   * - encoding: Each submitted frame
   *   except the last must contain exactly frame_size samples per channel.
   *   May be 0 when the codec has CODEC_CAP_VARIABLE_FRAME_SIZE set, then the
   *   frame size is not restricted.
   * - decoding: may be set by some decoders to indicate constant frame size
   *
   * @return number of samples per channel.
   */
  virtual int32_t getFrameSize();

  /**
   * Get the 32-bit compliment of all flags.
   */
  virtual int32_t getFlags();
  /**
   * Check if a given flag is set.
   */
  virtual int32_t getFlag(Flag flag);
  /**
   * Get the 32-bit compliment of all flags 2 flags
   */
  virtual int32_t getFlags2();
  /**
   * Check if a given flag2 flag is set.
   */
  virtual int32_t getFlag2(Flag2 flag);
  /**
   * Set a new bitmask of all Flag values (bit xored together).
   */
  virtual void setFlags(int32_t newVal);
  /**
   * Set a flag to true or false.
   */
  virtual void setFlag(Flag flag, bool value);
  /**
   * Set a new bitmask of all Flag2 values (bit xored together).
   */
  virtual void setFlags2(int32_t newVal);
  /**
   * Set a flag2 to true or false.
   */
  virtual void setFlag2(Flag2 flag, bool value);

#ifndef SWIG
  virtual void* getCtx() { return getCodecCtx(); }
  virtual AVCodecContext* getCodecCtx() { return mCtx; }
#endif

protected:
  virtual void setState(State state);
  /*
   * Override to make a more specific allocator for frames.
   */
  virtual int prepareFrame(AVFrame* frame, int flags) {
    return avcodec_default_get_buffer2(mCtx, frame, flags);
  }
  Coder(Codec* codec, AVCodecContext* src, bool copySrc);
  virtual
  ~Coder();
  /**
   * Make sure the audio passed in has parameters that match this coder.
   */
  void ensureAudioParamsMatch(MediaAudio* audio);
  void ensurePictureParamsMatch(MediaPicture* audio);

  // This gets called after options are set on a Codec but before
  // open. Subclasses should override to check or prepare any
  // other contexts they need before open. Throw an exception
  // if not ready to proceed.
  virtual void checkOptionsBeforeOpen() {};
private:

  AVCodecContext *mCtx;
  static int getBuffer(struct AVCodecContext *s, AVFrame *frame, int flags);

  io::humble::ferry::RefPointer<Codec> mCodec;
  io::humble::ferry::RefPointer<Rational> mTimebase;

  State mState;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CODER_H_ */
