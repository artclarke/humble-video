#ifndef __SOURCESTREAM_TEST_H__
#define __SOURCESTREAM_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include "TestData.h"

class DemuxerStreamTest : public CxxTest::TestSuite
{
public:
  DemuxerStreamTest();
  virtual
  ~DemuxerStreamTest();
  void
  setUp();
  void
  tearDown();
  void testCreation();
  void testCreationAndDestruction();

private:
  TestData mFixtures;
};

#endif // __SOURCESTREAM_TEST_H__
