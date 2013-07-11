#include "PacketTest.h"

PacketTest::PacketTest() {
}

PacketTest::~PacketTest() {
  tearDown();
}

void
PacketTest::setUp() {
  packet = 0;
}

void
PacketTest::tearDown() {
  packet = 0;
}

void
PacketTest::testCreationAndDestruction() {
  packet = Packet::make();
  TSM_ASSERT("was able to allocate packet", packet);
}

void
PacketTest::testGetDefaults() {
  packet = Packet::make();
  TSM_ASSERT("was able to allocate packet", packet);

  // everything else should be garbage.
  int64_t position = packet->getPosition();
  TSM_ASSERT("position was not set to -1", position == -1);

}

void
PacketTest::testCopyPacket() {
  const int32_t size = 512;
  packet = Packet::make(size);
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
  int64_t convergenceDuration = 283;
  packet->setConvergenceDuration(convergenceDuration);

  // let's get access to the data

  RefPointer<IBuffer> data = packet->getData();
  TS_ASSERT_EQUALS(size, data->getBufferSize());
  TS_ASSERT_EQUALS(size, packet->getSize());
  uint8_t* raw = (uint8_t*) data->getBytes(0, size);
  for (int i = 0; i < size; i++)
    raw[i] = i % 16;

  // Now, make a copy
  bool tests[] =
    { true, false };
  for (size_t i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
    RefPointer<Packet> newPacket = Packet::make(packet.value(), tests[i]);
    TSM_ASSERT("should not be empty", newPacket);

    // let's make sure that when not copying, the data is the same.

    TSM_ASSERT_EQUALS("should equal", position, newPacket->getPosition());
    TSM_ASSERT_EQUALS("should equal", pts, newPacket->getPts());
    TSM_ASSERT_EQUALS("should equal", dts, newPacket->getDts());
    TSM_ASSERT_EQUALS("should equal", streamIndex, newPacket->getStreamIndex());
    TSM_ASSERT_EQUALS("should equal", duration, newPacket->getDuration());
    TSM_ASSERT_EQUALS("should equal", convergenceDuration,
        newPacket->getConvergenceDuration());
    RefPointer<Rational> newBase = newPacket->getTimeBase();
    TSM_ASSERT("should be equal", newBase->compareTo(timeBase.value()) == 0);

    RefPointer<IBuffer> buf = newPacket->getData();
    TS_ASSERT_EQUALS(size, newPacket->getSize());
    TS_ASSERT_EQUALS(size, buf->getBufferSize());
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
PacketTest::testWrapIBuffer()
{
  const int size = 512;
  RefPointer<IBuffer> buf = IBuffer::make(0, size);
  uint8_t* d = (uint8_t*)buf->getBytes(0, size);
  for(int i = 0; i < size; i++)
    d[i] = i % 16;

  packet = Packet::make(buf.value());

  RefPointer<IBuffer> data = packet->getData();
  TSM_ASSERT_DIFFERS("should be different IBuffer objects since once in a packet, AV manages buffer",
      buf.value(), data.value());
  TS_ASSERT_EQUALS(size-16, data->getBufferSize());
  TS_ASSERT_EQUALS(size-16, packet->getSize());
  uint8_t* raw = (uint8_t*) data->getBytes(0, size-16);
  for (int j = 0; j < size-16; j++)
    TS_ASSERT_EQUALS(raw[j], j % 16);
}
