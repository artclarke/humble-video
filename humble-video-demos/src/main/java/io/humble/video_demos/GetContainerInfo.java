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
package io.humble.video_demos;

import java.io.IOException;

import io.humble.video.Codec;
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
 * @author aclarke
 */
public class GetContainerInfo {

  /**
   * Takes a media container (file) as the first argument, opens it,
   * and tells you what's inside the container.
   * 
   * To run from maven, do:
   * <pre>
   * mvn install exec:java -Dexec.mainClass="io.humble.video_demos.GetContainerInfo" -Dexec.args="filename.mp4"
   * </pre>
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
        formatter.printHelp(GetContainerInfo.class.getCanonicalName() + " <filename>", options);
      } else {
        final String[] parsedArgs = cmd.getArgs();
        for(String arg: parsedArgs)
          getInfo(arg, options);
      }
    } catch (ParseException e) {
      System.err.println("Exception parsing command line: " + e.getLocalizedMessage());
    }
  }

  /**
   * Parse information from a file, and also optionally print information about what
   * formats, containers and codecs fit into that file.
   * 
   * @param arg The file to open, or null if we just want generic options.
   * @param options A set of command line options passed in.
   * @throws IOException if file cannot be opened.
   * @throws InterruptedException if process is interrupted while querying.
   */
  private static void getInfo(String arg, Options options) throws InterruptedException, IOException {
    final Demuxer container = Demuxer.make();
    // attempt to open the container. Notes that we assume no extra streams
    // can be added, and that we should attempt to query StreamMetaData (false and true
    // respectively).
    container.open(arg, null, false, true, null, null);
    final DemuxerFormat format = container.getFormat();
    System.out.printf("URL: '%s' (%s: %s)\n", container.getURL(), format.getLongName(), format.getName());

    // get metadata
    KeyValueBag metadata = container.getMetaData();
    System.out.println("MetaData:");
    for(String key: metadata.getKeys())
      System.out.printf("  %s: %s\n", key, metadata.getValue(key));

    // print out some basics about the container
    final String formattedDuration = formatTimeStamp(container.getDuration());
    System.out.printf("Duration: %s, start: %f, bitrate: %d kb/s\n",
        formattedDuration,
        container.getStartTime() == Global.NO_PTS ? 0 : container.getStartTime() / 1000000.0,
            container.getBitRate()/1000);
    
    // now, let's print out details on each stream.
    int ns = container.getNumStreams();
    for (int i = 0; i < ns; i++) {
      DemuxerStream stream = container.getStream(i);
      Decoder d = stream.getDecoder();
      System.out.printf(" Stream #0.%1$d (%s): %s\n", i, d != null ? d.toString() : "unknown coder");
      System.out.println("  Metadata:");
      metadata = stream.getMetaData();
      for(String key: metadata.getKeys())
        System.out.printf("  %s: %s\n", key, metadata.getValue(key));
    }
  }

  /**
   * @param duration
   * @return
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
}
