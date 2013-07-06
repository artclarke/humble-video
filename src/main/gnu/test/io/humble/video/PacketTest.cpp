#include "PacketTest.h"

PacketTest :: PacketTest()
{
}

PacketTest :: ~PacketTest()
{
  tearDown();
}

void
PacketTest :: setUp()
{
  packet = 0;
}

void
PacketTest :: tearDown()
{
  packet = 0;
}

void
PacketTest :: testCreationAndDestruction()
{
  packet = Packet::make();
  TSM_ASSERT("was able to allocate packet", packet);
}

void
PacketTest :: testGetDefaults()
{
  packet = Packet::make();
  TSM_ASSERT("was able to allocate packet", packet);

  // everything else should be garbage.
  int64_t position = packet->getPosition();
  TSM_ASSERT("position was not set to -1", position == -1);

}

void
PacketTest :: testCopyPacket()
{
  packet = Packet::make();
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
  RefPointer<Rational> timeBase = Rational::make(3,28972);
  packet->setTimeBase(timeBase.value());
  int32_t streamIndex = 8;
  packet->setStreamIndex(streamIndex);
  int64_t duration = 28387728;
  packet->setDuration(duration);
  int64_t convergenceDuration = 283;
  packet->setConvergenceDuration(convergenceDuration);
  
  // Now, make a copy
  RefPointer<Packet> newPacket = Packet::make(packet.value(), false);
  TSM_ASSERT("should not be empty", newPacket);
  
  TSM_ASSERT_EQUALS("should equal", position, newPacket->getPosition());
  TSM_ASSERT_EQUALS("should equal", pts, newPacket->getPts());
  TSM_ASSERT_EQUALS("should equal", dts, newPacket->getDts());
  TSM_ASSERT_EQUALS("should equal", streamIndex, newPacket->getStreamIndex());
  TSM_ASSERT_EQUALS("should equal", duration, newPacket->getDuration());
  TSM_ASSERT_EQUALS("should equal", convergenceDuration,
      newPacket->getConvergenceDuration());
  RefPointer<Rational> newBase = newPacket->getTimeBase();
  TSM_ASSERT("should be equal", newBase->compareTo(timeBase.value()) == 0);
}
