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
 */

#ifndef VIDEOPICTURE_H_
#define VIDEOPICTURE_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/MediaRaw.h>
#include <io/humble/ferry/IBuffer.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/PixelFormat.h>

namespace io { namespace humble { namespace video
{

  /**
   * Represents one raw (undecoded) picture in a video stream, plus a timestamp
   * for when to display that video picture relative to other items in a {@link IContainer}.
   * <p>
   * All timestamps for this object are always in Microseconds.
   * </p>
   */
  class VS_API_HUMBLEVIDEO VideoPicture : public MediaRaw
  {
  public:
    /**
     * Is this a key frame?
     * 
     * @return is this a key frame
     */
    virtual bool isKeyFrame()=0;
    
    /**
     * Reset if this is a key frame or not.  Note that regardless of how
     * this flag is set, an VideoPicture always contains raw video data (hence the
     * key setting isn't really that important).
     * 
     * @param aIsKey True if a key frame; false if not.
     */
    virtual void setKeyFrame(bool aIsKey)=0;
    
    /**
     * Is this picture completely decoded?
     * 
     * @return is this picture completely decoded?
     */
    virtual bool isComplete()=0;
    
    /**
     * Total size in bytes of the decoded picture.
     * 
     * @return number of bytes of decoded picture
     */
    virtual int32_t getSize()=0;
        
    /**
     * What is the width of the picture.
     * 
     * @return the width of the picture
     */
    virtual int getWidth()=0;
    
    /**
     * What is the height of the picture
     * 
     * @return the height of the picture
     */
    virtual int getHeight()=0;

    /**
     * Returns the pixel format of the picture.
     * 
     * @return the pixel format of the picture.
     */
    virtual PixelFormat::Type getPixelType()=0;
    
    /**
     * What is the Presentation Time Stamp (in Microseconds) of this picture.
     * 
     * The PTS is is scaled so that 1 PTS = 
     * 1/1,000,000 of a second.
     *
     * @return the presentation time stamp (pts)
     */
    virtual int64_t getPts()=0;
    
    /**
     * Set the Presentation Time Stamp (in Microseconds) for this picture.
     * 
     * @see #getPts()
     * 
     * @param value the new timestamp
     */
    virtual void setPts(int64_t value)=0;
    
    /**
     * This value is the quality setting this VideoPicture had when it was
     * decoded, or is the value to use when this picture is next
     * encoded (if reset with setQuality()
     * 
     * @return The quality.
     */
    virtual int getQuality()=0;
    
    /**
     * Set the Quality to a new value.  This will be used the
     * next time this VideoPicture is encoded by a StreamCoder
     * 
     * @param newQuality The new quality.
     * 
     */
    virtual void setQuality(int newQuality)=0;    

    /**
     * Return the size of each line in the VideoPicture data.  Usually there
     * are no more than 4 lines, but the first line no that returns 0
     * is the end of the road.
     * 
     * @param lineNo The line you want to know the (byte) size of.
     * 
     * @return The size (in bytes) of that line in data.
     */
    virtual int getDataLineSize(int lineNo)=0;
    
    /**
     * After modifying the raw data in this buffer, call this function to
     * let the object know it is now complete.
     * 
     * @param aIsComplete Is this VideoPicture complete
     * @param format The pixel format of the data in this picture.  Must match
     *   what the picture was originally constructed with.
     * @param width The width of the data in this picture.  Must match what
     *   the picture was originally constructed with.
     * @param height The height of the data in this picture.  Must match what
     *   the picture was originally constructed with.
     * @param pts The presentation timestamp of the picture that is now complete.
     *   The caller must ensure this PTS is in units of 1/1,000,000 seconds.
     */
    virtual void setComplete(bool aIsComplete, PixelFormat::Type format,
        int width, int height, int64_t pts)=0;

    /**
     * Copy the contents of the given picture into this picture.  All
     * buffers are copied by value, not be reference.
     * 
     * @param srcPicture The picture you want to copy.
     * 
     * @return true if a successful copy; false if not.
     */
    virtual bool copy(VideoPicture* srcPicture)=0;
    
