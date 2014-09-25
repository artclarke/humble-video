package io.humble.video.javaxsound;


import java.nio.ByteBuffer;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Creates a connection to the Java Sound system, and allows
 * convenient playback of the output form a {@link MediaAudioConverter}.
 * <p>
 * Caller must call {@link #dispose()} when done to drain the line and
 * release resources.
 * </p>
 * <p>
 * This class has no Humble dependencies per-see, but does
 * playback audio that comes out of a converter.
 * </p>
 * @author aclarke
 *
 */
public class AudioFrame {

  static private final Logger log = LoggerFactory.getLogger(AudioFrame.class);

  private SourceDataLine mLine;
  
  /**
   * Create an audio frame with the default assumed format.
   * @return
   */
  public static AudioFrame make() {
    final AudioFormat audioFormat = new AudioFormat(22050, 
        2*8,
        2,
        true,
        false);
    return make(audioFormat);
  }
  
  /**
   * Get a connection to the speaker, if available.
   * @param audioFormat The java audio format that we will use.
   * @return A new audio format, or null if we cannot create one.
   */
  public static AudioFrame make(final AudioFormat audioFormat) {
    try {
      return new AudioFrame(audioFormat);
    } catch (LineUnavailableException e) {
      log.error("Could not get audio data line: {}", e.getMessage());
      return null;
    }
  }
  
  /**
   * Get the audio format being used.
   * @return
   */
  public AudioFormat getFormat() {
    return mLine.getFormat();
  }
  
  private AudioFrame(final AudioFormat audioFormat) throws LineUnavailableException{
    DataLine.Info info = new DataLine.Info(SourceDataLine.class, audioFormat);
    mLine = (SourceDataLine) AudioSystem.getLine(info);
    /**
     * if that succeeded, try opening the line.
     */
    mLine.open(audioFormat);
    /**
     * And if that succeed, start the line.
     */
    mLine.start();
  }

  /**
   * Play the given bytes (in {@link #getFormat()} format on the device.
   * @param rawAudio a byte buffer where the data from {@link ByteBuffer#position()}
   *   to {@link ByteBuffer#limit()} will be written.
   */
  public void play(ByteBuffer rawAudio)
  {
    byte[] data = rawAudio.array();
    mLine.write(data, rawAudio.position(), rawAudio.limit());
  }

  /**
   * Drain the line and then close it.
   */
  public void dispose()
  {
    if (mLine != null)
    {
      /*
       * Wait for the line to finish playing
       */
      mLine.drain();
      /*
       * Close the line.
       */
      mLine.close();
      mLine=null;
    }
  }

}
