package io.humble.video;

import io.humble.video.awt.ImageFrame;
import io.humble.video.awt.MediaPictureConverter;
import io.humble.video.awt.MediaPictureConverterFactory;
import io.humble.video.javaxsound.AudioFrame;
import io.humble.video.javaxsound.MediaAudioConverter;
import io.humble.video.javaxsound.MediaAudioConverterFactory;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URL;
import java.nio.ByteBuffer;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class FilterGraphTest {

  private Source source;

  @Before
  public void setUp() throws Exception {
    final URL s = this.getClass().getResource("/ucl_h264_aac.mp4");
//    final URL s = this.getClass().getResource("/testfile.mp3");
    final String f = s.getPath();
    source = Source.make();
    source.open(f, null, false, true, null, null);
    source.queryStreamMetaData();

  }

  @After
  public void tearDown() throws Exception {
    source.close();
    source.delete();
  }
  
  @Test
  public void testFilterGenerateVideo() {
    MediaPicture picture = MediaPicture.make(
        320*2,
        240*2,
        PixelFormat.Type.PIX_FMT_BGR24
        );
    final MediaPictureConverter converter = MediaPictureConverterFactory.createConverter(MediaPictureConverterFactory.HUMBLE_BGR_24, picture);
    BufferedImage image = null;
  
    final ImageFrame window = ImageFrame.make();
  
    FilterGraph graph = FilterGraph.make();
    
    // add a sink
    FilterPictureSink fsink = graph.addPictureSink("out", picture.getFormat());
   // create a graph
    graph.open("mandelbrot=s=320x240[mb];" +
        "[mb]split=4[0][1][2][3];" +
        "[0]pad=iw*2:ih*2[a];" +
        "[1]negate[b];" +
        "[2]hflip[c];" +
        "[3]edgedetect[d];" +
        "[a][b]overlay=w[x];" +
        "[x][c]overlay=0:h[y];" +
        "[y][d]overlay=w:h[out]");
  
   int numPics = 0;
   while(fsink.getPicture(picture) >= 0 && numPics < 200) {
     image = converter.toImage(image, picture);
     if (window != null)
       window.setImage(image);
     ++numPics;
   }
    if (window != null)
      window.dispose();
    
  }

  @Test
  public void testFilterGenerateAudio() {
    FilterGraph graph = FilterGraph.make();
    
    int frameSize = 1024;
    MediaAudio audio = MediaAudio.make(frameSize, 22050, 2, AudioChannel.Layout.CH_LAYOUT_STEREO, AudioFormat.Type.SAMPLE_FMT_S16);
    
    // add a sink
   FilterAudioSink fsink = graph.addAudioSink("out", audio.getSampleRate(), audio.getChannelLayout(), audio.getFormat());
   
   // create a graph
   graph.open("sine[a];[a]aphaser[out]");
   
   // now start playing audio
   final MediaAudioConverter converter = MediaAudioConverterFactory.createConverter(MediaAudioConverterFactory.DEFAULT_JAVA_AUDIO,
       audio);
   final AudioFrame audioFrame = AudioFrame.make(converter.getJavaFormat());
   ByteBuffer rawAudio = null;

   int numSamplesPlayed=0;
   do {
     if (fsink.getAudio(audio) >= 0 ) {
       numSamplesPlayed+= audio.getNumSamples();
       rawAudio = converter.toJavaAudio(rawAudio, audio);
       if (audioFrame != null) 
         audioFrame.play(rawAudio);
     }
   } while(numSamplesPlayed < audio.getSampleRate()*10); // 10 seconds of audio
  }

  @Test
  public void testFilterAudio() throws InterruptedException, IOException {
    int audioStream = -1;

    Decoder decoder=null;
    int n = source.getNumStreams();
    for(int i = 0; i < n; i++) {
      SourceStream stream = source.getSourceStream(i);
      decoder = stream.getDecoder();
      if (decoder.getCodecType() == MediaDescriptor.Type.MEDIA_AUDIO) {
        audioStream = i;
        break;
      } else {
        stream.delete();
        decoder.delete();
        decoder = null;
      }
    }
    decoder.open(null, null);
    
    MediaPacket packet = MediaPacket.make();
    MediaAudio audio = MediaAudio.make(
        decoder.getFrameSize(),
        decoder.getSampleRate(),
        decoder.getChannels(),
        decoder.getChannelLayout(),
        decoder.getSampleFormat());
    
    // let's get our filtered audio
    MediaAudio filteredAudio = MediaAudio.make(audio, true);
    
    FilterGraph graph = FilterGraph.make();
    
    // add a source
    FilterAudioSource filterSource = graph.addAudioSource("in", audio.getSampleRate(),
        audio.getChannelLayout(),
        audio.getFormat(),
        audio.getTimeBase());
    
    // add a sink
    FilterAudioSink filterSink = graph.addAudioSink("out",
        audio.getSampleRate(),
        audio.getChannelLayout(),
        audio.getFormat());
    
    // now let's set up a filter
    graph.open("[in]aphaser[out]");
    
    final MediaAudioConverter converter = MediaAudioConverterFactory.createConverter(MediaAudioConverterFactory.DEFAULT_JAVA_AUDIO,
        audio);
    final AudioFrame audioFrame = AudioFrame.make(converter.getJavaFormat());
    ByteBuffer rawAudio = null;
    
    int bytesRead = 0;
    while(source.read(packet)>=0) {
     if (packet.isComplete() && packet.getStreamIndex()==audioStream) {
       // let's try decoding
       int byteOffset = 0;
       do {
         bytesRead = decoder.decodeAudio(audio, packet, byteOffset);
         byteOffset += bytesRead;
         filterSource.addAudio(audio);
         while(filterSink.getAudio(filteredAudio)>= 0)
         {         
           rawAudio = converter.toJavaAudio(rawAudio, filteredAudio);
           if (audioFrame != null) 
             audioFrame.play(rawAudio);
         }
       } while (byteOffset < packet.getSize());
     }
    }
    // now, flush the decoder
    do {
      bytesRead = decoder.decodeAudio(audio, null, 0);
      if (audio.isComplete()) {
        filterSource.addAudio(audio);
        while(filterSink.getAudio(filteredAudio)>= 0)
        {         
          rawAudio = converter.toJavaAudio(rawAudio, filteredAudio);
          if (audioFrame != null) 
            audioFrame.play(rawAudio);
        }
      }
    } while (bytesRead > 0 && audio.isComplete());
    if (audioFrame != null) 
      audioFrame.dispose();
  }
}
