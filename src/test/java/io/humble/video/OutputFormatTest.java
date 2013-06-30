package io.humble.video;

import java.util.List;

import io.humble.video.Codec.Id;
import io.humble.video.MediaDescriptor;

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
    
    assertEquals(Codec.Id.ID_AAC, f.getDefaultAudioCodecId());
    assertEquals(Codec.Id.ID_H264, f.getDefaultVideoCodecId());
    assertEquals(Codec.Id.ID_NONE, f.getDefaultSubtitleCodecId());
    
    assertEquals(Codec.Id.ID_AAC, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_AUDIO));
    assertEquals(Codec.Id.ID_H264, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_VIDEO));
    assertEquals(Codec.Id.ID_NONE, f.guessCodec("mov", null, null, MediaDescriptor.Type.MEDIA_SUBTITLE));
  }
  
  @Test
  public void testGetCodecs() {
    final OutputFormat f = OutputFormat.guessFormat("mp4", null, null);
    assertNotNull(f);
    
    final List<Id> l = f.getOutputCodecsSupported();
    assertEquals(20, l.size());
  }

}
