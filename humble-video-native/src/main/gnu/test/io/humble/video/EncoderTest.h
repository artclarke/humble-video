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
/*
 * EncoderTest.h
 *
 *  Created on: Sep 6, 2013
 *      Author: aclarke
 */

#ifndef ENCODERTEST_H_
#define ENCODERTEST_H_
#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/Muxer.h>
#include <io/humble/video/MediaRaw.h>
#include <io/humble/video/Demuxer.h>
#include <io/humble/video/Decoder.h>
#include <io/humble/video/Encoder.h>
#include <io/humble/video/MediaPacket.h>
#include <io/humble/video/MediaResampler.h>

#include "TestData.h"

using namespace io::humble::video;
using namespace io::humble::ferry;

class EncoderTest : public CxxTest::TestSuite
{
public:
  EncoderTest();
  virtual
  ~EncoderTest();

  void testCreation();
  void testEncodeVideo();
  void testEncodeAudio();
  void testEncodeInvalidParameters();
  void testTranscode();
  // https://github.com/artclarke/humble-video/issues/36
  void testRegression36();
private:
  void decodeAndEncode(
      MediaPacket*,
      Decoder*,
      MediaSampled* decoded,
      MediaResampler*,
      MediaSampled* resampled,
      Muxer*,
      Encoder*
      );

  void resampleEncodeAndMux(
      MediaSampled* in,
      MediaResampler*,
      MediaSampled* out,
      Muxer*,
      Encoder*
      );

  void encodeAndMux(MediaSampled*, Muxer*, Encoder*);
  void
  testRegression36Internal (const Codec::ID codecId, const int32_t numSamples,
                            const int32_t sampleRate, const int32_t channels,
                            const AudioChannel::Layout channelLayout,
                            const AudioFormat::Type audioFormat,
                            const int64_t bitRate, const char* testOutputName);

  TestData mFixtures;

};

#endif /* ENCODERTEST_H_ */
