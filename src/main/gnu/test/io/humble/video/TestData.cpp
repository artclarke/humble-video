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
 * MediaTestFixture.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/Logger.h>
#include "TestData.h"
// for getenv
#include <cstdlib>

VS_LOG_SETUP(VS_CPP_PACKAGE);

using namespace io::humble::video;

int32_t
TestData::addFixture(TestData::Fixture* f) {
  if (f) {
    // expand our array
    TestData::Fixture *newArray =
        new TestData::Fixture[mNumFixtures + 1];
    if (!newArray) throw std::bad_alloc();
    for (int i = 0; i < mNumFixtures; i++)
      // we rely on copy constructors.
      newArray[i] = mFixtures[i];
    newArray[mNumFixtures] = *f;
    ++mNumFixtures;
    mFixtures = newArray;
  }
  return mNumFixtures;
}
void
TestData::fillPath(Fixture* f, char *dst, size_t size)
{
  snprintf(dst, size, "%s/%s", mFixtureDir, f->url );
}
void
TestData::setupFixtures() {
  char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
  if (!fixtureDirectory) {
    VS_LOG_ERROR("Need to define environment variable VS_TEST_FIXTUREDIR");
    TSM_ASSERT("no fixture dir", false);
    throw new std::runtime_error("Must define environment variable VS_TEST_FIXTUREDIR");
  }
  snprintf(mFixtureDir, sizeof(mFixtureDir), "%s", fixtureDirectory);

  TestData::Fixture fixture;
  fixture.url = "testfile.flv";
  fixture.num_streams = 2;
  fixture.duration = 149264000;
  fixture.filesize = 4546420;
  fixture.bitrate = 243671;
  fixture.audio_packets = 5714;
  fixture.video_packets = 2236;
  // 1 frame per packet
  fixture.video_frames = 2236;
  fixture.video_key_frames = 270;
  fixture.audio_samples = 3291264;
  fixture.packets = 5714 + 2236;
  fixture.frame_rate = 15;
  fixture.time_base = .0010; // This is the default BEFORE we read any packets
  fixture.width = 424;
  fixture.height = 176;
  fixture.gops = 12;
  fixture.pixel_format = PixelFormat::PIX_FMT_YUV420P;
  fixture.bit_rate = 64000;
  fixture.sample_rate = 22050;
  fixture.channels = 1;
  fixture.audio_format = AudioFormat::SAMPLE_FMT_S16P;
//  fixture.codec_types = {
//    MediaDescriptor::Type::MEDIA_VIDEO,
//    MediaDescriptor::Type::MEDIA_AUDIO,
//    MediaDescriptor::Type::MEDIA_UNKNOWN
//  };
//  fixture.codec_ids = {Codec::CODEC_ID_FLV1, Codec::CODEC_ID_MP3, Codec::CODEC_ID_NONE};

  addFixture(&fixture);

  fixture.url = "testfile.mp3";
  fixture.num_streams = 1;
  fixture.duration = Global::NO_PTS;
  fixture.filesize = -1;
  fixture.bitrate = -1;
  fixture.audio_packets = 0;
  fixture.video_packets = 0;
  // 1 frame per packet
  fixture.video_frames = 0;
  fixture.video_key_frames = 0;
  fixture.audio_samples = -1;
  fixture.packets = fixture.audio_packets + fixture.video_packets;
  fixture.frame_rate = 0;
  fixture.time_base = .0010; // This is the default BEFORE we read any packets
  fixture.width = 0;
  fixture.height = 0;
  fixture.gops = 0;
  fixture.pixel_format = PixelFormat::PIX_FMT_NONE;
  fixture.audio_format = AudioFormat::SAMPLE_FMT_S16P;
  fixture.bit_rate = -1;
  fixture.sample_rate = 44100;
  fixture.channels = 2;
//  fixture.codec_types = {
//    MediaDescriptor::Type::MEDIA_AUDIO,
//    MediaDescriptor::Type::MEDIA_UNKNOWN
//  };
//  fixture.codec_ids = {Codec::CODEC_ID_MP3, Codec::CODEC_ID_NONE};
  addFixture(&fixture);

}

