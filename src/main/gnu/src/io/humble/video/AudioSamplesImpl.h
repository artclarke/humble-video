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

#ifndef AUDIOSAMPLESIMPL_H_
#define AUDIOSAMPLESIMPL_H_

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/AudioSamples.h>
#include <io/humble/ferry/IBuffer.h>
#include <io/humble/video/Rational.h>

namespace io { namespace humble { namespace video
{

  class AudioSamplesImpl : public AudioSamples
  {
    VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(AudioSamplesImpl)
  public:
    // Media
    virtual int64_t getTimeStamp() { return getPts(); }
    virtual void setTimeStamp(int64_t aTimeStamp) { setPts(aTimeStamp); }
    virtual Rational* getTimeBase() { return mTimeBase.get(); }
    virtual void setTimeBase(Rational *aBase) { mTimeBase.reset(aBase, true); }
    virtual int32_t getSize();
    virtual bool isKey() { return mFrame->key_frame; }
    virtual bool isPlanar();

    // AudioSamples
    virtual bool isComplete();
    virtual int32_t getSampleRate();
    virtual int32_t getChannels();
    virtual Format getFormat();

    virtual int32_t getNumSamples();
    virtual int32_t getMaxBufferSize();
    virtual int32_t getSampleSize();
    virtual int32_t getMaxSamples();
    virtual io::humble::ferry::IBuffer* getData();
    virtual int64_t getPts();
    virtual void setPts(int64_t aValue);
    virtual int64_t getNextPts();
    virtual void setData(io::humble::ferry::IBuffer* buffer);
    virtual void setComplete(bool complete, int32_t numSamples,
        int32_t sampleRate, int32_t channels, Format sampleFmt,
        int64_t pts);


    /*
     * Convenience method that from C++ returns the buffer
     * managed by getData() above.
     *
     * @param startingSample The sample to start the array at.
     *   That means that only getNumSamples()-startingSample
     *   samples are available in this AudioSamplesImpl collection.
     */
    virtual short *getRawSamples(uint32_t startingSample);

    /*
     * This creates an audio sample.
     */
    static AudioSamplesImpl* make(int32_t numSamples,
        uint32_t numChannels, AudioSamples::Format);
    
    static AudioSamplesImpl* make(io::humble::ferry::IBuffer* buffer,
        int32_t numSamples,
        int32_t channels,
        AudioSamples::Format format);
  protected:
    AVFrame* getCtx() { return mFrame; }
    AudioSamplesImpl();
    virtual ~AudioSamplesImpl();
  private:
    void allocInternalSamples();
    static void setBufferType(AudioSamples::Format format,
        io::humble::ferry::IBuffer * buffer);
    io::humble::ferry::RefPointer<io::humble::ferry::IBuffer> mSamples;
    io::humble::ferry::RefPointer<Rational> mTimeBase;
    AVFrame* mFrame;
    int32_t mIsComplete;
    int32_t mMaxRequestedSamples;
  };

}}}

#endif /*AUDIOSAMPLESIMPL_H_*/
