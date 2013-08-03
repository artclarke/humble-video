package io.humble.video.javaxsound;

import io.humble.video.AudioChannel.Layout;
import io.humble.video.AudioFormat.Type;
import io.humble.video.MediaAudio;

/**
 * A factory class for {@link MediaAudioConverter} interfaces.
 * @author aclarke
 *
 */
public class MediaAudioConverterFactory {

  protected MediaAudioConverterFactory() {
  }
  
  /**
   * Create a converter.
   * @param description A descriptor string for the converter.
   * @param protoAudio A MediaAudio object to copy the input audio parameters from.
   * @return
   */
  public static MediaAudioConverter createConverter(String description, MediaAudio protoAudio)
  {
    return createConverter(description, 
        protoAudio.getSampleRate(), protoAudio.getChannelLayout(), protoAudio.getFormat());
  }

  public static final String DEFAULT_JAVA_AUDIO="HUMBLE-STEREO-S16-22050";
  
  /**
   * Create a converter.
   * @param description A descriptor string for the converter.
   * @param sampleRate Sample rate of MediaAudio objects that will be converted to/from.
   * @param layout Channel layout of MediaAudio objects that will be converted to/from.
   * @param format Sample format of MediaAudio objets that will be converted to/from.
   * @return A converter to use
   */
  public static MediaAudioConverter createConverter(String description,
      int sampleRate, Layout layout, Type format) {
    if (description != DEFAULT_JAVA_AUDIO)
      throw new RuntimeException("Unsupported converter type");
    return new StereoS16AudioConverter(sampleRate, layout, format);
  }

}
