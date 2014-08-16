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

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/Codec.h>
using namespace io::humble::video;
using namespace io::humble::ferry;

class CodecTest : public CxxTest::TestSuite
{
  public:
    void setUp();
    void tearDown();
    void testCreationAndDescruction();
    void testInvalidArguments();
    void testFindByName();
    void testGuessEncodingCodecs();
    void testGetInstalledCodecs();
    void testHasCapability();
    void testGetSupportedVideoFramRates();
    void testGetSupportedVideoPixelFormats();
    void testGetSupportedAudioSampleRates();
    void testGetSupportedAudioSampleFormats();
    void testGetSupportedAudioChannelLayouts();
    void testGetSupportedProfiles();
    void testEncodePCM();
  private:
    RefPointer<Codec> codec;
};


#endif // __CODEC_TEST_H__

