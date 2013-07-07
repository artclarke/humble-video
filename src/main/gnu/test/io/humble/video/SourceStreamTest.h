#ifndef __SOURCESTREAM_TEST_H__
#define __SOURCESTREAM_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include "TestData.h"
using namespace VS_CPP_NAMESPACE;

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
