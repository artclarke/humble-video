#ifndef __SOURCESTREAM_TEST_H__
#define __SOURCESTREAM_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include "TestData.h"

class SourceStreamTest : public CxxTest::TestSuite
{
public:
  SourceStreamTest();
  virtual
  ~SourceStreamTest();
  void
  setUp();
  void
  tearDown();
  void testCreationAndDestruction();
private:
  TestData mFixtures;
};

#endif // __SOURCESTREAM_TEST_H__
