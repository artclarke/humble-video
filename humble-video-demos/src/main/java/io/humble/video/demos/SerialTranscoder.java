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

import java.awt.AWTException;
import java.io.IOException;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 * This demonstration shows how to build a standalone Java program
 * that reads in one media file and attempts to transcode it into
 * another format, potentially decoding and reencoding the data as it does this.
 * 
 * This is really just a proof-of-concept -- if all you're doing is transcoding
 * then I recommend using ffmpeg's command line tool (since it's more than a demo).
 * 
 * But this demo is helpful as you try to understand the Parallel Transcoder.
 * @author aclarke
 *
 */
public class SerialTranscoder {

  @SuppressWarnings("static-access")
  public static void main(String[] args) throws InterruptedException, IOException, AWTException
  {
    final Options options = new Options();
    options.addOption("h", "help", false, "displays help");
    options.addOption("v", "version", false, "version of this library");
    options.addOption(OptionBuilder.withArgName("format")
        .withLongOpt("format")
        .hasArg().
        withDescription("muxer format to use. If unspecified, we will guess from filename")
        .create("f"));
    options.addOption(OptionBuilder.withArgName("codec")
        .withLongOpt("codec")
        .hasArg()
        .withDescription("codec to use when encoding video; If unspecified, we will guess from format")
        .create("c"));
    options.addOption(OptionBuilder.withArgName("duration")
        .withLongOpt("duration")
        .hasArg()
        .withDescription("number of seconds of screenshot to record; defaults to 10.")
        .create("d"));
    options.addOption(OptionBuilder.withArgName("snaps per second")
        .withLongOpt("snaps")
        .hasArg()
        .withDescription("number of pictures to take per second (i.e. the frame rate); defaults to 5")
        .create("s"));

    final CommandLineParser parser = new org.apache.commons.cli.BasicParser();
    try {
      final CommandLine cmd = parser.parse(options, args);
      final String[] parsedArgs = cmd.getArgs();
      if (cmd.hasOption("version")) {
        // let's find what version of the library we're running
        final String version = io.humble.video_native.Version.getVersionInfo();
        System.out.println("Humble Version: " + version);
      } else if (cmd.hasOption("help") || parsedArgs.length != 1) {
        final HelpFormatter formatter = new HelpFormatter();
        formatter.printHelp(RecordAndEncodeVideo.class.getCanonicalName() + " <filename>", options);
      } else {
        /**
         * Read in some option values and their defaults.
         */
        final int duration = Integer.parseInt(cmd.getOptionValue("duration", "10"));
        if (duration <= 0)
          throw new IllegalArgumentException("duration must be > 0");
        final int snaps = Integer.parseInt(cmd.getOptionValue("snaps", "5"));
        if (snaps <= 0)
          throw new IllegalArgumentException("snaps must be > 0");
        final String codecname = cmd.getOptionValue("codec");
        final String formatname = cmd.getOptionValue("format");
        final String filename = cmd.getArgs()[0];

      }
    } catch (ParseException e) {
      System.err.println("Exception parsing command line: " + e.getLocalizedMessage());
    }
  }
}
