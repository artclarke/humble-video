/*******************************************************************************
 * Copyright (c) 2018, Andrew "Art" Clarke.  All rights reserved.
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
 * MediaParameters.h
 *
 *  Created on: Nov 30, 2018
 *      Author: aclarke
 */

#ifndef SRC_MAIN_GNU_SRC_IO_HUMBLE_VIDEO_MEDIAPARAMETERS_H_
#define SRC_MAIN_GNU_SRC_IO_HUMBLE_VIDEO_MEDIAPARAMETERS_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Global.h>
#include <io/humble/video/Codec.h>

namespace io {
namespace humble {
namespace video {

  /**
   * A virtual class for MediaParameters
   */
  class VS_API_HUMBLEVIDEO MediaParameters : public virtual io::humble::ferry::RefCounted
  {
    VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaParameters);

  private:
    MediaParameters();
    virtual ~MediaParameters();
    io::humble::ferry::RefPointer<Rational> mTimeBase;
    AVCodecParameters *mCtx;
  public:
#ifndef SWIG
    AVCodecParameters* getCtx() { return mCtx; }
    static MediaParameters *make(AVCodecParameters* ctx, Rational* tb);
#endif // ! SWIG


    /** Get the timebase for this MediaParameters set of properties */
    virtual Rational* getTimeBase() { return mTimeBase.get(); }
    /** Sets the timebase */
    virtual void setTimeBase(Rational* tb) { mTimeBase = tb; }

    /**
     * General type of the encoded data.
     */
    virtual MediaDescriptor::Type
    getType () const
    {
      return (MediaDescriptor::Type) mCtx->codec_type;
    }
    /**
     * Specific type of the encoded data (the codec used).
     */
    virtual Codec::ID
    getID () const
    {
      return (Codec::ID) mCtx->codec_id;
    }
    /**
     * Additional information about the codec (corresponds to the AVI FOURCC).
     */
    virtual uint32_t
    getTag () const
    {
      return mCtx->codec_tag;
    }

    /**
     * Extra binary data needed for initializing the decoder, codec-dependent.
     *
     * Must be allocated with av_malloc() and will be freed by
     * avcodec_parameters_free(). The allocated size of extradata must be at
     * least extradata_size + AV_INPUT_BUFFER_PADDING_SIZE, with the padding
     * bytes zeroed.
     */
    // TODO: Expose Extradata
    //uint8_t *extradata;
    /**
     * Size of the extradata content in bytes.
     */
    //int      extradata_size;
    /**
     * Get the Pixel Format, or PixelFormat.PIX_FMT_NONE if the parameter #getType()
     * is not MediaDescriptor.MEDIA_VIDEO.
     *
     * @return the pixel format.
     */
    virtual PixelFormat::Type
    getPixelFormat () const
    {
      if (getType () == MediaDescriptor::MEDIA_VIDEO)
        return (PixelFormat::Type) mCtx->format;
      else
        return PixelFormat::PIX_FMT_NONE;
    }
    /**
     * Sets the pixel format (it is up to the caller
     * to ensure that #getType() is MediaDescriptor.MEDIA_VIDEO. Otherwise
     * behaviour is undefined.
     *
     * @param format Pixel Format to set format to.
     * @see #getPixelFormat()
     */
    virtual void
    setPixelFormat (PixelFormat::Type format)
    {
      mCtx->format = format;
    }

    /**
     * Get the Audio Format, or AudioFormat.SAMPLE_FMT_NONE if the parameter #getType()
     * is not MediaDescriptor.MEDIA_AUDIO.
     *
     * @return the audio format.
     */
    virtual AudioFormat::Type
    getAudioFormat () const
    {
      if (getType () == MediaDescriptor::MEDIA_AUDIO)
        return (AudioFormat::Type) mCtx->format;
      else
        return AudioFormat::SAMPLE_FMT_NONE;
    }
    /**
     * Sets the audio format (it is up to the caller
     * to ensure that #getType() is MediaDescriptor.MEDIA_AUDIO. Otherwise
     * behaviour is undefined.
     *
     * @param format Audio Format to set format to.
     * @see #getAudioFormat()
     */
    virtual void
    setAudioFormat (AudioFormat::Type format)
    {
      mCtx->format = format;
    }

    /**
     * Get the average bit-rate (in bits per second) of encoded data.
     * @return the bit rate.
     */
    virtual int64_t
    getBitRate () const
    {
      return mCtx->bit_rate;
    }
    /**
     * Set the bit-rate (in bits per second) of encoded data.
     * @param bit_rate the bit rate.
     */
    virtual void
    setBitRate (int64_t bit_rate)
    {
      mCtx->bit_rate = bit_rate;
    }

