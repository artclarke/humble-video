package io.humble.video.javaxsound;

import java.nio.ByteBuffer;

import io.humble.ferry.Buffer;
import io.humble.video.AudioChannel;
import io.humble.video.AudioFormat;
import io.humble.video.MediaAudio;
import io.humble.video.MediaAudioResampler;

class StereoS16AudioConverter implements MediaAudioConverter {
  
  private final int mJavaSampleRate;
  private final int mJavaChannels;
  private final AudioChannel.Layout mJavaLayout;
  private final AudioFormat.Type mJavaFormat;
  
  private final int mMediaSampleRate;
  private final int mMediaChannels;
  private final AudioChannel.Layout mMediaLayout;
  private final AudioFormat.Type mMediaFormat;
  
  private final MediaAudioResampler mMediaAudioToJavaSoundResampler;

  private MediaAudio mResampledAudio;

  
  public StereoS16AudioConverter(int sampleRate, AudioChannel.Layout layout, AudioFormat.Type format) {
    if (sampleRate < 0)
      throw new IllegalArgumentException("sample rate must be > 0");
    if (layout == AudioChannel.Layout.CH_LAYOUT_UNKNOWN)
      throw new IllegalArgumentException("channel layout must be known");
    
    mJavaSampleRate=22050;
    mJavaFormat = AudioFormat.Type.SAMPLE_FMT_S16;
    mJavaLayout = AudioChannel.Layout.CH_LAYOUT_STEREO;
    mJavaChannels = AudioChannel.getNumChannelsInLayout(mJavaLayout);

    mMediaSampleRate = sampleRate;
    mMediaLayout = layout;
    mMediaChannels = AudioChannel.getNumChannelsInLayout(layout);
    mMediaFormat = format;
    
    if (willResample()) {
      mMediaAudioToJavaSoundResampler = MediaAudioResampler.make(mJavaLayout,
          mJavaSampleRate, mJavaFormat, mMediaLayout, mMediaSampleRate, mMediaFormat);
      mMediaAudioToJavaSoundResampler.open();
    }
    else {
      mMediaAudioToJavaSoundResampler = null;
    }
  }
 
  /**
   * @return the javaFormat
   */
  @Override
  public javax.sound.sampled.AudioFormat getJavaFormat() {
    return new javax.sound.sampled.AudioFormat(mJavaSampleRate,
        AudioFormat.getBytesPerSample(mJavaFormat)*8, mJavaChannels, true, false);
  }
  /**
   * @return the mediaSampleRate
   */
  @Override
  public int getMediaSampleRate() {
    return mMediaSampleRate;
  }
  /**
   * @return the mediaChannels
   */
  @Override
  public int getMediaChannels() {
    return mMediaChannels;
  }
  /**
   * @return the mediaLayout
   */
  @Override
  public AudioChannel.Layout getMediaLayout() {
    return mMediaLayout;
  }
  /**
   * @return the mediaFormat
   */
  @Override
  public AudioFormat.Type getMediaFormat() {
    return mMediaFormat;
  }
  private boolean willResample() {
    return !(
        mMediaSampleRate == mJavaSampleRate &&
        mMediaChannels == mJavaChannels &&
        mMediaFormat == mJavaFormat
        );
  }

  private void validateMediaAudio(MediaAudio audio) {
    if (audio == null)
      throw new IllegalArgumentException("must pass in audio");
    if (audio.getSampleRate() != mMediaSampleRate)
      throw new IllegalArgumentException("input sample rate does not match value converter expected");
    if (audio.getChannelLayout() != mMediaLayout)
      throw new IllegalArgumentException("input channel layout does not match value converter expected");
    if (audio.getFormat() != mMediaFormat)
      throw new IllegalArgumentException("input sample format does not match value converter expected");
    if (!audio.isComplete())
      throw new IllegalArgumentException("input audio is not complete");
    
  }
  @Override
  public ByteBuffer toJavaAudio(ByteBuffer output, MediaAudio input) {
    validateMediaAudio(input);
    final MediaAudio audio;
    final int outputNumSamples;
    if (willResample()) {
      outputNumSamples = mMediaAudioToJavaSoundResampler.getNumResampledSamples(input.getNumSamples());
      if (mResampledAudio == null ||
          mResampledAudio.getMaxNumSamples() < outputNumSamples) {
        if (mResampledAudio != null) mResampledAudio.delete();
        mResampledAudio = MediaAudio.make(outputNumSamples,
            mJavaSampleRate, mJavaChannels, mJavaLayout, mJavaFormat);
      }
      audio = mResampledAudio;
    } else {
      outputNumSamples = input.getNumSamples();
      audio = input;
    }
    int size = AudioFormat.getBufferSizeNeeded(outputNumSamples, audio.getChannels(), audio.getFormat());
    if (output == null) {
      output = ByteBuffer.allocate(size);
    } else {
      if (output.capacity() < size)
        throw new RuntimeException("output bytes not large enough to hold data");
    }
    if (willResample()) {
      mMediaAudioToJavaSoundResampler.resample(audio, input);
    }
    // now, copy the resulting data into the bytes.
    
    // we force audio to be packed, so only one plane.
    final Buffer buffer = audio.getData(0);
    int bufferSize = audio.getDataPlaneSize(0);
    byte[] bytes = output.array();
    buffer.get(0, bytes, 0, bufferSize);
    output.limit(size);
    output.position(0);
    buffer.delete();
    
    return output;
  }


}
