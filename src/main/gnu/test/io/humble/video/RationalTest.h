#ifndef __RATIONAL_TEST_H__
#define __RATIONAL_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Rational.h>

using namespace io::humble::video;

class RationalTest : public CxxTest::TestSuite
{
  public:
    void setUp();
    void testCreationAndDestruction();
    void testReduction();
    void testGetDouble();
    void testMultiplication();
    void testAddition();
    void testSubtraction();
    void testDivision();
    void testConstructionFromNumeratorAndDenominatorPair();
    void testRescaling();
  private:
    io::humble::ferry::RefPointer<Rational> num;
};


#endif // __RATIONAL_TEST_H__

