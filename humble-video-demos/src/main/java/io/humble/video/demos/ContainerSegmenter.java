/*******************************************************************************
 * Copyright (c) 2014, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
package io.humble.video.demos;

import java.io.IOException;

import io.humble.video.Coder;
import io.humble.video.Decoder;
import io.humble.video.Demuxer;
import io.humble.video.DemuxerStream;
import io.humble.video.MediaPacket;
import io.humble.video.Muxer;
import io.humble.video.MuxerFormat;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 * Takes a mp4, mov or other MP4 variant file, and segments it into smaller files that comply with the HTTP Live Streaming spec.
 * 
 * This is meant as a demonstration program to teach the use of the Humble API.
 * <p>
 * Concepts introduced:
 * </p>
 * <ul>
 * <li>Re-muxing: The idea of changing containers while not actually re-encoding the data.</li>
 * </ul>
 * 
 * <p> 
 * To run from maven, do:
 * </p>
 * <pre>
 * mvn -DskipTests install exec:java -Dexec.mainClass="io.humble.video.demos.ContainerSegmenter" -Dexec.args="inputfile.mp4 output.m3u8"
 * </pre>
 * @author aclarke
 *
 */
public class ContainerSegmenter {

  /**
   */
  private static void segmentFile(String input, String output, int hls_start,
      int hls_time, int hls_list_size, int hls_wrap, String hls_base_url) throws InterruptedException, IOException {

    final Demuxer demuxer = Demuxer.make();
    
    demuxer.open(input, null, false, true, null, null);
    
    // we're forcing this to be HTTP Live Streaming for this demo.
    final Muxer muxer = Muxer.make(output, null, "hls");
    muxer.setProperty("start_number", hls_start);
    muxer.setProperty("hls_time", hls_time);
    muxer.setProperty("hls_list_size", hls_list_size);
    muxer.setProperty("hls_wrap", hls_wrap);
    if (hls_base_url != null && hls_base_url.length() > 0)
      muxer.setProperty("hls_base_url", hls_base_url);
    
    final MuxerFormat format = MuxerFormat.guessFormat("mp4", null, null);
    
    int n = demuxer.getNumStreams();
    Decoder[] decoders = new Decoder[n];
    for(int i = 0; i < n; i++) {
      DemuxerStream ds = demuxer.getStream(i);
      decoders[i] = ds.getDecoder();
      Decoder d = decoders[i];
      
      if (d != null) {
        // neat; we can decode. Now let's see if this decoder can fit into the mp4 format.
        if (!format.getSupportedCodecs().contains(d.getCodecID())) {
          throw new RuntimeException("Input filename (" + input + ") contains at least one stream with a codec not supported in the output format: " + d.toString());
        }
        if (format.getFlag(MuxerFormat.Flag.GLOBAL_HEADER))
          d.setFlag(Coder.Flag.FLAG_GLOBAL_HEADER, true);
        d.open(null, null);
        muxer.addNewStream(d);
      }
    }
    muxer.open(null, null);
    final MediaPacket packet = MediaPacket.make();
    while(demuxer.read(packet) >= 0) {
      /**
       * Now we have a packet, but we can only write packets that had decoders we knew what to do with.
       */
      if (packet.isComplete() && decoders[packet.getStreamIndex()] != null)
        muxer.write(packet, true);
    }

    // It is good practice to close demuxers when you're done to free
    // up file handles. Humble will EVENTUALLY detect if nothing else
    // references this demuxer and close it then, but get in the habit
    // of cleaning up after yourself, and your future girlfriend/boyfriend
    // will appreciate it.
    muxer.close();
    demuxer.close();

  }


  /**
   */
  @SuppressWarnings("static-access")
  public static void main(String[] args) throws InterruptedException, IOException {
    final Options options = new Options();
    options.addOption("h", "help", false, "displays help");
    options.addOption("v", "version", false, "version of this library");
    options.addOption(OptionBuilder.withArgName("hls_start")
        .withLongOpt("hls_start")
        .hasArg().
        withDescription("first number in the sequence (defaults to 0).")
        .create("s"));
    options.addOption(OptionBuilder.withArgName("hls_time")
        .withLongOpt("hls_time")
        .hasArg()
        .withDescription("segment length in seconds (defaults to 2).")
        .create("t"));
    options.addOption(OptionBuilder.withArgName("hls_list_size")
        .withLongOpt("hls_list_size")
        .hasArg()
        .withDescription("maximum number of playlist entries (defaults to 5).")
        .create("l"));
    options.addOption(OptionBuilder.withArgName("hls_wrap")
        .withLongOpt("hls_wrap")
        .hasArg()
        .withDescription("set number after which the index wraps (defaults to 0).")
        .create("w"));
    options.addOption(OptionBuilder.withArgName("hls_base_url")
        .withLongOpt("hls_base_url")
        .hasArg()
        .withDescription("URL to prepend to each playlist entry (defaults to '').")
        .create("w"));

    final CommandLineParser parser = new org.apache.commons.cli.BasicParser();
    try {
      final CommandLine cmd = parser.parse(options, args);
      final String[] parsedArgs = cmd.getArgs();
      if (cmd.hasOption("version")) {
        // let's find what version of the library we're running
        final String version = io.humble.video_native.Version.getVersionInfo();
        System.out.println("Humble Version: " + version);
      } else if (cmd.hasOption("help") || parsedArgs.length != 2) {
        final HelpFormatter formatter = new HelpFormatter();
        formatter.printHelp(ContainerSegmenter.class.getCanonicalName() + "<input-filename> <output-filename>", options);
      } else {
        /**
         * Read in some option values and their defaults.
         */
        final int hls_start = Integer.parseInt(cmd.getOptionValue("hls_start", "0"));
        if (hls_start < 0)
          throw new IllegalArgumentException("hls_time must be >= 0");
        final int hls_time = Integer.parseInt(cmd.getOptionValue("hls_time", "2"));
        if (hls_time <= 0)
          throw new IllegalArgumentException("hls_time must be > 0");
        final int hls_list_size = Integer.parseInt(cmd.getOptionValue("hls_list_size", "5"));
        if (hls_list_size <= 0)
          throw new IllegalArgumentException("hls_list_size must be > 0");
        final int hls_wrap = Integer.parseInt(cmd.getOptionValue("hls_wrap", "0"));
        if (hls_wrap < 0)
          throw new IllegalArgumentException("hls_wrap must be >= 0");
        final String hls_base_url = cmd.getOptionValue("hls_base_url");
        final String input = cmd.getArgs()[0];
        final String output = cmd.getArgs()[1];
        segmentFile(input, output, hls_start, hls_time, hls_list_size, hls_wrap, hls_base_url);
      }
    } catch (ParseException e) {
      System.err.println("Exception parsing command line: " + e.getLocalizedMessage());
    }
  }

}
