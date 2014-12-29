package io.humble.video;

import static org.junit.Assert.*;

import java.util.Collection;

import org.junit.Test;

public class DemuxerFormatTest {

  @Test
  public void testSourceFormat() {
    final DemuxerFormat f = DemuxerFormat.findFormat("aiff");
    assertNotNull(f);
    
    assertEquals("aiff", f.getName());
    assertEquals("Audio IFF", f.getLongName());
    Collection<Codec.ID> l = f.getSupportedCodecs();
    assertTrue(17 < l.size());
  }
  
  @Test
  public void testInstallation() {
    Collection<DemuxerFormat> formats = DemuxerFormat.getFormats();
    // A well configured FFmpeg will have over 180 formats
    assertTrue(formats.size() > 100);
    for (DemuxerFormat  f : formats)
    {
      assertNotNull(f);
//      System.out.println(f);
    }
  }

}
