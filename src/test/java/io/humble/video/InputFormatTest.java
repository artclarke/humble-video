package io.humble.video;

import static org.junit.Assert.*;
import io.humble.video.Codec.Id;

import java.util.List;

import org.junit.Test;

public class InputFormatTest {

  @Test
  public void testInputFormat() {
    final InputFormat f = InputFormat.findFormat("aiff");
    assertNotNull(f);
    
    assertEquals("aiff", f.getName());
    assertEquals("Audio IFF", f.getLongName());
    List<Id> l = f.getSupportedCodecs();
    assertEquals(17, l.size());
  }

}