    /**
     * Get the number of bits per sample in the codedwords.
     * <p>
     * This is basically the bitrate per sample. It is mandatory for a bunch of
     * formats to actually decode them. It's the number of bits for one sample in
     * the actual coded bitstream.
     * </p>
     * <p>
     * This could be for example 4 for ADPCM
     * For PCM formats this matches bits_per_raw_sample
     * </p>
     * <p>
     * Can be 0.
     * </p>
     */
    virtual int64_t
    getBitsPerCodedSample () const
    {
      return (int64_t) mCtx->bits_per_coded_sample;
    }
    /**
     * Set the bits per coded sample.
     * @param bitsPerCodedSample bits per coded sample.
     * @see #getBitsPerCodedSample()
     */
    virtual void
    setBitsPerCodedSample (int64_t bitsPerCodedSample)
    {
      mCtx->bits_per_coded_sample = (int) bitsPerCodedSample;
    }

    /**
     * This is the number of valid bits in each output sample.
     *         * <p>
     * If the sample format has more bits, the least significant bits are additional
     * padding bits, which are always 0. Use right shifts to reduce the sample
     * to its actual size. For example, audio formats with 24 bit samples will
     * have bits_per_raw_sample set to 24, and format set to AudioFormat.SAMPLE_FMT_S32.
     * To get the original sample use "(int32_t)sample >> 8"."
     * </p>
     * <p>
     * For ADPCM this might be 12 or 16 or similar
     * </p>
     * <p>
     * Can be 0.
     * </p>
     */
    virtual int64_t
    getBitsPerRawSample () const
    {
      return (int64_t) mCtx->bits_per_raw_sample;
    }

    /**
     * Sets the bits per raw sample.
     * @param bitsPerRawSample bits per raw sample.
     * @see #getBitsPerRawSample()
     */
    virtual void
    setBitsPerRawSample (int64_t bitsPerRawSample)
    {
      mCtx->bits_per_raw_sample = (int) bitsPerRawSample;
    }

    /**
     * Codec-specific bitstream restrictions that the stream conforms to.
     * @return the profile.
     */
    virtual int64_t
    getProfile () const
    {
      return (int64_t) mCtx->profile;
    }
    /**
     * @see getProfile()
     */
    virtual void
    setProfile (int64_t profile)
    {
      mCtx->profile = (int) profile;
    }
    /**
     * Codec-specific bitstream restriction level that the stream conforms to.
     * @return the level.
     */
    virtual int64_t
    getLevel () const
    {
      return (int64_t) mCtx->level;
    }
    /**
     * See #getLevel()
     */
    virtual void
    setLevel (int64_t level)
    {
      mCtx->level = level;
    }

    /**
     * Get the picture width in pixels. Video only.
     */
    virtual int32_t
    getWidth () const
    {
      return mCtx->width;
    }
    /**
     * @see #getWidth()
     */
    virtual void
    setWidth (int32_t width)
    {
      mCtx->width = width;
    }
    /**
     * Get the picture height in pixels. Video only.
     */
    virtual int32_t
    getHeight () const
    {
      return mCtx->height;
    }
    /**
     * @see #getHeight()
     */
    virtual void
    setHeight (int32_t height)
    {
      mCtx->height = height;
    }

    /**
     *  The aspect ratio (width / height) which a single pixel
     * should have when displayed. Video only.
     *
     * When the aspect ratio is unknown / undefined, the numerator should be
     * set to 0 (the denominator may have any value).
     */
    virtual Rational*
    getSampleAspectRatio () const
    {
      return Rational::make (mCtx->sample_aspect_ratio.num,
                             mCtx->sample_aspect_ratio.den);
    }

    /**
     * See #getSampleAspectRatio()
     */
    virtual void
    setSampleAspectRatio (Rational* newValue)
    {
      if (newValue)
      {
        mCtx->sample_aspect_ratio.num = newValue->getNumerator ();
        mCtx->sample_aspect_ratio.den = newValue->getDenominator ();
      }
      return;
    }

    /**
     * Sets the field order. Video only.
     */
    virtual PixelFormat::FieldOrder getFieldOrder() const {
      return (PixelFormat::FieldOrder) mCtx->field_order;
    }
    /**
     * @see getFieldOrder()
     */
    virtual void setFieldOrder(PixelFormat::FieldOrder order) {
      mCtx->field_order = (enum AVFieldOrder) order;
    }


    /**
     * Get the color range. Video only.
     */
    virtual PixelFormat::ColorRange getColorRange() const {
      return (PixelFormat::ColorRange) mCtx->color_range;
    }
    /**
     * @see #getColorRange()
     */
    virtual void setColorRange(PixelFormat::ColorRange range) {
      mCtx->color_range = (enum AVColorRange) range;
    }
    /**
     * Get the color primaries. Video only.
     */
    virtual PixelFormat::ColorPrimaries getColorPrimaries() const {
      return (PixelFormat::ColorPrimaries) mCtx->color_primaries;
    }
    /**
     * @see #getColorPrimaries()
     */
    virtual void setColorPrimaries(PixelFormat::ColorPrimaries primaries) {
      mCtx->color_primaries = (enum AVColorPrimaries) primaries;
    }

