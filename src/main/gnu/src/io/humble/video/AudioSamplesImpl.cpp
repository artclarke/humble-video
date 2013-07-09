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

#include <io/humble/ferry/Logger.h>
#include <io/humble/video/AudioSamplesImpl.h>
#include <io/humble/video/Global.h>

// for memset
#include <cstring>
#include <stdexcept>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video
{
  using namespace io::humble::ferry;
  
  AudioSamplesImpl::AudioSamplesImpl()
  {
    mSamples = 0;
    mNumSamples = 0;
    mSampleRate = 0;
    mChannels = 1;
    mIsComplete = false;
    mSampleFmt = SAMPLE_FMT_S16;
    mPts = Global::NO_PTS;
    mRequestedSamples = 0;
    mTimeBase = Rational::make(1, Global::DEFAULT_PTS_PER_SECOND);
  }

  AudioSamplesImpl::~AudioSamplesImpl()
  {
  }

#define VS_AudioSamplesImpl_BUFFER_PADDING 64
  void
  AudioSamplesImpl::allocInternalSamples()
  {
    if (!mSamples)
    {
      int32_t bufSize = mRequestedSamples * getSampleSize() +
          VS_AudioSamplesImpl_BUFFER_PADDING;

      mSamples = IBuffer::make(this, bufSize);
      if (!mSamples)
        throw std::bad_alloc();
      setBufferType(mSampleFmt, mSamples.value());

      if (mSamples)
      {
        //void * buf = retval->mSamples->getBytes(0, bufSize);
        //don't set to zero; this means the memory is uninitialized
        //which helps find bugs with valgrind
        //memset(buf, 0, bufSize);
        mNumSamples = 0;
        VS_LOG_TRACE("Created AudioSamplesImpl(%d bytes)", bufSize);
      }
      
    }
  }
  
  int32_t
  AudioSamplesImpl::ensureCapacity(int32_t capacity)
  {
    if (mSamples && 
        mSamples->getBufferSize() < (capacity + VS_AudioSamplesImpl_BUFFER_PADDING))
    {
      // crap; need to ditch and re-recreate
      mSamples=0;
    }
    int32_t sampleSize = getSampleSize();
    int32_t requiredSamples;
    if (sampleSize > 0)
      requiredSamples = capacity / getSampleSize();
    else
      requiredSamples = AVCODEC_MAX_AUDIO_FRAME_SIZE;
    mRequestedSamples = requiredSamples;
    
    return 0;
  }
  short*
  AudioSamplesImpl::getRawSamples(uint32_t startingSample)
  {
    short *retval = 0;
    allocInternalSamples();
    if (mSamples)
    {
      uint32_t startingOffset = startingSample*getSampleSize();
      uint32_t bufLen = (mNumSamples*getSampleSize())-startingOffset;
      retval = (short*)mSamples->getBytes(startingOffset, bufLen);
    }
    return retval;
  }

  AudioSamplesImpl*
  AudioSamplesImpl::make(uint32_t numSamples,
      uint32_t numChannels)
  {
    return make(numSamples, numChannels, AudioSamples::SAMPLE_FMT_S16);
  }
  
  AudioSamplesImpl*
  AudioSamplesImpl::make(uint32_t numSamples,
      uint32_t numChannels,
      AudioSamples::Format format)
  {
    AudioSamplesImpl *retval=0;
    if (numSamples > 0 && numChannels > 0)
    {
      retval = AudioSamplesImpl::make();
      if (retval)
      {
        // FFMPEG actually requires a minimum buffer size, so we
        // make sure we're always at least that large.
        retval->mChannels = numChannels;
        retval->mSampleFmt = format;
        retval->mRequestedSamples = numSamples;
      }
    }
    return retval;
  }
  
  AudioSamplesImpl*
  AudioSamplesImpl::make(IBuffer* buffer, int channels,
      AudioSamples::Format format)
  {
    if (!buffer)
      return 0;
    if (format == AudioSamples::SAMPLE_FMT_NONE)
      return 0;
    if (channels < 0)
      return 0;
    if (buffer->getBufferSize()<= 0)
      return 0;
    
    int bytesPerSample = AudioSamples::findSampleBitDepth(format)/8*channels;
    int samplesRequested = buffer->getBufferSize()/bytesPerSample;
    AudioSamplesImpl* retval = 0;
    try
    {
      retval = make(samplesRequested, channels, format);
      if (!retval)
        return 0;
      retval->setData(buffer);
    }
    catch (std::bad_alloc &e)
    {
      VS_REF_RELEASE(retval);
      throw e;
    }
    catch (std::exception& e)
    {
      VS_LOG_DEBUG("error: %s", e.what());
      VS_REF_RELEASE(retval);
    }

    return retval;
  }

  void
  AudioSamplesImpl::setData(IBuffer* buffer)
  {
    if (!buffer) return;
    mSamples.reset(buffer, true);
    setBufferType(mSampleFmt, buffer);
  }
  
  bool
  AudioSamplesImpl::isComplete()
  {
    return mIsComplete;
  }

  AudioSamples::Format
  AudioSamplesImpl::getFormat()
  {
    return mSampleFmt;
  }

  int32_t
  AudioSamplesImpl::getSampleRate()
  {
    return mSampleRate;
  }

  int32_t
  AudioSamplesImpl::getChannels()
  {
    return mChannels;
  }

  uint32_t
  AudioSamplesImpl::getNumSamples()
  {
    return mNumSamples;
  }

  uint32_t
  AudioSamplesImpl::getMaxBufferSize()
  {
    allocInternalSamples();
    return mSamples->getBufferSize()-VS_AudioSamplesImpl_BUFFER_PADDING;
  }

  uint32_t
  AudioSamplesImpl::getSampleBitDepth()
  {
    return AudioSamples::findSampleBitDepth(mSampleFmt);
  }

  uint32_t
  AudioSamplesImpl::getSampleSize()
  {
    uint32_t bits = getSampleBitDepth();
    if (bits < 8)
      bits = 8;

    return bits/8 * getChannels();
  }

  IBuffer*
  AudioSamplesImpl::getData()
  {
    allocInternalSamples();
    IBuffer* retval = mSamples.get();
    if (!retval)
      throw std::bad_alloc();
    return retval;
  }

  uint32_t
  AudioSamplesImpl::getMaxSamples()
  {
    return getMaxBufferSize() / getSampleSize();
  }

  void
  AudioSamplesImpl::setComplete(bool complete, uint32_t numSamples,
      int32_t sampleRate, int32_t channels, Format format,
      int64_t pts)
  {
    mIsComplete = complete;
    if (channels <= 0)
      channels = 1;

    mChannels = channels;
    mSampleRate = sampleRate;
    mSampleFmt = format;
    if (mSamples)
      // if we've allocated a buffer, reset the type
      setBufferType(mSampleFmt, mSamples.value());

    if (mIsComplete)
    {
      mNumSamples = FFMIN(numSamples,
          getMaxBufferSize()/(getSampleSize()));
#if 0
      {
        short* samps = this->getRawSamples(0);
        for(uint32_t i = 0; i < mNumSamples;i++)
        {
          int32_t samp = samps[i];
          VS_LOG_DEBUG("i: %d; samp: %d", i, samp);
        }
      }
#endif // VS_DEBUG
    } else {
      mNumSamples = 0;
    }
    setPts(pts);
  }

  int64_t
  AudioSamplesImpl::getPts()
  {
    return mPts;
  }
  
  void
  AudioSamplesImpl::setPts(int64_t aValue)
  {
    mPts = aValue;
  }

  int64_t
  AudioSamplesImpl::getNextPts()
  {
    int64_t retval = Global::NO_PTS;
    if (mPts != Global::NO_PTS)
      retval = mPts + AudioSamples::samplesToDefaultPts(this->getNumSamples(), this->getSampleRate());

    return retval;
  }

  int32_t
  AudioSamplesImpl::setSample(uint32_t sampleIndex, int32_t channel, Format format, int32_t sample)
  {
    int32_t retval = -1;
    try {
      if (channel < 0 || channel >= mChannels)
        throw std::invalid_argument("cannot setSample for given channel");
      if (format != SAMPLE_FMT_S16)
        throw std::invalid_argument("only support format: SAMPLE_FMT_S16");
      if (sampleIndex >= this->getMaxSamples())
        throw std::invalid_argument("sampleIndex out of bounds");

      short *rawSamples = this->getRawSamples(0);
      if (!rawSamples)
        throw std::runtime_error("no samples buffer set in AudioSamplesImpl");

      rawSamples[sampleIndex*mChannels + channel] = (short)sample;
      retval = 0;
    }
    catch (std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = -1;
    }
    return retval;
  }

  int32_t
  AudioSamplesImpl::getSample(uint32_t sampleIndex, int32_t channel, Format format)
  {
    int32_t retval = 0;
    try
    {
      if (channel < 0 || channel >= mChannels)
        throw std::invalid_argument("cannot getSample for given channel");
      if (format != SAMPLE_FMT_S16)
        throw std::invalid_argument("only support format: SAMPLE_FMT_S16");
      if (sampleIndex >= this->getNumSamples())
        throw std::invalid_argument("sampleIndex out of bounds");

      short *rawSamples = this->getRawSamples(0);
      if (!rawSamples)
        throw std::runtime_error("no samples buffer set in AudioSamplesImpl");

      retval = rawSamples[sampleIndex*mChannels + channel];
    }
    catch(std::exception & e)
    {
      VS_LOG_DEBUG("Error: %s", e.what());
      retval = 0;
    }
    return retval;
  }

  void
  AudioSamplesImpl::setBufferType(AudioSamples::Format format,
      IBuffer* buffer)
  {
    if (!buffer)
      return;
    switch(format)
    {
      case SAMPLE_FMT_DBL:
      case SAMPLE_FMT_DBLP:
        buffer->setType(IBuffer::IBUFFER_DBL64);
        break;
      case SAMPLE_FMT_FLT:
      case SAMPLE_FMT_FLTP:
        buffer->setType(IBuffer::IBUFFER_FLT32);
        break;
      case SAMPLE_FMT_S16:
      case SAMPLE_FMT_S16P:
        buffer->setType(IBuffer::IBUFFER_SINT16);
        break;
      case SAMPLE_FMT_S32:
      case SAMPLE_FMT_S32P:
        buffer->setType(IBuffer::IBUFFER_SINT32);
        break;
      case SAMPLE_FMT_U8:
      case SAMPLE_FMT_U8P:
        buffer->setType(IBuffer::IBUFFER_UINT8);
        break;
      default:
        break;
    }
  }
}}}
