/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
/*
 * MediaSubtitle.h
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#ifndef MEDIASUBTITLE_H_
#define MEDIASUBTITLE_H_

#include <io/humble/ferry/HumbleException.h>
#include <io/humble/video/MediaRaw.h>

namespace io {
namespace humble {
namespace video {

class MediaSubtitleRectangle;

/**
 * NOT CURRENTLY USED.
 */
class MediaSubtitle : public io::humble::video::MediaRaw
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaSubtitle);
public:
  typedef enum Type {
      SUBTITLE_NONE,

      /** A bitmap. */
      SUBTITLE_BITMAP,

      /**
       * Plain text, the text field must be set by the decoder and is
       * authoritative. ass and pict fields may contain approximations.
       */
      SUBTITLE_TEXT,

      /**
       * Formatted text, the ass field must be set by the decoder and is
       * authoritative. pict and text fields may contain approximations.
       */
      SUBTITLE_ASS,
  } Type;

  virtual uint16_t getFormat() { return mCtx->format; }

  /**
   * Get the start display time in ms, relative to the #getTimeStamp().
   */
  virtual int64_t getStartDisplayTime() { return mCtx->start_display_time; }

  /**
   * Get the end display time in ms, relative to #getTimeStamp().
   */
  virtual int64_t getEndDisplayTime() { return mCtx->end_display_time; }
  /** Get the number of rectangles this subtitle draws */
  virtual int32_t getNumRectangles() { return mCtx->num_rects; }
  /** Get the presentation timestamp of this subtitle */
  virtual int64_t getTimeStamp() { return mCtx->pts; }

  virtual MediaSubtitleRectangle* getRectangle(int32_t n);

#ifndef SWIG
  static MediaSubtitle* make(AVSubtitle* ctx);
#endif // !SWIG

  virtual void setTimeStamp(int64_t timeStamp) { mCtx->pts = timeStamp; }
  virtual bool isKey() { return true; }
  virtual int64_t getPts() { return mCtx->pts; }
  virtual int64_t getPacketPts() { return Global::NO_PTS; }
  virtual int64_t getPacketDts() { return Global::NO_PTS; }
  virtual int32_t getPacketSize() { return -1; };
  virtual int64_t getPacketDuration() { return Global::NO_PTS; }
  virtual int64_t getBestEffortTimeStamp() { return mCtx->pts; }
  virtual bool isComplete() { return mComplete; }
  /**
   * Sets the subtitle to be complete.
   */
  virtual void setComplete(bool complete, int64_t timestamp) {
    mCtx->pts = timestamp;
    mComplete = complete;
  }
  virtual void setComplete(bool complete) { setComplete(complete, Global::NO_PTS); }

#ifndef SWIG
   virtual AVFrame *getCtx() { return 0; }
#endif
protected:
  MediaSubtitle();
  virtual
  ~MediaSubtitle();
private:
  AVSubtitle* mCtx;
  bool mComplete;
};

/**
 * NOT CURRENTLY USED.
 */
class MediaSubtitleRectangle : public io::humble::ferry::RefCounted
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaSubtitleRectangle);
public:
  typedef enum Flag {
    SUBTITLE_FLAG_FORCED=AV_SUBTITLE_FLAG_FORCED,
  } Flag;
  virtual int32_t getX() { return mCtx->x; }
  virtual int32_t getY() { return mCtx->y; }
  virtual int32_t getWidth() { return mCtx->w; }
  virtual int32_t getHeight() { return mCtx->h; }
  virtual int32_t getNumColors() { return mCtx->nb_colors; }
  virtual MediaSubtitle::Type getType() { return (MediaSubtitle::Type)mCtx->type; }
  virtual const char* getText() { return mCtx->text; }
  virtual const char* getASS() { return mCtx->ass; }
  virtual int32_t getFlags() { return mCtx->flags; }
  virtual int32_t getPictureLinesize(int line);
  virtual io::humble::ferry::Buffer* getPictureData(int line);
#ifndef SWIG
  static MediaSubtitleRectangle* make(AVSubtitleRect* ctx);
#endif // !SWIG
protected:
  MediaSubtitleRectangle() : mCtx(0) {}
  virtual ~MediaSubtitleRectangle() {}
private:
  AVSubtitleRect* mCtx;
};


} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MEDIASUBTITLE_H_ */
