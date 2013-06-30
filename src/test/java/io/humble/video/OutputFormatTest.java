package io.humble.video;

import java.util.Collection;
import java.util.List;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import junit.framework.TestCase;

public class OutputFormatTest extends TestCase {
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  public OutputFormatTest() {
    log.trace("Remove a warning; ignore");
  }

  @Test
  public void testOutputFormat() {
    OutputFormat f = OutputFormat.guessFormat("flv", null, null);
    assertNotNull(f);
    
    f = OutputFormat.guessFormat("mp4", null, null);
    assertNotNull(f);
    
    assertEquals(Codec.ID.ID_AAC, f.getDefaultAudioCodecId());
    assertEquals(Codec.ID.ID_H264, f.getDefaultVideoCodecId());
    assertEquals(Codec.ID.ID_NONE, f.getDefaultSubtitleCodecId());
    
    assertEquals(Codec.ID.ID_AAC, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_AUDIO));
    assertEquals(Codec.ID.ID_H264, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_VIDEO));
    assertEquals(Codec.ID.ID_NONE, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_SUBTITLE));
  }
  
  @Test
  public void testGetCodecs() {
    final OutputFormat f = OutputFormat.guessFormat("mp4", null, null);
    assertNotNull(f);
    
    final List<Codec.ID> l = f.getSupportedCodecs();
    assertEquals(20, l.size());
  }

  @Test
  public void testInstallation() {
    Collection<OutputFormat> formats = OutputFormat.getFormats();
    // A well configured FFmpeg will have over 120 output formats
    assertTrue(formats.size() > 100);
    for (OutputFormat  f : formats)
    {
      assertNotNull(f);
//      System.out.println(f);
    }
  }
}
