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
    mFrame = avcodec_alloc_frame();
    if (!mFrame)
      throw std::bad_alloc();
    mSamples = 0;
    mFrame->opaque = this;
    mFrame->nb_samples = 0;
    mFrame->channels = 1;
    mFrame->sample_rate = 0;
//    mFrame->channel_layout = (int)AudioSamples::CH_SIDE_LEFT;
    mFrame->format = (int) AudioSamples::SAMPLE_FMT_S16;
    mFrame->pts = Global::NO_PTS;
    // Audio is always a key frame;
    mFrame->key_frame = 1;
    mIsComplete = false;
    mMaxRequestedSamples = 0;
    mTimeBase = Rational::make(1, Global::DEFAULT_PTS_PER_SECOND);
  }

  AudioSamplesImpl::~AudioSamplesImpl()
  {
    avcodec_free_frame(&mFrame);
  }

#define VS_AudioSamplesImpl_BUFFER_PADDING 64
  void
  AudioSamplesImpl::allocInternalSamples()
  {
    if (!mSamples)
    {
      int32_t bufSize = mMaxRequestedSamples * getSampleSize() * getChannels() +
          VS_AudioSamplesImpl_BUFFER_PADDING;

      mSamples = IBuffer::make(this, bufSize);
      if (!mSamples)
        throw std::bad_alloc();
      setBufferType((AudioSamples::Format)mFrame->format, mSamples.value());

      if (mSamples)
      {
        //void * buf = retval->mSamples->getBytes(0, bufSize);
        //don't set to zero; this means the memory is uninitialized
        //which helps find bugs with valgrind
        //memset(buf, 0, bufSize);
        mFrame->nb_samples = 0;
        VS_LOG_TRACE("Created AudioSamplesImpl(%d bytes)", bufSize);
      }
      
    }
  }
  
  bool
  AudioSamplesImpl::isPlanar()
  {
    return av_sample_fmt_is_planar((enum AVSampleFormat)mFrame->format);
  }

  short*
  AudioSamplesImpl::getRawSamples(uint32_t startingSample)
  {
    short *retval = 0;
    allocInternalSamples();
    if (mSamples)
    {
      //TODO: This is broken.
      uint32_t startingOffset = startingSample*getSampleSize();
      uint32_t bufLen = (mFrame->nb_samples*getSampleSize())-startingOffset;
      retval = (short*)mSamples->getBytes(startingOffset, bufLen);
    }
    return retval;
  }
  
  AudioSamplesImpl*
  AudioSamplesImpl::make(int32_t numSamples,
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
        retval->mFrame->channels = numChannels;
        retval->mFrame->format = format;
        retval->mFrame->nb_samples = 0;
        retval->mMaxRequestedSamples = numSamples;
      }
    }
    return retval;
  }
  
  AudioSamplesImpl*
  AudioSamplesImpl::make(IBuffer* buffer,
      int32_t numSamples,
      int32_t channels,
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
    
    AudioSamplesImpl* retval = 0;
    try
    {
      retval = make(numSamples, channels, format);
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
    setBufferType((AudioSamples::Format)mFrame->format, buffer);
  }
  
  bool
  AudioSamplesImpl::isComplete()
  {
    return mIsComplete;
  }

  AudioSamples::Format
  AudioSamplesImpl::getFormat()
  {
    return (AudioSamples::Format)mFrame->format;
  }

  int32_t
  AudioSamplesImpl::getSampleRate()
  {
    return mFrame->sample_rate;
  }

  int32_t
  AudioSamplesImpl::getChannels()
  {
    return mFrame->channels;
  }

  int32_t
  AudioSamplesImpl::getNumSamples()
  {
    return mFrame->nb_samples;
  }

  int32_t
  AudioSamplesImpl::getMaxBufferSize()
  {
    allocInternalSamples();
    return mSamples->getBufferSize()-VS_AudioSamplesImpl_BUFFER_PADDING;
  }

  int32_t
  AudioSamplesImpl::getSampleSize()
  {
    return av_get_bytes_per_sample((enum AVSampleFormat)mFrame->format);
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

  int32_t
  AudioSamplesImpl::getMaxSamples()
  {
    return getMaxBufferSize() / getSampleSize();
  }

  void
  AudioSamplesImpl::setComplete(bool complete, int32_t numSamples,
      int32_t sampleRate, int32_t channels, Format format,
      int64_t pts)
  {
    mIsComplete = complete;
    if (channels <= 0)
      channels = 1;

    mFrame->channels = channels;
    mFrame->sample_rate = sampleRate;
    mFrame->format = format;
    if (mSamples)
      // if we've allocated a buffer, reset the type
      setBufferType((AudioSamples::Format)mFrame->format, mSamples.value());

    if (mIsComplete)
    {
      mFrame->nb_samples = FFMIN(numSamples,
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
      mFrame->nb_samples = 0;
    }
    setPts(pts);
  }

  int64_t
  AudioSamplesImpl::getPts()
  {
    return mFrame->pts;
  }
  
  void
  AudioSamplesImpl::setPts(int64_t aValue)
  {
    mFrame->pts = aValue;
  }

  int64_t
  AudioSamplesImpl::getNextPts()
  {
    int64_t retval = Global::NO_PTS;
    if (mFrame->pts != Global::NO_PTS)
      retval = mFrame->pts + AudioSamples::samplesToDefaultPts(this->getNumSamples(), this->getSampleRate());

    return retval;
  }

  int32_t
  AudioSamplesImpl::getSize()
  {
    return getSampleSize()*getChannels()*getNumSamples();
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
