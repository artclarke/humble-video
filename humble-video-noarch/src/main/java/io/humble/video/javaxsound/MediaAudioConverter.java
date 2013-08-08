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

  public AudioFormat.Type getMediaFormat();

  public AudioChannel.Layout getMediaLayout();

  public int getMediaChannels();

  public int getMediaSampleRate();

  /**
   * @return the java format of the audio (byte[]) that will be converted to/from.
   */
  public javax.sound.sampled.AudioFormat getJavaFormat();

}
