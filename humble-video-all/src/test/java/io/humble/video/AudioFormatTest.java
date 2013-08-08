package io.humble.video;

import static org.junit.Assert.*;

import org.junit.Test;

public class AudioFormatTest {

  @Test
  public void testGetName() {
    String format = AudioFormat.getName(AudioFormat.Type.SAMPLE_FMT_DBLP);
    assertEquals("dblp", format);
  }

  @Test
  public void testGetSampleFormat() {
    AudioFormat.Type format = AudioFormat.getFormat("dblp");
    assertEquals(AudioFormat.Type.SAMPLE_FMT_DBLP, format);
  }

}