    /**
     * Get a new picture object.
     * <p>
     * You can specify -1 for width and height, in which case all getData() methods
     * will return error until Humble Video decodes something into this frame.  In general
     * you should always try to specify the width and height.
     * </p>
     * <p>
     * Note that any buffers this objects needs will be
     * lazily allocated (i.e. we won't actually grab all
     * the memory until we need it).
     * </p>
     * <p>This is useful because
     * it allows you to hold a VideoPicture object that remembers
     * things like format, width, and height, but know
     * that it doesn't actually take up a lot of memory until
     * the first time someone tries to access that memory.
     * </p>
     * @param format The pixel format (for example, YUV420P).
     * @param width The width of the picture, in pixels, or -1 if you want Humble Video to guess when decoding.
     * @param height The height of the picture, in pixels, or -1 if you want Humble Video to guess when decoding.
     * @return A new object, or null if we can't allocate one.
     */
    static VideoPicture* make(PixelFormat::Type format, int width, int height);
    
    /**
     * Get a new picture by copying the data in an existing frame.
     * @param src The picture to copy.
     * @return The new picture, or null on error.
     */
    static VideoPicture* make(VideoPicture* src);

    /**
     * The different types of images that we can set. 
     * 
     * @see #getPictureType()
     */
    typedef enum {
      /** Undefined */
      PICTURE_TYPE_NONE = AV_PICTURE_TYPE_NONE,
      /** Intra */
      PICTURE_TYPE_I = AV_PICTURE_TYPE_I,
      /** Predicted */
      PICTURE_TYPE_P = AV_PICTURE_TYPE_P,
      /** Bi-dir predicted */
      PICTURE_TYPE_B = AV_PICTURE_TYPE_B,
      /** S(GMC)-VOP MPEG4 */
      PICTURE_TYPE_S = AV_PICTURE_TYPE_S,
      /** Switching Intra */
      PICTURE_TYPE_SI = AV_PICTURE_TYPE_SI,
      /** Switching Predicted */
      PICTURE_TYPE_SP = AV_PICTURE_TYPE_SP,
      /** BI type */
      PICTURE_TYPE_BI = AV_PICTURE_TYPE_BI,
    } PictType;
    
    /**
     * Get the picture type.
     * <p>
     * This will be set on decoding to tell you what type of
     * packet this was decoded from, and when encoding
     * is a request to the encoder for how to encode the picture.
     * </p>
     * <p>
     * The request may be ignored by your codec.
     * </p>
     * @return the picture type.
     */
    virtual PictType getPictureType()=0;
    /**
     * Set the picture type.
     * 
     * @param type The type.
     * 
     * @see #getPictureType()
     */
    virtual void setPictureType(PictType type)=0;

    /**
     * Get a new picture object, by wrapping an existing
     * {@link io.humble.ferry.IBuffer}.
     * <p>
     * Use this method if you have existing video data that you want
     * to have us wrap and pass to FFmpeg.  Note that if decoding
     * into this video picture and the decoded data actually takes more
     * space than is in this buffer, this object will release the reference
     * to the passed in buffer and allocate a new buffer instead so the decode
     * can continue.
     * </p>
     * <p>
     * Due to some decoders assembly optimizations, you should ensure the
     * IBuffer you pass in has at least 8 more bytes than would typically
     * be required based on the format, width and height.
     * </p>
     * @param buffer The {@link io.humble.ferry.IBuffer} to wrap.
     * @param format The pixel format (for example, YUV420P).
     * @param width The width of the picture, in pixels.
     * @param height The height of the picture, in pixels.
     * @return A new object, or null if we can't allocate one.
     */
    static VideoPicture* make(
        io::humble::ferry::IBuffer* buffer,
        PixelFormat::Type format, int width, int height);

  protected:
    VideoPicture();
    virtual ~VideoPicture();
  };

}}}

#endif /*VIDEOPICTURE_H_*/
