/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
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

%typemap (javacode) io::humble::video::Media,io::humble::video::Media*,io::humble::video::Media& %{

  // used to correct timezone offsets for timestamp format 

  private static final long TIME_OFFSET = -java.util.Calendar.getInstance()
    .getTimeZone().getRawOffset();

  /** The default time stamp format. */
  
  public static final String DEFAULT_TIME_STAMP_FORMAT =
    "%1$tH:%1$tM:%1$tS.%1$tL";

  /**
   * Get a string representation of the time stamp for this {@link
   * Media}.  The time is formatted as: <b>HH:MM:SS.ms</b>
   *
   * @return the printable string form of the time stamp of this media
   *
   * @see #getFormattedTimeStamp(String)
   * @see #DEFAULT_TIME_STAMP_FORMAT
   */

  public String getFormattedTimeStamp()
  {
    return getFormattedTimeStamp(DEFAULT_TIME_STAMP_FORMAT);
  }

  /**
   * Get a string representation of the time stamp for this {@link
   * Media}.  The format of the resulting string is specified by
   * the format parameter.  See {@link java.util.Formatter} for 
   * details on how to specify formats, however a good place to start
   * is with the following format: <b>%1$tH:%1$tM:%1$tS.%1$tL</b>
   *
   * @param format the format for the time stamp string
   *
   * @return the printable string form of the timestamp
   * 
   * @see #getFormattedTimeStamp()
   * @see #DEFAULT_TIME_STAMP_FORMAT
   * @see java.util.Formatter
   */

  public String getFormattedTimeStamp(String format)
  {
    String retval = null;
    java.util.Formatter formatter = new java.util.Formatter();

    try {
      Rational timeBase = getTimeBase();
      if (timeBase == null)
        timeBase = Rational.make(1,(int)Global.DEFAULT_PTS_PER_SECOND);
      retval = formatter.format(format,
          (long)(getTimeStamp() * timeBase.getDouble() * 1000) +
          TIME_OFFSET).toString();
      timeBase.delete();
    } finally {
      formatter.close();
    }
    return retval;
  }
  
%}

%include <io/humble/video/Media.h>
