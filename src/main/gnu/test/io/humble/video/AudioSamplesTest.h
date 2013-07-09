
#ifndef __AUDIOSAMPLES_TEST_H__
#define __AUDIOSAMPLES_TEST_H__

#include <io/humble/testutils/TestUtils.h>

class AudioSamplesTest : public CxxTest::TestSuite
{
  public:
    AudioSamplesTest();
    virtual ~AudioSamplesTest();
    void setUp();
    void tearDown();
    void testCreationAndDestruction();
#ifdef FULLTESTS
    void notestDecodingToBuffer();
    void notestEncodingToBuffer();
#endif
    void testSetSampleEdgeCases();
    void testSetSampleSunnyDayScenarios();
    void testGetSampleRainyDayScenarios();
  private:
#ifdef FULLTESTS
    Helper *h;
    Helper *hw;
#endif
};


#endif // __AUDIOSAMPLES_TEST_H__

