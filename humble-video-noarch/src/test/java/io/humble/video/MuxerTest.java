package io.humble.video;

import static org.junit.Assert.*;

import java.io.File;
import java.net.URL;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class MuxerTest {

  @Before
  public void setUp() {
  }
  
  @After
  public void tearDown() throws Exception {
  }

  /**
   * Test remuxing from mp4 to mov
   */
  @Test
  public void testRemuxing() throws Exception {
    final URL s = this.getClass().getResource("/ucl_h264_aac.mp4");
    final String f = new File(s.getPath()).getPath();
    final String o = this.getClass().getName()+"testRemuxing.mov";
    
    Demuxer demuxer = Demuxer.make();
    demuxer.open(f, null, false, true, null, null);
    
    // open output
    Muxer muxer = Muxer.make(o, null, null);
    int n = demuxer.getNumStreams();
    for(int i = 0; i < n; i++) {
      DemuxerStream ds = demuxer.getStream(i);
      Decoder d = ds.getDecoder();
      muxer.addNewStream(d);
    }
    muxer.open(null, null);
    MediaPacket packet = MediaPacket.make();
    while(demuxer.read(packet) >= 0) {
      muxer.write(packet, false);
    }
    muxer.close();
    
    // now, let's read in the file we wrote and confirm correct # of packets and size.
    Demuxer demuxer2 = Demuxer.make();
    demuxer2.open(o,  null, false, true, null, null);
    int numPackets = 0;
    while(demuxer2.read(packet) >= 0)
      ++numPackets;
    
    assertEquals(demuxer2.getFileSize(), demuxer.getFileSize(), 1024);
    assertEquals(1162, numPackets);

    demuxer.close();
    demuxer2.close();
  }

}
