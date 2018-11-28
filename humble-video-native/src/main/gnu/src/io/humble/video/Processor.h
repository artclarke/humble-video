/*******************************************************************************
 * Copyright (c) 2018, Andrew "Art" Clarke.  All rights reserved.
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

/*
 * Processor.h
 *
 *  Created on: Nov 27, 2018
 *      Author: aclarke
 */

#ifndef SRC_MAIN_GNU_SRC_IO_HUMBLE_VIDEO_PROCESSOR_H_
#define SRC_MAIN_GNU_SRC_IO_HUMBLE_VIDEO_PROCESSOR_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/MediaRaw.h>

namespace io { namespace humble { namespace video {

  /*
   * The Processor family. Note that NONE of these inherit from
   * Refcounted. These are all interface classes.
   */


  /**
   *
   * #receive(...) and #send(...) methods on Processors can return one of these
   * two flags as a result
   */
  typedef enum ProcessorResult {
    /**
     * The method was successful.
     */
    RESULT_SUCCESS=0,
    /** If returned from #send(Media), then call #receive(Media), and
     * vice-versa if returned from #receive(Media).
     */
    RESULT_AWAITING_DATA=AVERROR(EAGAIN),
    /**
     * The processor has reached the end of it's stream and will return no
     * more data.
     */
    RESULT_END_OF_STREAM=AVERROR_EOF,
  } ProcessorResult;

  /**
   * A ProcessorSource object can have Media objects received
   * from it.
   */
  class VS_API_HUMBLEVIDEO ProcessorSource
  {
  protected:
    ProcessorSource() {}
  };

  /**
   * A ProcessorRawSink object can have MediaRaw objects sent
   * to it.
   */
  class VS_API_HUMBLEVIDEO ProcessorRawSink
  {
  public:
    virtual ProcessorResult send(MediaRaw*)=0;
  protected:
    ProcessorRawSink() {}
    virtual ~ProcessorRawSink() {}
  };
  /**
   * A ProcessorRawSink object can have MediaRaw objects received
   * from it.
   */
  class VS_API_HUMBLEVIDEO ProcessorRawSource
  {
  public:
    virtual ProcessorResult receive(MediaRaw*)=0;
  protected:
    ProcessorRawSource() {}
    virtual ~ProcessorRawSource() {}

  };
  /**
   * A ProcessorRawSink object can have MediaRaw objects sent
   * to it.
   */
  class VS_API_HUMBLEVIDEO ProcessorEncodedSink
  {
  public:
    virtual ProcessorResult send(MediaEncoded*)=0;
  protected:
    ProcessorEncodedSink() {}
    virtual ~ProcessorEncodedSink() {}
  };
  /**
   * A ProcessorRawSink object can have MediaRaw objects received
   * from it.
   */
  class VS_API_HUMBLEVIDEO ProcessorEncodedSource
  {
  public:
    virtual ProcessorResult receive(MediaEncoded*)=0;
  protected:
    ProcessorEncodedSource() {}
    virtual ~ProcessorEncodedSource() {}

  };

}}}


#endif /* SRC_MAIN_GNU_SRC_IO_HUMBLE_VIDEO_PROCESSOR_H_ */
