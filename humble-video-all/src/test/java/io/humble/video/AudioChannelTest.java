package io.humble.video;

import static org.junit.Assert.*;

import org.junit.Test;

public class AudioChannelTest {

  @Test
  public void testGetLayoutDescription() {
    AudioChannel.Layout layout = AudioChannel.Layout.CH_LAYOUT_7POINT1_WIDE;
    assertEquals(8, AudioChannel.getNumChannelsInLayout(layout));
  }

}
