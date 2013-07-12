#ifndef __PACKET_TEST_H__
#define __PACKET_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/video/MediaPacket.h>
using namespace io::humble::video;
using namespace io::humble::ferry;

class PacketTest : public CxxTest::TestSuite
{
  public:
    PacketTest();
    virtual ~PacketTest();
    void setUp();
    void tearDown();
    void testCreationAndDestruction();
    void testGetDefaults();
    void testCopyPacket();
    void testWrapIBuffer();
  private:
    RefPointer<MediaPacket> packet;
};


#endif // __PACKET_TEST_H__

