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

import io.humble.video.Container;
import io.humble.video.Decoder;
import io.humble.video.Demuxer;
import io.humble.video.DemuxerFormat;
import io.humble.video.DemuxerStream;
import io.humble.video.Global;
import io.humble.video.KeyValueBag;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 * Demo application that takes a Media file and displays the known meta-data about it.
 * <p>
 * Concepts introduced:
 * </p>
 * <ul>
 * <li>Demuxers: An {@link Demuxer} object can read from Media {@link Container} objects.</li>
 * <li>DemuxerStreams: {@link DemuxerStream} objects represent Streams of media information inside a {@link Container}.</li>
 * <li>KeyValueBags: {@link KeyValueBag} objects are used throughout Humble to represent key-value meta-data stored inside different objects.</li>
 * </ul>
 * <p>
 * To run from maven, do:
 * </p>
 * <pre>
 * mvn install exec:java -Dexec.mainClass="io.humble.video.demos.GetContainerInfo" -Dexec.args="filename.mp4"
 * </pre>
 * @author aclarke
 */
public class GetContainerInfo {

  /**
   * Parse information from a file, and also optionally print information about what
   * formats, containers and codecs fit into that file.
   * 
   * @param arg The file to open, or null if we just want generic options.
   * @throws IOException if file cannot be opened.
   * @throws InterruptedException if process is interrupted while querying.
   */
  private static void getInfo(String arg) throws InterruptedException, IOException {
    // In Humble, all objects have special contructors named 'make'.
    
    // A Demuxer opens up media containers, parses  and de-multiplexes the streams
    // of media data without those containers.
    final Demuxer demuxer = Demuxer.make();

    // We open the demuxer by pointing it at a URL.
    demuxer.open(arg, null, false, true, null, null);
    
    // Once we've opened a demuxer, Humble can make a guess about the
    // DemuxerFormat. Humble supports over 100+ media container formats.
    final DemuxerFormat format = demuxer.getFormat();
    System.out.printf("URL: '%s' (%s: %s)\n", demuxer.getURL(), format.getLongName(), format.getName());

    // Many programs that make containers, such as iMovie or Adobe Elements, will
    // insert meta-data about the container. Here we extract that meta data and print it.
    KeyValueBag metadata = demuxer.getMetaData();
    System.out.println("MetaData:");
    for(String key: metadata.getKeys())
      System.out.printf("  %s: %s\n", key, metadata.getValue(key));

    // There are a few other key pieces of information that are interesting for
    // most containers; The duration, the starting time, and the estimated bit-rate.
    // This code extracts all three.
    final String formattedDuration = formatTimeStamp(demuxer.getDuration());
    System.out.printf("Duration: %s, start: %f, bitrate: %d kb/s\n",
        formattedDuration,
        demuxer.getStartTime() == Global.NO_PTS ? 0 : demuxer.getStartTime() / 1000000.0,
            demuxer.getBitRate()/1000);
    
    // Finally, a container consists of several different independent streams of
    // data called Streams. In Humble there are two objects that represent streams:
    // DemuxerStream (when you are reading) and MuxerStreams (when you are writing).
    
    // First find the number of streams in this container.
    int ns = demuxer.getNumStreams();
    
    // Now, let's iterate through each of them.
    for (int i = 0; i < ns; i++) {
      DemuxerStream stream = demuxer.getStream(i);

      metadata = stream.getMetaData();
      // Language is usually embedded as metadata in a stream.
      final String language = metadata.getValue("language");
      
      // We will only be able to make a decoder for streams we can actually
      // decode, so the caller should check for null.
      Decoder d = stream.getDecoder();

      System.out.printf(" Stream #0.%1$d (%2$s): %3$s\n", i, language, d != null ? d.toString() : "unknown coder");
      System.out.println("  Metadata:");
      for(String key: metadata.getKeys())
        System.out.printf("    %s: %s\n", key, metadata.getValue(key));
    }
  }

  /**
   * Pretty prints a timestamp (in {@link Global.NO_PTS} units) into a string.
   * @param duration A timestamp in {@link Global.NO_PTS} units).
   * @return A string representing the duration.
   */
  private static String formatTimeStamp(long duration) {
    if (duration == Global.NO_PTS) {
      return "00:00:00.00";
    }
    double d = 1.0 * duration / Global.DEFAULT_PTS_PER_SECOND;
    int hours = (int) (d / (60*60));
    int mins = (int) ((d - hours*60*60) / 60);
    int secs = (int) (d - hours*60*60 - mins*60);
    int subsecs = (int)((d - (hours*60*60.0 + mins*60.0 + secs))*100.0);
    return String.format("%1$02d:%2$02d:%3$02d.%4$02d", hours, mins, secs, subsecs);
  }
  /**
   * Takes a media container (file) as the first argument, opens it,
   * and tells you what's inside the container.
   * 
   * @param args Must contain one string which represents a filename. If no arguments, then prints help.
   * @throws IOException if file cannot be opened.
   * @throws InterruptedException if process is interrupted while querying.
   */
  public static void main(String[] args) throws InterruptedException, IOException {
    final Options options = new Options();
    options.addOption("h", "help", false, "displays help");
    options.addOption("v", "version", false, "version of this library");
    
    final CommandLineParser parser = new org.apache.commons.cli.BasicParser();
    try {
      final CommandLine cmd = parser.parse(options, args);
      if (cmd.hasOption("version")) {
        // let's find what version of the library we're running
        final String version = io.humble.video_native.Version.getVersionInfo();
        System.out.println("Humble Version: " + version);
      } else if (cmd.hasOption("help") || args.length == 0) {
        final HelpFormatter formatter = new HelpFormatter();
        formatter.printHelp(GetContainerInfo.class.getCanonicalName() + " [<filename> ...]", options);
      } else {
        final String[] parsedArgs = cmd.getArgs();
        for(String arg: parsedArgs)
          getInfo(arg);
      }
    } catch (ParseException e) {
      System.err.println("Exception parsing command line: " + e.getLocalizedMessage());
    }
  }


}
