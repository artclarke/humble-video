
#ifndef __VIDEOPICTURE_TEST_H__
#define __VIDEOPICTURE_TEST_H__

#include <io/humble/testutils/TestUtils.h>

class VideoPictureTest : public CxxTest::TestSuite
{
  public:
    VideoPictureTest();
    virtual ~VideoPictureTest();
    void setUp();
    void tearDown();
    void testCreationAndDestruction();
#ifdef FULL_TESTS
    void notestDecodingIntoReusedFrame();
    void notestDecodingAndEncodingIntoFrame();
    void notestDecodingAndEncodingIntoFrameByCopyingData();
    void notestDecodingAndEncodingIntoAFrameByCopyingDataInPlace();
#endif
    void testGetAndSetPts();
  private:
#ifdef FULL_TESTS
    Helper* hr; //reading helper
    Helper* hw; // writing helper
#endif
};


#endif // __VIDEOPICTURE_TEST_H__

