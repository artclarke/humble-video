package io.humble.video;

import static org.junit.Assert.*;

import java.util.Collection;
import java.util.List;

import org.junit.Test;

public class InputFormatTest {

  @Test
  public void testInputFormat() {
    final InputFormat f = InputFormat.findFormat("aiff");
    assertNotNull(f);
    
    assertEquals("aiff", f.getName());
    assertEquals("Audio IFF", f.getLongName());
    List<Codec.ID> l = f.getSupportedCodecs();
    assertEquals(17, l.size());
  }
  
  @Test
  public void testInstallation() {
    Collection<InputFormat> formats = InputFormat.getFormats();
    // A well configured FFmpeg will have over 180 formats
    assertTrue(formats.size() > 100);
    for (InputFormat  f : formats)
    {
      assertNotNull(f);
//      System.out.println(f);
    }
  }

}
