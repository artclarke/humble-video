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
#include "MediaPacketTest.h"

MediaPacketTest::MediaPacketTest() {
}

MediaPacketTest::~MediaPacketTest() {
  tearDown();
}

void
MediaPacketTest::setUp() {
  packet = 0;
}

void
MediaPacketTest::tearDown() {
  packet = 0;
}

void
MediaPacketTest::testCreationAndDestruction() {
  packet = MediaPacket::make();
  TSM_ASSERT("was able to allocate packet", packet);
}

void
MediaPacketTest::testGetDefaults() {
  packet = MediaPacket::make();
  TSM_ASSERT("was able to allocate packet", packet);

  // everything else should be garbage.
  int64_t position = packet->getPosition();
  TSM_ASSERT("position was not set to -1", position == -1);

}

void
MediaPacketTest::testCopyPacket() {
  const int32_t size = 512;
  packet = MediaPacket::make(size);
  TSM_ASSERT("was able to allocate packet", packet);

  // everything else should be garbage.
  int64_t position = packet->getPosition();
  TSM_ASSERT("position was not set to -1", position == -1);

  position = 4;
  packet->setPosition(position);
  int64_t dts = 28349762;
  packet->setDts(dts);
  int64_t pts = 82729373;
  packet->setPts(pts);
  RefPointer<Rational> timeBase = Rational::make(3, 28972);
  packet->setTimeBase(timeBase.value());
  int32_t streamIndex = 8;
  packet->setStreamIndex(streamIndex);
  int64_t duration = 28387728;
  packet->setDuration(duration);

  // let's get access to the data

  RefPointer<Buffer> data = packet->getData();
  TS_ASSERT_EQUALS(size+64, data->getBufferSize());
  TS_ASSERT_EQUALS(size, packet->getSize());
  uint8_t* raw = (uint8_t*) data->getBytes(0, size);
  for (int i = 0; i < size; i++)
    raw[i] = i % 16;

  // Now, make a copy
  bool tests[] =
    { true, false };
  for (size_t i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
    RefPointer<MediaPacket> newPacket = MediaPacket::make(packet.value(), tests[i]);
    TSM_ASSERT("should not be empty", newPacket);

    // let's make sure that when not copying, the data is the same.

    TSM_ASSERT_EQUALS("should equal", position, newPacket->getPosition());
    TSM_ASSERT_EQUALS("should equal", pts, newPacket->getPts());
    TSM_ASSERT_EQUALS("should equal", dts, newPacket->getDts());
    TSM_ASSERT_EQUALS("should equal", streamIndex, newPacket->getStreamIndex());
    TSM_ASSERT_EQUALS("should equal", duration, newPacket->getDuration());
    RefPointer<Rational> newBase = newPacket->getTimeBase();
    TSM_ASSERT("should be equal", newBase->compareTo(timeBase.value()) == 0);

    RefPointer<Buffer> buf = newPacket->getData();
    TS_ASSERT_EQUALS(size, newPacket->getSize());
    uint8_t* d = (uint8_t*) buf->getBytes(0, size);
    if (!tests[i]) {
      TS_ASSERT_EQUALS(d, raw);
    } else {
      TS_ASSERT_DIFFERS(d, raw);
    }

    TS_ASSERT(d);
    for (int j = 0; j < size; j++) {
      TS_ASSERT_EQUALS(d[j], j % 16);
    }
  }
}

void
MediaPacketTest::testWrapBuffer()
{
  const int size = 512;
  RefPointer<Buffer> buf = Buffer::make(0, size);
  uint8_t* d = (uint8_t*)buf->getBytes(0, size);
  for(int i = 0; i < size; i++)
    d[i] = i % 16;

  packet = MediaPacket::make(buf.value());

  RefPointer<Buffer> data = packet->getData();
  TSM_ASSERT_DIFFERS("should be different Buffer objects since once in a packet, AV manages buffer",
      buf.value(), data.value());
  TS_ASSERT_EQUALS(size, data->getBufferSize());
  TS_ASSERT_EQUALS(size, packet->getSize());
  uint8_t* raw = (uint8_t*) data->getBytes(0, size);
  for (int j = 0; j < size; j++)
    TS_ASSERT_EQUALS(raw[j], j % 16);
}
