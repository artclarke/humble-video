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
    void testWrapIBuffer();
  private:
    RefPointer<MediaPacket> packet;
};


#endif // __MEDIAPACKET_TEST_H__

