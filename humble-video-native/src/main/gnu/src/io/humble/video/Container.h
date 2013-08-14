/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Container.h
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Property.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/ContainerFormat.h>

namespace io
{
namespace humble
{
namespace video
{

  class ContainerStream;
/**
 * A Container for Media data. This is an abstract class and
 * cannot be instantiated on its own.
 */
class VS_API_HUMBLEVIDEO Container : public ::io::humble::video::Configurable
{
public:
  /**
   * Do not set these flags -- several are used by the internals of Humble Video.
   */
  typedef enum Flag
  {
    /**  Generate missing pts even if it requires parsing future frames. **/
    FLAG_GENPTS = AVFMT_FLAG_GENPTS,
    /**  Ignore index. **/
    FLAG_IGNIDX = AVFMT_FLAG_IGNIDX,
    /**  Do not block when reading packets from input. **/
    FLAG_NONBLOCK = AVFMT_FLAG_NONBLOCK,
    /**  Ignore DTS on frames that contain both DTS & PTS **/
    FLAG_IGNDTS = AVFMT_FLAG_IGNDTS,
    /**  Do not infer any values from other values, just return what is stored in the container **/
    FLAG_NOFILLIN = AVFMT_FLAG_NOFILLIN,
    /**  Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled **/
    FLAG_NOPARSE = AVFMT_FLAG_NOPARSE,
    /**  Do not buffer frames when possible **/
    FLAG_NOBUFFER = AVFMT_FLAG_NOBUFFER,
    /**  The caller has supplied a custom AVIOContext, don't avio_close() it. **/
    FLAG_CUSTOM_IO = AVFMT_FLAG_CUSTOM_IO,
    /**  Discard frames marked corrupted **/
    FLAG_DISCARD_CORRUPT = AVFMT_FLAG_DISCARD_CORRUPT,
    /**  Enable RTP MP4A-LATM payload **/
    FLAG_MP4A_LATM = AVFMT_FLAG_MP4A_LATM,
    /**  try to interleave outputted packets by dts (using this flag can slow demuxing down) **/
    FLAG_SORT_DTS = AVFMT_FLAG_SORT_DTS,
    /**  Enable use of private options by delaying codec open (this could be made default once all code is converted) **/
    FLAG_PRIV_OPT = AVFMT_FLAG_PRIV_OPT,
    /**  Don't merge side data but keep it separate. **/
    FLAG_KEEP_SIDE_DATA = AVFMT_FLAG_KEEP_SIDE_DATA,

  } Flag;

  /**
   * The number of streams in this container.
   * <p>If this container is a Source this will query the stream and find out
   * how many streams are in it.</p><p>If the current thread is interrupted while this blocking method
   * is running the method will return with a negative value.
   * To check if the method exited because of an interruption
   * pass the return value to Error#make(int) and then
   * check Error#getType() to see if it is
   * Error.Type#ERROR_INTERRUPTED.
   * </p>
   *
   * @return The number of streams in this container.
   */
  virtual int32_t
  getNumStreams()=0;

protected:
  Container();
  virtual
  ~Container();
#ifndef SWIG
  virtual void* getCtx() { return getFormatCtx(); }
  virtual AVFormatContext* getFormatCtx()=0;
  // static methods for custom IO
  static int url_read(void*h, unsigned char* buf, int size);
  static int url_write(void*h, unsigned char* buf, int size);
  static int64_t url_seek(void*h, int64_t position, int whence);
#endif // ! SWIG
protected:

};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* CONTAINER_H_ */
