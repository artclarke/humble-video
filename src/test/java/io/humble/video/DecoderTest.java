package io.humble.video;

import io.humble.video.awt.Converter;
import io.humble.video.awt.ConverterFactory;
import io.humble.video.awt.ImageFrame;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URL;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class DecoderTest {

  private Source source;
  
  @Before
  public void setUp() throws Exception {
    final URL s = this.getClass().getResource("/testfile.flv");
    final String f = s.getPath();
    source = Source.make();
    source.open(f, null, false, true, null, null);

  }

  @After
  public void tearDown() throws Exception {
    source.close();
    source.delete();
  }

  @Test
  public void testDecodeAudio() throws InterruptedException, IOException {
    int audioStream = 1;
    SourceStream stream = source.getSourceStream(audioStream);
    Decoder decoder = stream.getDecoder();
    
    decoder.open(null, null);
    
    MediaPacket packet = MediaPacket.make();
    MediaAudio audio = MediaAudio.make(
        decoder.getFrameSize(),
        decoder.getSampleRate(),
        decoder.getChannels(),
        decoder.getChannelLayout(),
        decoder.getSampleFormat());
//    long startTime = Global.NO_PTS;
    
//    openWindow();
    
    while(source.read(packet)>=0) {
     if (packet.isComplete() && packet.getStreamIndex()==audioStream) {
       // let's try decoding
       int byteOffset = 0;
       int bytesRead = 0;
       do {
         bytesRead = decoder.decodeAudio(audio, packet, byteOffset);
//         startTime = displayPicture(startTime, picture);
         byteOffset += bytesRead;
       } while (byteOffset < packet.getSize());
     }
     // now, flush the decoder
     int bytesRead = 0;
     do {
       bytesRead = decoder.decodeAudio(audio, null, 0);
//       startTime = displayPicture(startTime, picture);
     } while (bytesRead > 0 && audio.isComplete());
    }
//    closeWindow();
  }

  @Test
  public void testDecodeVideo() throws InterruptedException, IOException {
    int videoStream = 0;
    SourceStream stream = source.getSourceStream(videoStream);
    Decoder decoder = stream.getDecoder();
    
    decoder.open(null, null);
    
    MediaPacket packet = MediaPacket.make();
    MediaPicture picture = MediaPicture.make(
        decoder.getWidth(),
        decoder.getHeight(),
        decoder.getPixelFormat()
        );
    final Converter converter = ConverterFactory.createConverter(ConverterFactory.HUMBLE_BGR_24, picture);
    BufferedImage image = null;

    long startTime = Global.NO_PTS;
    
    final ImageFrame window = ImageFrame.make();
    
    while(source.read(packet)>=0) {
     if (packet.isComplete() && packet.getStreamIndex()==videoStream) {
       // let's try decoding
       int byteOffset = 0;
       int bytesRead = 0;
       do {
         bytesRead = decoder.decodeVideo(picture, packet, byteOffset);
         if (picture.isComplete()) {
           image = converter.toImage(image, picture);
           startTime = displayPicture(startTime, picture.getTimeStamp(), window, image);
         }
         byteOffset += bytesRead;
       } while (byteOffset < packet.getSize());
     }
     // now, flush the decoder
     int bytesRead = 0;
     do {
       bytesRead = decoder.decodeVideo(picture, null, 0);
       if (picture.isComplete()) {
         image = converter.toImage(image, picture);
         startTime = displayPicture(startTime, picture.getTimeStamp(), window, image);
       }
     } while (bytesRead > 0 && picture.isComplete());
    }
    if (window != null)
      window.dispose();
    
  }

  private long displayPicture(long startTime, long timeStamp,
      ImageFrame window,
      BufferedImage image) throws InterruptedException {
    if (startTime == Global.NO_PTS)
      startTime = timeStamp;
    // we will play this back as quickly as we can.
    // Thread.sleep(50);
    if (window != null)
      window.setImage(image);
    
    return startTime;
  }

}
