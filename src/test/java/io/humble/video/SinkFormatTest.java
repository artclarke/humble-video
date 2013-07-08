package io.humble.video;

import java.util.Collection;
import java.util.List;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import junit.framework.TestCase;

public class SinkFormatTest extends TestCase {
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  public SinkFormatTest() {
    log.trace("Remove a warning; ignore");
  }

  @Test
  public void testSinkFormat() {
    SinkFormat f = SinkFormat.guessFormat("flv", null, null);
    assertNotNull(f);
    
    f = SinkFormat.guessFormat("mp4", null, null);
    assertNotNull(f);
    
    assertEquals(Codec.ID.CODEC_ID_AAC, f.getDefaultAudioCodecId());
    assertEquals(Codec.ID.CODEC_ID_H264, f.getDefaultVideoCodecId());
    assertEquals(Codec.ID.CODEC_ID_NONE, f.getDefaultSubtitleCodecId());
    
    assertEquals(Codec.ID.CODEC_ID_AAC, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_AUDIO));
    assertEquals(Codec.ID.CODEC_ID_H264, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_VIDEO));
    assertEquals(Codec.ID.CODEC_ID_NONE, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_SUBTITLE));
  }
  
  @Test
  public void testGetCodecs() {
    final SinkFormat f = SinkFormat.guessFormat("mp4", null, null);
    assertNotNull(f);
    
    final List<Codec.ID> l = f.getSupportedCodecs();
    assertEquals(20, l.size());
  }

  @Test
  public void testInstallation() {
    Collection<SinkFormat> formats = SinkFormat.getFormats();
    // A well configured FFmpeg will have over 120 output formats
    assertTrue(formats.size() > 100);
    for (SinkFormat  f : formats)
    {
      assertNotNull(f);
//      System.out.println(f);
    }
  }
}
