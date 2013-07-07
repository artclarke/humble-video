#ifndef __SOURCESTREAM_TEST_H__
#define __SOURCESTREAM_TEST_H__

#include <io/humble/testutils/TestUtils.h>
using namespace VS_CPP_NAMESPACE;

class StreamTest : public CxxTest::TestSuite
{
  public:
    StreamTest();
    virtual ~StreamTest();
    void setUp();
    void tearDown();
    void testCreationAndDestruction();
  private:
};


#endif // __SOURCESTREAM_TEST_H__

