package io.humble.video;

import static org.junit.Assert.*;

import java.util.Collection;
import java.util.List;

import org.junit.Test;

public class SourceFormatTest {

  @Test
  public void testSourceFormat() {
    final SourceFormat f = SourceFormat.findFormat("aiff");
    assertNotNull(f);
    
    assertEquals("aiff", f.getName());
    assertEquals("Audio IFF", f.getLongName());
    List<Codec.ID> l = f.getSupportedCodecs();
    assertTrue(17 < l.size());
  }
  
  @Test
  public void testInstallation() {
    Collection<SourceFormat> formats = SourceFormat.getFormats();
    // A well configured FFmpeg will have over 180 formats
    assertTrue(formats.size() > 100);
    for (SourceFormat  f : formats)
    {
      assertNotNull(f);
//      System.out.println(f);
    }
  }

}
