/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
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

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <io/humble/ferry/Mutex.h>
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/config.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/Rational.h>

namespace io { namespace humble { namespace video
{

/**
 * A collection of static functions that refer to the entire package (like version getters).
 *
 */
  class VS_API_HUMBLEVIDEO Global : public io::humble::ferry::RefCounted
  {
  public:
    /**
     * A value that means no time stamp is set for a given object.
     * if the Media#getTimeStamp() method of an
     * object returns this value it means the time stamp wasn't set.
     */
    static const int64_t NO_PTS=0x8000000000000000LL;
    /**
     * The default time units per second that we use for decoded
     * MediaRaw objects.
     *
     * This means that 1 tick of a time stamp is 1 Microsecond.
     */
    static const int64_t DEFAULT_PTS_PER_SECOND=1000000;

    /**
     * The default timebase used by media if not otherwise specified.
     */
    static Rational* getDefaultTimeBase();

    /**
     * Get the major version number of this library.
     * @return the major version number of this library or 0 if unknown.
     */
    static int32_t getVersionMajor();
    /**
     * Get the minor version number of this library.
     * @return the minor version number of this library or 0 if unknown.
     */
    static int32_t getVersionMinor();

    /**
     * Get a string representation of the version of this library.
     * @return the version of this library in string form.
     */
    static const char* getVersionStr();

    /**
     * Get the version of the FFMPEG libavformat library we are compiled against.
     * @return the version.
     */
    static int getAVFormatVersion();
    /**
     * Get the version of the FFMPEG libavformat library we are compiled against.
     * @return the version.
     */
    static const char* getAVFormatVersionStr();
    /**
     * Get the version of the FFMPEG libavcodec library we are compiled against.
     * @return the version.
     */
    static int getAVCodecVersion();
    /**
     * Get the version of the FFMPEG libavcodec library we are compiled against.
     * @return the version.
     */
    static const char* getAVCodecVersionStr();

#ifndef SWIG
    /**
     * Performs a global-level lock of the HUMBLEVIDEO library.
     * While this lock() is held, every other method that calls
     * lock() in the same DLL/SO will block.
     * <p>
     * Use this sparingly.  FFMPEG's libraries have few
     * real global objects, but when they do exist, you must lock.
     * </p>
     * <p>
     * Lastly, if you lock, make damn sure you call #unlock()
     * </p>
     */
    static void lock();

    /**
     * Unlock the global lock.
     * @see #lock()
     */
    static void unlock();


    /**
     * Internal Only.  Do not call.
     * Checks to determine if there is
     */
    static int avioInterruptCB(void*);

    /**
     * Internal Only.  Call to relese globals.
     */
    static void deinit();

    static void catchException(const std::exception & e);
#endif // ! SWIG


    /**
     * Internal Only.  Do not call.
     * Methods using the C++ interface that will not necessarily
     * create other Global object should call this.  In general,
     * unless you're extending Humble Video directly yourself, ordinary
     * users of this library don't need to call this.
     * <p>
     * It's main purpose is to ensure any FFmpeg required environment
     * initialization functions are called, and any Humble Video required
     * environmental contexts are set up.
     * </p>
     */
    static void init();

    /**
     * Internal Only.  This method can be used to turn up or
     * down FFmpeg's logging level.
     * @param level An integer value for level.  Lower numbers
     *   mean less logging.  A negative number tells FFmpeg to
     *   shut up.
     */
    static void setFFmpegLoggingLevel(int32_t level);

  private:
    Global();
    virtual ~Global();
    static void destroyStaticGlobal(JavaVM*vm,void*closure);

    static Global* sGlobal;
    io::humble::ferry::Mutex* mLock;
    io::humble::ferry::RefPointer<Rational> mDefaultTimeBase;
  };
}}}

#endif /*GLOBAL_H_*/
