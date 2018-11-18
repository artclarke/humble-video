package io.humble.video;

import java.util.Collection;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import junit.framework.TestCase;

public class MuxerFormatTest extends TestCase {
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  public MuxerFormatTest() {
    log.trace("Remove a warning; ignore");
  }

  @Test
  public void testSinkFormat() {
    MuxerFormat f = MuxerFormat.guessFormat("flv", null, null);
    assertNotNull(f);
    
    f = MuxerFormat.guessFormat("mp4", null, null);
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
    final MuxerFormat f = MuxerFormat.guessFormat("mp4", null, null);
    assertNotNull(f);
    
    final Collection<Codec.ID> l = f.getSupportedCodecs();
    assertTrue(l.size() > 21);
  }

  @Test
  public void testInstallation() {
    Collection<MuxerFormat> formats = MuxerFormat.getFormats();
    // A well configured FFmpeg will have over 120 output formats
    assertTrue(formats.size() > 100);
    for (MuxerFormat  f : formats)
    {
      assertNotNull(f);
//      System.out.println(f);
    }
  }
}
