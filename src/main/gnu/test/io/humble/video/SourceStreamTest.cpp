#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/Source.h>
#include <io/humble/video/SourceStream.h>
#include <io/humble/video/Global.h>
#include "SourceStreamTest.h"

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

SourceStreamTest::SourceStreamTest()
{
  VS_LOG_TRACE("Creation!");
}

SourceStreamTest::~SourceStreamTest()
{
}

void
SourceStreamTest::setUp()
{
}

void
SourceStreamTest::tearDown()
{
}

void
SourceStreamTest::testCreationAndDestruction()
{
  RefPointer<Source> s = Source::make();
  TestData::Fixture* f = mFixtures.getFixture(0);
  char path[2048];
  mFixtures.fillPath(f, path, sizeof(path));

  int32_t retval;
  retval = s->open(path, 0, false, true, 0, 0);
  TS_ASSERT(retval >= 0);

  int32_t ns = s->getNumStreams();
  TS_ASSERT_EQUALS(ns, f->num_streams);

  // Remember this for after the close -- we best not crash
  RefPointer<SourceStream> firstStream = s->getSourceStream(0);

  for(int i = 0; i < ns; i++) {
    RefPointer<SourceStream> ss = s->getSourceStream(i);
    TS_ASSERT(ss);

    // now, let's go a testing.
    TS_ASSERT_EQUALS(i, ss->getIndex());
  }

  retval = s->close();
  TS_ASSERT(retval >= 0);

  TS_ASSERT(!firstStream->getContainer());
}
