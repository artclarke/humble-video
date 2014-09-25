package io.humble.video.javaxsound;

import java.nio.ByteBuffer;

import io.humble.video.AudioChannel;
import io.humble.video.AudioFormat;
import io.humble.video.MediaAudio;

/**
 * Converts from Humble {@link MediaAudio} to byte arrays that can be used
 * in Java's sound system and back.
 * @author aclarke
 *
 */
public interface MediaAudioConverter {

  /**
   * Converts the given {@link MediaAudio} into a byte array
   * that can be played back on Java's sound APIs
   * @param output The array to write data to, or null if you want the converter to allocate a new array for output.
   * @param input The input media to convert.
   * @return The output audio.
   */
  public ByteBuffer toJavaAudio(ByteBuffer output, MediaAudio input);

  /**
   * The Humble {@link AudioFormat} this converter converts from.
   * @return The {@link AudioFormat}.
   */
  public AudioFormat.Type getMediaFormat();

  /**
   * The Humble {@link AudioChannel.Layout} this converter converts form.
   * 
   * @return The {@link AudioChannel.Layout}.
   */
  public AudioChannel.Layout getMediaLayout();

  /**
   * The number of channels this converter assumes input audio has.
   * @return The number of channels.
   */
  public int getMediaChannels();

  /**
   * The sample rate this convert assumes input audio has.
   * @return The sample rate.
   */
  public int getMediaSampleRate();

  /**
   * @return the java format of the audio (byte[]) that will be converted to/from.
   */
  public javax.sound.sampled.AudioFormat getJavaFormat();

}
