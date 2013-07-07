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
 * TestData.h
 *
 *  Created on: Jul 7, 2013
 *      Author: aclarke
 */

#ifndef TESTDATA_H_
#define TESTDATA_H_

#include <io/humble/testutils/TestUtils.h>

#include <io/humble/video/Codec.h>
#include <io/humble/video/PixelFormat.h>

class TestData
{
public:
  class Fixture {
  public:
    const char* url;
    int num_streams;
    int packets;
    int video_packets;
    int video_frames;
    int video_key_frames;
    int audio_packets;
    int audio_samples;
    double frame_rate;
    double time_base;
    int width;
    int height;
    int gops;
    //  PixelFormat pixel_format;
    int bit_rate;
    int sample_rate;
    int channels;
    int32_t duration;
    int64_t filesize;
    int32_t bitrate;
    io::humble::video::Codec::ID *codec_ids;
    io::humble::video::MediaDescriptor::Type *codec_types;
  };
  void fillPath(TestData::Fixture* f, char *dst, size_t size);

  TestData() {
    mFixtures = 0;
    mNumFixtures = 0;
    mFixtureDir[0] = 0;
    setupFixtures();
  }
  virtual ~TestData() { delete [] mFixtures; }
  int32_t getNumFixtures();
  Fixture* getFixture(int32_t i) {
    if ( i >=0 && i < mNumFixtures)
      return &mFixtures[i];
    else
      return 0;
  }
  private:
  int32_t mNumFixtures;
  Fixture* mFixtures;
  char mFixtureDir[2048];
  void setupFixtures();
  int32_t addFixture(Fixture* fixture);
};


#endif /* TESTDATA_H_ */
