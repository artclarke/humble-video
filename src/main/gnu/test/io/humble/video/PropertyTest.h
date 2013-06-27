/*
 * PropertyTest.h
 *
 *  Created on: Feb 2, 2012
 *      Author: aclarke
 */

#ifndef PROPERTYTEST_H_
#define PROPERTYTEST_H_

#include <io/humble/testutils/TestUtils.h>

class PropertyTest: public CxxTest::TestSuite
{
public:
  PropertyTest();
  virtual
  ~PropertyTest();
  void setUp();
  void tearDown();
  void testCreation();
  void testIteration();
  void testSetMetaData();
};

#endif /* PROPERTYTEST_H_ */
