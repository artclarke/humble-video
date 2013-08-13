package io.humble.video;

import io.humble.video.awt.MediaPictureConverter;
import io.humble.video.awt.MediaPictureConverterFactory;
import io.humble.video.awt.ImageFrame;
import io.humble.video.javaxsound.AudioFrame;
import io.humble.video.javaxsound.MediaAudioConverter;
import io.humble.video.javaxsound.MediaAudioConverterFactory;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.nio.ByteBuffer;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class DecoderTest {

  private Demuxer source;
  
  @Before
  public void setUp() throws Exception {
    final URL s = this.getClass().getResource("/ucl_h264_aac.mp4");
    final String f = new File(s.getPath()).getPath();
    source = Demuxer.make();
    source.open(f, null, false, true, null, null);
    source.queryStreamMetaData();

  }

  @After
  public void tearDown() throws Exception {
    source.close();
    source.delete();
  }

  @Test
  public void testDecodeAudio() throws InterruptedException, IOException {
    int audioStream = 0;
    DemuxerStream stream = source.getStream(audioStream);
    Decoder decoder = stream.getDecoder();
    
    decoder.open(null, null);
    
    MediaPacket packet = MediaPacket.make();
    MediaAudio audio = MediaAudio.make(
        decoder.getFrameSize(),
        decoder.getSampleRate(),
        decoder.getChannels(),
        decoder.getChannelLayout(),
        decoder.getSampleFormat());
    
    final MediaAudioConverter converter = MediaAudioConverterFactory.createConverter(MediaAudioConverterFactory.DEFAULT_JAVA_AUDIO,
        audio);
    final AudioFrame audioFrame = AudioFrame.make(converter.getJavaFormat());
    ByteBuffer rawAudio = null;
    
    while(source.read(packet)>=0) {
     if (packet.isComplete() && packet.getStreamIndex()==audioStream) {
       // let's try decoding
       int byteOffset = 0;
       int bytesRead = 0;
       do {
         bytesRead = decoder.decodeAudio(audio, packet, byteOffset);
         rawAudio = converter.toJavaAudio(rawAudio, audio);
         if (audioFrame != null) 
           audioFrame.play(rawAudio);
         byteOffset += bytesRead;
       } while (byteOffset < packet.getSize());
     }
     // now, flush the decoder
     int bytesRead = 0;
     do {
       bytesRead = decoder.decodeAudio(audio, null, 0);
     } while (bytesRead > 0 && audio.isComplete());
    }
    if (audioFrame != null) 
      audioFrame.dispose();
  }

  @Test
  public void testDecodeVideo() throws InterruptedException, IOException {
    int videoStream = 1;
    DemuxerStream stream = source.getStream(videoStream);
    Decoder decoder = stream.getDecoder();
    
    decoder.open(null, null);
    
    MediaPacket packet = MediaPacket.make();
    MediaPicture picture = MediaPicture.make(
        decoder.getWidth(),
        decoder.getHeight(),
        decoder.getPixelFormat()
        );
    final MediaPictureConverter converter = MediaPictureConverterFactory.createConverter(MediaPictureConverterFactory.HUMBLE_BGR_24, picture);
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