    /**
     * Get the color transfer characteristic. Video only.
     */
    virtual PixelFormat::ColorTransferCharacteristic getColorTransferCharacteristic() const {
      return (PixelFormat::ColorTransferCharacteristic) mCtx->color_trc;
    }
    /**
     * See #getColorTransferCharacteristic()
     */
    virtual void setColorTransferCharacteristic(PixelFormat::ColorTransferCharacteristic trc) {
      mCtx->color_trc = (enum AVColorTransferCharacteristic) trc;
    }

    /**
     * Get the color space. Video only.
     */
    virtual PixelFormat::ColorSpace getColorSpace() const {
      return (PixelFormat::ColorSpace) mCtx->color_space;
    }
    /**
     * @see #getColorSpace()
     */
    virtual void setColorSpace(PixelFormat::ColorSpace space) {
      mCtx->color_space = (enum AVColorSpace) space;
    }

    /**
     * Get the chroma location. Video only.
     * @see PixelFormat.ChromaLocation
     */
    virtual PixelFormat::ChromaLocation getChromaLocation() const {
      return (PixelFormat::ChromaLocation) mCtx->chroma_location;
    }
    /**
     * @see #getChromaLocation()
     */
    virtual void setChromaLocation(PixelFormat::ChromaLocation loc) {
      mCtx->chroma_location = (enum AVChromaLocation) loc;
    }

    /**
     * Number of delayed frames. Video only.
     */
    virtual int32_t getVideoDelay() const {
      return (int32_t) mCtx->video_delay;
    }
    /**
     * @see #getVideoDelay()
     */
    virtual void setVideoDelay(int32_t delay) {
      mCtx->video_delay = (int) delay;
    }

    /**
     * The channel layout bitmask. Audio only. May be 0 if the channel layout is
     * unknown or unspecified, otherwise the number of bits set must be equal to
     * the channels field.
     */
    virtual uint64_t getChannelLayout() const {
      return mCtx->channel_layout;
    }
    /**
     * @see #getChannelLayout()
     */
    virtual void setChannelLayout(uint64_t layout) {
      mCtx->channel_layout = layout;
    }

    /**
     * The number of audio channels. Audio only.
     */
    virtual int32_t getChannels() const {
      return mCtx->channels;
    }
    /**
     * @see #getChannels()
     */
    virtual void setChannels(int32_t channels) {
      mCtx->channels = (int) channels;
    }

    /**
     * The number of audio samples per second. Audio only.
     */
    virtual int32_t getSampleRate() const {
      return mCtx->sample_rate;
    }
    /**
     * @see #getSampleRate()
     */
    virtual void setSampleRate(int32_t rate) {
      mCtx->sample_rate = (int) rate;
    }

    /**
     * The number of bytes per coded audio frame, required by some
     * formats. Audio only.
     *
     * Corresponds to nBlockAlign in WAVEFORMATEX.
     */
    virtual int32_t getBlockAlign() const {
      return mCtx->block_align;
    }
    virtual void setBlockAlign(int32_t align) {
      mCtx->block_align = (int) align;
    }

    /**
     * Audio frame size, if known. Audio only. Required by some formats to be static.
     */
    virtual int32_t getFrameSize() const { return mCtx->frame_size; }
    /**
     * @see #getFrameSize()
     */
    virtual void setFrameSize(int32_t size) { mCtx->frame_size = (int) size; }

    /**
     * The amount of padding (in samples) inserted by the encoder at
     * the beginning of the audio. Audio only. I.e. this number of leading decoded samples
     * must be discarded by the caller to get the original audio without leading
     * padding.
     */
    virtual int32_t getInitialPadding() const { return mCtx->initial_padding; }
    /**
     * @see #getInitialPadding()
     */
    virtual void setInitialPadding(int32_t pad) { mCtx->initial_padding = (int) pad; }

    /**
     * The amount of padding (in samples) appended by the encoder to
     * the end of the audio. Audio only. I.e. this number of decoded samples must be
     * discarded by the caller from the end of the stream to get the original
     * audio without any trailing padding.
     */
    virtual int32_t getTrailingPadding() const { return mCtx->trailing_padding; }
    /**
     * @see #getTrailingPadding()
     */
    virtual void setTrailingPadding(int32_t pad) { mCtx->trailing_padding = (int) pad; }

    /**
     * Number of samples to skip after a discontinuity. Audio only.
     */
    virtual int32_t getSeekPreroll() const { return mCtx->seek_preroll; }
    /**
     * @see #getSeekPreroll()
     */
    virtual void setSeekPreroll(int32_t preroll) { mCtx->seek_preroll = (int) preroll; }
  };

#ifndef SWIG
  /** Some marker "pure virtual" classes. */

  /**
   * This object can have MediaParameters set.
   */
  class VS_API_HUMBLEVIDEO MediaParametersSettable {
    /**
     * Set the parameters.
     */
    void setParameters(MediaParameters*);
  };

  class VS_API_HUMBLEVIDEO MediaParametersGettable {
    /**
     * Get the parameters.
     */
    MediaParameters* getParameters();
  };
#endif


}}}

#endif /* SRC_MAIN_GNU_SRC_IO_HUMBLE_VIDEO_MEDIAPARAMETERS_H_ */
