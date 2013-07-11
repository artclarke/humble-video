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

#include "AudioSamples.h"
#include "Global.h"
#include "AudioSamplesImpl.h"

namespace io { namespace humble { namespace video
{

  AudioSamples::AudioSamples()
  {
  }

  AudioSamples::~AudioSamples()
  {
  }
  
  AudioSamples*
  AudioSamples::make(int32_t numSamples, uint32_t numChannels,
      AudioSamples::Format format)
  {
    Global::init();
    return AudioSamplesImpl::make(numSamples, numChannels, format);
  }
  

  AudioSamples*
  AudioSamples::make(io::humble::ferry::IBuffer* buffer,
      int32_t numSamples,
      int32_t channels,
      AudioSamples::Format format)
  {
    Global::init();
    return AudioSamplesImpl::make(buffer, numSamples, channels, format);
  }
  
  int64_t
  AudioSamples::samplesToDefaultPts(int64_t samples, int sampleRate)
  {
    // Note: These need to always round up!  a "partial sample" actually must
    // be whole (and similar with time stamps).
    int64_t retval = Global::NO_PTS;
    Global::init();
    if (sampleRate > 0)
    {
      int64_t num = samples * Global::DEFAULT_PTS_PER_SECOND;
      int64_t den = sampleRate;
      long double calc = ((long double)num)/((long double)den);
      retval = (int64_t)(calc);
    }
    return retval;
  }

  int64_t
  AudioSamples::defaultPtsToSamples(int64_t duration, int sampleRate)
  {
    int64_t retval = 0;
    Global::init();
    if (duration != Global::NO_PTS)
    {
      int64_t num = duration * sampleRate;
      int64_t den = Global::DEFAULT_PTS_PER_SECOND;
      long double calc = ((long double)num)/((long double)den);
      retval = (int64_t)(calc);
    }
    return retval;
  }


}}}
