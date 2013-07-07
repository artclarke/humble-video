#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/xuggler/IStream.h>
#include <io/humble/xuggler/IStreamCoder.h>
#include <io/humble/xuggler/ICodec.h>
#include <io/humble/xuggler/Global.h>
#include "SourceStreamTest.h"

using namespace VS_CPP_NAMESPACE;

VS_LOG_SETUP(VS_CPP_PACKAGE);

SourceStreamTest::SourceStreamTest()
{
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
  RefPointer<IStream> stream;
  RefPointer<IStreamCoder> coder;
  RefPointer<ICodec> codec;
  RefPointer<IRational> rational;

  h->setupReading(h->SAMPLE_FILE);

  for (int i = 0; i < h->num_streams; i++)
  {
    VS_LOG_TRACE("Starting test of stream: %d", i);

    VS_LOG_TRACE("Stream test: Getting stream: %d", i);
    stream = h->streams[i];
    TSM_ASSERT("could not get stream", stream);

    // container, helper, and me.
    TSM_ASSERT_EQUALS("stream", stream->getCurrentRefCount(), 3);

    TSM_ASSERT("stream index did not match",
        stream->getIndex() == i);

    VS_LOG_TRACE("Stream test: Getting coder: %d", i);
    coder = stream->getStreamCoder();
    TSM_ASSERT("could not get coder for stream", coder);

    TSM_ASSERT_EQUALS("coder", coder->getCurrentRefCount(), 3);

    VS_LOG_TRACE("Stream test: Getting codec: %d", i);
    codec = coder->getCodec();
    TSM_ASSERT("could not get codec", codec);

    TSM_ASSERT_EQUALS("codec", codec->getCurrentRefCount(), 3);

    if (!codec->getType() == ICodec::CODEC_TYPE_AUDIO &&
        !codec->getType() == ICodec::CODEC_TYPE_VIDEO)
    {
      VS_LOG_ERROR("Unexpected type of codec");
      TS_ASSERT(false);
    }

    if (codec->getType() == ICodec::CODEC_TYPE_VIDEO)
    {
      rational = stream->getFrameRate();
      if (h->expected_frame_rate > 0)
        TSM_ASSERT_DELTA("unexpected frame rate",
            rational->getDouble(),
            h->expected_frame_rate, 0.0001);
      rational = stream->getTimeBase();
      if (h->expected_time_base > 0)
        TSM_ASSERT_DELTA("unexpected frame rate",
            rational->getDouble(),
            h->expected_time_base, 0.0001);
      VS_LOG_TRACE("Finished test of stream: %d", i);
    }
  }
}
