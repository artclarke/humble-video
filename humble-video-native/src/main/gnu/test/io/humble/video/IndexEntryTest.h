#ifndef INDEXENTRYTEST_H_
#define INDEXENTRYTEST_H_

#include <io/humble/testutils/TestUtils.h>

class IndexEntryTest: public CxxTest::TestSuite
{
public:
  IndexEntryTest();
  virtual
  ~IndexEntryTest();
  void setUp();
  void tearDown();
  void testCreation();
#if 0
  void notestAddIndexEntry();
  void notestGetIndexEntry();
#endif
};

#endif /* INDEXENTRYTEST_H_ */
