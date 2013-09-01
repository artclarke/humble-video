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
 * MediaAudioResampler.h
 *
 *  Created on: Jul 30, 2013
 *      Author: aclarke
 */

#ifndef MEDIAAUDIORESAMPLER_H_
#define MEDIAAUDIORESAMPLER_H_

#include <io/humble/video/Configurable.h>
#include <io/humble/video/MediaAudio.h>

namespace io {
namespace humble {
namespace video {

/**
 * A MediaAudioResampler object resamples MediaAudio objects from
 * one format/sample-rate/channel-layout to another.
 */
class VS_API_HUMBLEVIDEO MediaAudioResampler : public io::humble::video::Configurable
{
public:
  /**
   * Create a new MediaAudioResampler.
   */
  static MediaAudioResampler*
  make(
      AudioChannel::Layout outLayout,
      int32_t outSampleRate,
      AudioFormat::Type outFormat,
      AudioChannel::Layout inLayout,
      int32_t inSampleRate,
      AudioFormat::Type inFormat
      );

  /**
   * Get output channel layout.
   */
  virtual AudioChannel::Layout getOutputLayout();
  /**
   * Get input channel layout.
   */
  virtual AudioChannel::Layout getInputLayout();
  /**
   * Get output sample rate.
   */
  virtual int32_t getOutputSampleRate();
  /**
   * Get input sample rate.
   */
  virtual int32_t getInputSampleRate();
  /**
   * Get output audio format.
   */
  virtual AudioFormat::Type getOutputFormat();
  /**
   * Get input audio format.
   */
  virtual AudioFormat::Type getInputFormat();
  /**
   * Get number of input channels (derived from #getInputLayout()).
   */
  virtual int32_t getInputChannels();
  /**
   * Get number of output channels (derived from #getOutputLayout()).
   */
  virtual int32_t getOutputChannels();

  /**
   * Opens the resampler so it can be ready for resampling.
   * You should NOT set options after you open this object.
   */
  virtual void open();

  /**
   * Convert audio.
   *
   * in can be set to null to flush the last few samples out at the
   * end.
   *
   * If more input is provided than output space then the input will be buffered.
   * You can avoid this buffering by providing more output space than input.
   * Conversion will run directly without copying whenever possible.
   *
   * @param out       output audio object. caller is responsible for making the correct size.
   * @param in        input audio.
   *
   * @return number of samples output per channel.
   * @throws RuntimeError if we get an error or InvalidArgument if the attributes of
   *   in or out do not match what this resampler expected.
   */
  virtual int32_t resample(MediaAudio* out, MediaAudio* in);

  /**
   * Convert the next timestamp from input to output
   * timestamps are in 1/(in_sample_rate * out_sample_rate) units.
   *
   * @note There are 2 slightly differently behaving modes.
   *       First is when automatic timestamp compensation is not used, (min_compensation >= FLT_MAX)
   *              in this case timestamps will be passed through with delays compensated
   *       Second is when automatic timestamp compensation is used, (min_compensation < FLT_MAX)
   *              in this case the output timestamps will match output sample numbers
   *
   * @param pts   timestamp for the next input sample, INT64_MIN if unknown
   * @return the output timestamp for the next output sample
   */
  virtual int64_t getNextPts(int64_t pts);

  /**
   * Activate resampling compensation.
   */
  virtual void setCompensation(int32_t sample_delta, int32_t compensation_distance);

  /**
   * Set a customized input channel mapping.
   *
   * @param channel_map customized input channel mapping (array of channel
   *                    indexes, -1 for a muted channel)
   * @return AVERROR error code in case of failure.
   */
  //virtual void setChannelMapping(const int *channel_map);

  /**
   * Set a customized remix matrix.
   *
   * @param matrix  remix coefficients; matrix[i + stride * o] is
   *                the weight of input channel i in output channel o
   * @param stride  offset between lines of the matrix
   * @return  AVERROR error code in case of failure.
   */
  //virtual void setMatrix(const double *matrix, int stride);

  /**
   * Drops the specified number of output samples.
   * @return # of samples dropped.
   */
  virtual int32_t dropOutput(int32_t count);

  /**
   * Injects the specified number of silence samples.
   * @return # of samples injected.
   */
  virtual int32_t injectSilence(int32_t count);

  /**
   * Gets the delay the next input sample will experience relative to the next output sample.
   *
   * The resampler can buffer data if more input has been provided than available
   * output space, also converting between sample rates needs a delay.
   * This function returns the sum of all such delays.
   * The exact delay is not necessarily an integer value in either input or
   * output sample rate. Especially when downsampling by a large value, the
   * output sample rate may be a poor choice to represent the delay, similarly
   * for upsampling and the input sample rate.
   *
   * @param base  timebase in which the returned delay will be
   *              if its set to 1 the returned delay is in seconds
   *              if its set to 1000 the returned delay is in milli seconds
   *              if its set to the input sample rate then the returned delay is in input samples
   *              if its set to the output sample rate then the returned delay is in output samples
   *              an exact rounding free delay can be found by using LCM(in_sample_rate, out_sample_rate)
   * @returns     the delay in 1/base units.
   */
  virtual int64_t getDelay(int64_t base);

  /**
   * Returns the number of resampled samples (rounded up) that would
   * be required when resampling a given number of samples.
   * That was a mouthful, yes? So here's the way to think of this. If your input
   * audio is at 48000 hz, and you pass in 0.5 seconds of audio, that's 24,000 input samples.
   * But if you're resampling to 22050 hz, then 0.5 seconds of audio is 11,025 output samples.
   * So <code>getNumResampledSamples(24000)</code> would return <code>11025</code> if the input
   * sample rate was 48,000 and hte output was 22,050.
   */
  virtual int32_t getNumResampledSamples(int32_t numSamples);

  /**
   * Get the timebase used when outputting time stamps for audio.
   *
   * Defaults to 1 / (the lowest common multiple of getInputSampleRate()
   *   and getOutputSampleRate()) in order to ensure that no rounding
   *   of time stamps occur.
   *
   * For example, if the input sample rate is 22050 and the output sample
   * rate is 44100, then the output time base will be (1/44100). But if the
   * input sample rate is 48000 and the output sample rate is 22050, then
   * the output time base will be (1/lcm(48000,22050)) which will be 1/7056000
   * (trust me). This is done so that timestamp values do not get rounded (and
   * therefore introduce drift).
   */
  virtual Rational* getTimeBase() { return mTimeBase.get(); }

  /**
   * Set the timebase to use for timestamps on output audio.
   *
   * @throws InvalidArgument if null.
   */
  virtual void setTimeBase(Rational* rational);

  typedef enum State {
    STATE_INITED,
    STATE_OPENED,
    STATE_ERROR
  } State;
  virtual State getState() { return mState; }
protected:
  void* getCtx() { return mCtx; }
  MediaAudioResampler();
  virtual
  ~MediaAudioResampler();
private:
  SwrContext* mCtx;
  State mState;
  io::humble::ferry::RefPointer<Rational> mTimeBase;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIAAUDIORESAMPLER_H_ */
