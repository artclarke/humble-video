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
#ifndef __MEDIAPACKET_TEST_H__
#define __MEDIAPACKET_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/MediaPacket.h>
using namespace io::humble::video;
using namespace io::humble::ferry;

class MediaPacketTest : public CxxTest::TestSuite
{
  public:
    MediaPacketTest();
    virtual ~MediaPacketTest();
    void setUp();
    void tearDown();
    void testCreationAndDestruction();
    void testGetDefaults();
    void testCopyPacket();
    void testWrapBuffer();
  private:
    RefPointer<MediaPacket> packet;
};


#endif // __MEDIAPACKET_TEST_H__

