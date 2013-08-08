#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/video/Demuxer.h>
#include <io/humble/video/DemuxerStream.h>
#include <io/humble/video/Global.h>
#include "DemuxerStreamTest.h"

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

DemuxerStreamTest::DemuxerStreamTest()
{
  VS_LOG_TRACE("Creation!");
}

DemuxerStreamTest::~DemuxerStreamTest()
{
}

void
DemuxerStreamTest::setUp()
{
}

void
DemuxerStreamTest::tearDown()
{
}

void
DemuxerStreamTest::testCreationAndDestruction()
{
  RefPointer<Demuxer> s = Demuxer::make();
  TestData::Fixture* f = mFixtures.getFixture(0);
  char path[2048];
  mFixtures.fillPath(f, path, sizeof(path));

  s->open(path, 0, false, true, 0, 0);

  int32_t ns = s->getNumStreams();
  TS_ASSERT_EQUALS(ns, f->num_streams);

  for(int i = 0; i < ns; i++) {
    RefPointer<DemuxerStream> ss = s->getStream(i);
    TS_ASSERT(ss);

    // now, let's go a testing.
    TS_ASSERT_EQUALS(i, ss->getIndex());
  }
  // Remember this for after the close -- we best not crash
  RefPointer<DemuxerStream> fs = s->getStream(0);

  TS_ASSERT_EQUALS(Global::NO_PTS, fs->getDuration());
  TS_ASSERT_EQUALS(2667, fs->getCurrentDts());
  TS_ASSERT_EQUALS(0, fs->getNumFrames());
  TS_ASSERT_EQUALS(ContainerStream::DISPOSITION_NONE, fs->getDisposition());
  TS_ASSERT_EQUALS(ContainerStream::PARSE_NONE, fs->getParseType());
  TS_ASSERT(!fs->getAttachedPic());
  RefPointer<Rational> r = fs->getFrameRate();
  TS_ASSERT(!r);
  r = fs->getTimeBase();
  TS_ASSERT_DELTA(f->time_base, r->getDouble(), 0.0001);
  TS_ASSERT_EQUALS(4, fs->getNumIndexEntries());


  s->close();

  // this test makes sure that accessing a stream
  // after a container is closed does not cause crashes.
  TS_ASSERT(!fs->getContainer());
}
