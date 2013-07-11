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

#ifndef VIDEOPICTUREIMPL_H
#define VIDEOPICTUREIMPL_H

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/VideoPicture.h>
#include <io/humble/video/Codec.h>
#include <io/humble/ferry/IBuffer.h>
#include <io/humble/video/Rational.h>

namespace io { namespace humble { namespace video
{

  class VS_API_HUMBLEVIDEO VideoPictureImpl : public VideoPicture
  {
    VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(VideoPictureImpl);
  public:
    // IMediaData
    virtual int64_t getTimeStamp() { return getPts(); }
    virtual void setTimeStamp(int64_t aTimeStamp) { setPts(aTimeStamp); }
    virtual bool isKey() { return isKeyFrame(); }
    virtual Rational* getTimeBase() { return mTimeBase.get(); }
    virtual void setTimeBase(Rational *aBase) { mTimeBase.reset(aBase, true); }

    // VideoPicture Implementation
    virtual bool isKeyFrame();
    virtual void setKeyFrame(bool aIsKey);
    virtual bool isComplete() { return mIsComplete; }
    virtual int getWidth() { return mFrame->width; }
    virtual int getHeight() { return mFrame->height; }
    virtual PixelFormat::Type getPixelType() { return (PixelFormat::Type) mFrame->format; }
    virtual int64_t getPts();
    virtual void setPts(int64_t);

    virtual int getQuality();
    virtual void setQuality(int newQuality);    
    virtual int32_t getSize();
    virtual io::humble::ferry::IBuffer* getData();
    virtual int getDataLineSize(int lineNo);
    virtual void setComplete(bool aIsComplete, PixelFormat::Type format,
        int width, int height, int64_t pts);
    virtual bool copy(VideoPicture* srcFrame);
    virtual void setData(io::humble::ferry::IBuffer* buffer);

    // Not for calling from Java
    /**
     * Called by the StreamCoder before it encodes a picture.
     * 
     * The VideoPictureImpl fills in Ffmpeg's AVFrame structure with the
     * underlying data for the frame we're managing, but we
     * maintain memory management.
     */
    void fillAVFrame(AVFrame *frame);

    /**
     * Called by the StreamCoder once it's done decoding.
     * 
     * We copy data from the buffers that ffmpeg allocated into
     * our own buffers.
     * 
     * @param frame The AVFrame that ffmpeg filled in.
     * @param pixel The pixel type
     * @param width The width of the AVFrame
     * @param height The height of the AVFrame
     * 
     */
    void copyAVFrame(AVFrame *frame, PixelFormat::Type pixel,
        int32_t width, int32_t height);
    
    /**
     * Call to get the raw underlying AVFrame we manage; don't
     * pass this to ffmpeg directly as ffmpeg often does weird
     * stuff to these guys.
     *
     * Note: This method is exported out of the DLL because
     * the extras library uses it.
     */
    VS_API_HUMBLEVIDEO AVFrame *getAVFrame();

    /**
     * The default factory for a frame.  We require callers to always
     * tell us the format, width and height of the image they want to 
     * store in this VideoPictureImpl.
     * 
     * @param format The pixel format
     * @param width The expected width of this image.
     * @param height The expected height of this image.
     * 
     * @return A new frame that can store an image and associated decoding
     *   information.
     */
    static VideoPictureImpl* make(PixelFormat::Type format, int width, int height);
    
    virtual VideoPicture::PictType getPictureType();
    virtual void setPictureType(VideoPicture::PictType type);
    
    static VideoPictureImpl* make(io::humble::ferry::IBuffer* buffer,
        PixelFormat::Type format, int width, int height);

  protected:
    AVFrame* getCtx() { return getAVFrame(); }
    VideoPictureImpl();
    virtual ~VideoPictureImpl();
    
  private:
    void allocInternalFrameBuffer();
    
    // This is where frame information is kept
    // about a decoded frame.
    AVFrame * mFrame;
    bool mIsComplete;

    io::humble::ferry::RefPointer<io::humble::ferry::IBuffer> mBuffer;
    io::humble::ferry::RefPointer<Rational> mTimeBase;
  };

}}}

#endif /*VIDEOPICTUREIMPL_H*/
