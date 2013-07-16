#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/video/SourceStream.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/Global.h>
#include <io/humble/video/VideoPicture.h>
#include "VideoPictureTest.h"
#ifdef FULL_TESTS
#include <io/humble/video/StreamCoder.h>
#include "Helper.h"
#endif

using namespace io::humble::video;
using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

VideoPictureTest::VideoPictureTest()
{
}

VideoPictureTest::~VideoPictureTest()
{
  tearDown();
}

void
VideoPictureTest::setUp()
{
#ifdef FULL_TESTS
  hr = new Helper();
  hw = new Helper();
#endif
}

void
VideoPictureTest::tearDown()
{
#ifdef FULL_TESTS
  if (hr)
    delete hr;
  hr = 0;
  if (hw)
    delete hw;
  hw = 0;
#endif
}

void
VideoPictureTest::testCreationAndDestruction()
{
  RefPointer<VideoPicture> frame = 0;

  frame = VideoPicture::make(PixelFormat::PIX_FMT_YUV420P, 320, 240);
  TSM_ASSERT("got no frame", frame);
  TSM_ASSERT("not a key frame", frame->isKeyFrame());
  TSM_ASSERT("not complete", !frame->isComplete());
  TSM_ASSERT_EQUALS("wrong width", frame->getWidth(), 320);
  TSM_ASSERT_EQUALS("wrong height", frame->getHeight(), 240);
  TSM_ASSERT_EQUALS("wrong pixel format", frame->getPixelType(),
      PixelFormat::PIX_FMT_YUV420P);
}

#ifdef FULL_TESTS
void
VideoPictureTest::testDecodingIntoReusedFrame()
{
  int numFrames = 0;
  int numKeyFrames = 0;
  int numPackets = 0;
  int videoStream = -1;
  int retval = -1;
  RefPointer<VideoPicture> frame = 0;

  int maxFrames = 15 * 10; // 10 seconds of 15fps video
  hr->setupReading(hr->SAMPLE_FILE);
  RefPointer<IPacket> packet = IPacket::make();
  for (int i = 0; i < hr->num_streams; i++)
  {
    if (hr->codecs[i]->getType() == MediaDescriptor::Type::MEDIA_VIDEO)
    {
      // got audio
      videoStream = i;

      frame = VideoPicture::make(
          hr->coders[i]->getPixelType(),
          hr->coders[i]->getWidth(),
          hr->coders[i]->getHeight());
      TSM_ASSERT("got no frame", frame);


      retval = hr->coders[i]->open();
      TSM_ASSERT("! open coder", retval >= 0);
      break;
    }
  }
  TSM_ASSERT("couldn't find a video stream", videoStream >= 0);
  while (hr->container->readNextPacket(packet.value()) == 0
      && numFrames < maxFrames)
  {
    if (packet->getStreamIndex() == videoStream)
    {
      int offset = 0;

      numPackets++;

      VS_LOG_TRACE("packet: pts: %ld; dts: %ld", packet->getPts(),
          packet->getDts());

      while (offset < packet->getSize())
      {
        retval = hr->coders[videoStream]->decodeVideo(
            frame.value(),
            packet.value(),
            offset);
        TSM_ASSERT("could not decode any video", retval>0);
        VS_LOG_TRACE("Presentation time stamp: %ld", frame->getPts());
        offset += retval;
        if (frame->isComplete())
        {
          numFrames++;
          if (frame->isKeyFrame())
            numKeyFrames++;
          if (hr->expected_pixel_format != PixelFormat::PIX_FMT_NONE)
            TSM_ASSERT_EQUALS("unexpected pixel type",
                frame->getPixelType(),
                hr->expected_pixel_format);
          if (hr->expected_width)
            TSM_ASSERT_EQUALS("unexpected width",
                frame->getWidth(),
                hr->expected_width);
          if (hr->expected_height)
            TSM_ASSERT_EQUALS("unexpected height",
                frame->getHeight(),
                hr->expected_height);
        }
      }
    }
  }
  retval = hr->coders[videoStream]->close();
  TSM_ASSERT("could not close coder", retval >= 0);
  TSM_ASSERT("got partial last frame", frame->isComplete());
  TSM_ASSERT("could not find video packets", numPackets >0);
  TSM_ASSERT("could not decode any video", numFrames >0);
  TSM_ASSERT("could not find any key frames", numKeyFrames >0);
}

void
VideoPictureTest::testDecodingAndEncodingIntoFrame()
{
  int numFrames = 0;
  int numKeyFrames = 0;
  int numPackets = 0;
  int videoStream = -1;
  int retval = -1;
  RefPointer<VideoPicture> frame = 0;
  RefPointer<Rational> num(0);

  int maxFrames = 15 * 10; // 10 seconds of 15fps video

  RefPointer<IPacket> packet = IPacket::make();
  TSM_ASSERT("got no packet", packet);

  hw->setupWriting("FrameTest_3_output.flv", "flv", 0, "flv");
  int outStream = hw->first_output_video_stream;
  TSM_ASSERT("couldn't find an outbound video stream", outStream >= 0);

  hr->setupReading(hr->SAMPLE_FILE);
  videoStream = hr->first_input_video_stream;
  TSM_ASSERT("couldn't find a video stream", videoStream >= 0);

  retval = hr->coders[videoStream]->open();
  TSM_ASSERT("! open coder", retval >= 0);

  // Set up the outbound coder with the right
  // type information
  {
    RefPointer<IStreamCoder> ic = hr->coders[videoStream];
    RefPointer<IStreamCoder> oc = hw->coders[outStream];
    oc->setBitRate(ic->getBitRate());
    oc->setPixelType(ic->getPixelType());
    oc->setHeight(ic->getHeight());
    oc->setWidth(ic->getWidth());
    num = Rational::make(15,1);
    oc->setFrameRate(num.value());
    num = Rational::make(1,1000);
    oc->setTimeBase(num.value());
    oc->setNumPicturesInGroupOfPictures(
        ic->getNumPicturesInGroupOfPictures()
        );

    frame = VideoPicture::make(
        ic->getPixelType(),
        ic->getWidth(),
        ic->getHeight());
    TSM_ASSERT("got no frame", frame);

    // open the outbound coder
    retval = oc->open();
    TSM_ASSERT("! open coder", retval >= 0);

    // write a header to our file.
    retval = hw->container->writeHeader();
    TSM_ASSERT("! writeHeader", retval >= 0);
  }

  RefPointer<IPacket> encodingPacket = IPacket::make();
  TSM_ASSERT("! encodingPacket", encodingPacket);

  retval = encodingPacket->allocateNewPayload(200000);
  TSM_ASSERT("! allocateNewPayload", retval >= 0);

  while (hr->container->readNextPacket(packet.value()) == 0
      && numFrames < maxFrames)
  {
    VS_LOG_TRACE("packet: pts: %lld dts: %lld size: %d index: %d flags: %d duration: %d position: %lld",
        packet->getPts(),
        packet->getDts(),
        packet->getSize(),
        packet->getStreamIndex(),
        packet->getFlags(),
        packet->getDuration(),
        packet->getPosition());
    if (packet->getStreamIndex() == videoStream)
    {
      int offset = 0;

      numPackets++;

      VS_LOG_TRACE("packet: pts: %ld; dts: %ld", packet->getPts(),
          packet->getDts());
      while (offset < packet->getSize())
      {

        retval = hr->coders[videoStream]->decodeVideo(
            frame.value(),
            packet.value(),
            offset);
        TSM_ASSERT("could not decode any video", retval>0);
        num = hr->coders[videoStream]->getTimeBase();
        TSM_ASSERT_DISTANCE("time base changed", num->getDouble(), hr->expected_time_base, 0.0001);
        offset += retval;
        if (frame->isComplete())
        {
          numFrames++;
          if (frame->isKeyFrame())
            numKeyFrames++;
          VS_LOG_TRACE("Encoding frame %d; pts: %lld; cur_dts: %lld",
              numFrames, frame->getPts(),
              hr->streams[videoStream]->getCurrentDts());

          retval = hw->coders[outStream]->encodeVideo(
              encodingPacket.value(),
              frame.value(),
              -1);

          TSM_ASSERT("could not encode video", retval >= 0);
          TSM_ASSERT("could not encode video", encodingPacket->getSize() > 0);

          RefPointer<IBuffer> encodedBuffer = encodingPacket->getData();
          TSM_ASSERT("no encoded data", encodedBuffer);
          TSM_ASSERT("less data than there should be",
              encodedBuffer->getBufferSize() >=
              encodingPacket->getSize());

          // now, write the packet to disk.
          retval = hw->container->writePacket(encodingPacket.value());
          TSM_ASSERT("could not write packet", retval >= 0);

        }
      }
    }
  }
  // sigh; it turns out that to flush the encoding buffers you need to
  // ask the encoder to encode a NULL set of samples.  So, let's do that.
  retval = hw->coders[outStream]->encodeVideo(encodingPacket.value(), 0, 0);
  TSM_ASSERT("Could not encode any video", retval >= 0);
  if (retval > 0)
  {
    retval = hw->container->writePacket(encodingPacket.value());
    TSM_ASSERT("could not write packet", retval >= 0);
  }

  retval = hw->container->writeTrailer();
  TSM_ASSERT("! writeTrailer", retval >= 0);

  retval = hr->coders[videoStream]->close();
  TSM_ASSERT("could not close coder", retval >= 0);
  // open the outbound coder
  retval = hw->coders[outStream]->close();
  TSM_ASSERT("! close coder", retval >= 0);
  TSM_ASSERT("got partial last frame", frame->isComplete());
  TSM_ASSERT("could not find video packets", numPackets >0);
  TSM_ASSERT("could not decode any video", numFrames >0);
  TSM_ASSERT("could not find any key frames", numKeyFrames >0);
}

void
VideoPictureTest::testDecodingAndEncodingIntoFrameByCopyingData()
{
  int numFrames = 0;
  int numKeyFrames = 0;
  int numPackets = 0;
  int videoStream = -1;
  int retval = -1;
  RefPointer<VideoPicture> frame = 0;
  RefPointer<Rational> num(0);

  int maxFrames = 15 * 10; // 10 seconds of 15fps video

  RefPointer<IPacket> packet = IPacket::make();
  TSM_ASSERT("got no packet", packet);

  hw->setupWriting("FrameTest_4_output.flv", "flv", 0, "flv");
  int outStream = hw->first_output_video_stream;
  TSM_ASSERT("couldn't find an outbound video stream", outStream >= 0);

  hr->setupReading(hr->SAMPLE_FILE);
  videoStream = hr->first_input_video_stream;
  TSM_ASSERT("couldn't find a video stream", videoStream >= 0);

  retval = hr->coders[videoStream]->open();
  TSM_ASSERT("! open coder", retval >= 0);

  // Set up the outbound coder with the right
  // type information
  {
    RefPointer<IStreamCoder> ic = hr->coders[videoStream];
    RefPointer<IStreamCoder> oc = hw->coders[outStream];
    oc->setBitRate(ic->getBitRate());
    oc->setPixelType(ic->getPixelType());
    oc->setHeight(ic->getHeight());
    oc->setWidth(ic->getWidth());
    num = Rational::make(15,1);
    oc->setFrameRate(num.value());
    num = Rational::make(1,1000);
    oc->setTimeBase(num.value());
    oc->setNumPicturesInGroupOfPictures(
        ic->getNumPicturesInGroupOfPictures()
        );

    frame = VideoPicture::make(
        ic->getPixelType(),
        ic->getWidth(),
        ic->getHeight());
    TSM_ASSERT("got no frame", frame);

    // open the outbound coder
    retval = oc->open();
    TSM_ASSERT("! open coder", retval >= 0);

    // write a header to our file.
    retval = hw->container->writeHeader();
    TSM_ASSERT("! writeHeader", retval >= 0);

  }

  RefPointer<IPacket> encodingPacket = IPacket::make();
  TSM_ASSERT("! encodingPacket", encodingPacket);

  retval = encodingPacket->allocateNewPayload(200000);
  TSM_ASSERT("! allocateNewPayload", retval >= 0);

  while (hr->container->readNextPacket(packet.value()) == 0
      && numFrames < maxFrames)
  {
    VS_LOG_TRACE("packet: pts: %lld dts: %lld size: %d index: %d flags: %d duration: %d position: %lld",
        packet->getPts(),
        packet->getDts(),
        packet->getSize(),
        packet->getStreamIndex(),
        packet->getFlags(),
        packet->getDuration(),
        packet->getPosition());
    if (packet->getStreamIndex() == videoStream)
    {
      int offset = 0;

      numPackets++;

      VS_LOG_TRACE("packet: pts: %ld; dts: %ld", packet->getPts(),
          packet->getDts());
      while (offset < packet->getSize())
      {

        retval = hr->coders[videoStream]->decodeVideo(
            frame.value(),
            packet.value(),
            offset);
        TSM_ASSERT("could not decode any video", retval>0);
        num = hr->coders[videoStream]->getTimeBase();
        TSM_ASSERT_DISTANCE("time base changed", num->getDouble(), hr->expected_time_base, 0.0001);
        offset += retval;
        if (frame->isComplete())
        {
          numFrames++;
          if (frame->isKeyFrame())
            numKeyFrames++;

          // This is the key part of the test; we're going to try copying
          // the frame into a new frame;
          RefPointer<VideoPicture> frameCopy = IVideoPicture::make(
              frame->getPixelType(),
              frame->getWidth(),
              frame->getHeight());
          TSM_ASSERT("Could not make new frame", frameCopy);
          TSM_ASSERT("could not copy frame", frameCopy->copy(frame.value()));

          
          VS_LOG_TRACE("Encoding frame %d; pts: %lld; cur_dts: %lld",
              numFrames, frameCopy->getPts(),
              hr->streams[videoStream]->getCurrentDts());

          retval = hw->coders[outStream]->encodeVideo(
              encodingPacket.value(),
              frameCopy.value(),
              -1);

          TSM_ASSERT("could not encode video", retval >= 0);
          TSM_ASSERT("could not encode video", encodingPacket->getSize() > 0);

          RefPointer<IBuffer> encodedBuffer = encodingPacket->getData();
          TSM_ASSERT("no encoded data", encodedBuffer);
          TSM_ASSERT("less data than there should be",
              encodedBuffer->getBufferSize() >=
              encodingPacket->getSize());

          // now, write the packet to disk.
          retval = hw->container->writePacket(encodingPacket.value());
          TSM_ASSERT("could not write packet", retval >= 0);

        }
      }
    }
  }
  // sigh; it turns out that to flush the encoding buffers you need to
  // ask the encoder to encode a NULL set of samples.  So, let's do that.
  retval = hw->coders[outStream]->encodeVideo(encodingPacket.value(), 0, 0);
  TSM_ASSERT("Could not encode any video", retval >= 0);
  if (retval > 0)
  {
    retval = hw->container->writePacket(encodingPacket.value());
    TSM_ASSERT("could not write packet", retval >= 0);
  }

  retval = hw->container->writeTrailer();
  TSM_ASSERT("! writeTrailer", retval >= 0);

  retval = hr->coders[videoStream]->close();
  TSM_ASSERT("could not close coder", retval >= 0);
  // open the outbound coder
  retval = hw->coders[outStream]->close();
  TSM_ASSERT("! close coder", retval >= 0);
  TSM_ASSERT("got partial last frame", frame->isComplete());
  TSM_ASSERT("could not find video packets", numPackets >0);
  TSM_ASSERT("could not decode any video", numFrames >0);
  TSM_ASSERT("could not find any key frames", numKeyFrames >0);
}

void
VideoPictureTest::testDecodingAndEncodingIntoAFrameByCopyingDataInPlace()
{
  int numFrames = 0;
  int numKeyFrames = 0;
  int numPackets = 0;
  int videoStream = -1;
  int retval = -1;
  RefPointer<VideoPicture> frame = 0;
  RefPointer<VideoPicture> frameCopy = 0;

  RefPointer<Rational> num(0);

  int maxFrames = 15 * 10; // 10 seconds of 15fps video

  RefPointer<IPacket> packet = IPacket::make();
  TSM_ASSERT("got no packet", packet);

  hw->setupWriting("FrameTest_5_output.flv", "flv", 0, "flv");
  int outStream = hw->first_output_video_stream;
  TSM_ASSERT("couldn't find an outbound video stream", outStream >= 0);

  hr->setupReading(hr->SAMPLE_FILE);
  videoStream = hr->first_input_video_stream;
  TSM_ASSERT("couldn't find a video stream", videoStream >= 0);

  retval = hr->coders[videoStream]->open();
  TSM_ASSERT("! open coder", retval >= 0);

  // Set up the outbound coder with the right
  // type information
  {
    RefPointer<IStreamCoder> ic = hr->coders[videoStream];
    RefPointer<IStreamCoder> oc = hw->coders[outStream];
    oc->setBitRate(ic->getBitRate());
    oc->setPixelType(ic->getPixelType());
    oc->setHeight(ic->getHeight());
    oc->setWidth(ic->getWidth());
    num = Rational::make(15,1);
    oc->setFrameRate(num.value());
    num = Rational::make(1,1000);
    oc->setTimeBase(num.value());
    oc->setNumPicturesInGroupOfPictures(
        ic->getNumPicturesInGroupOfPictures()
        );

    frame = VideoPicture::make(
        ic->getPixelType(),
        ic->getWidth(),
        ic->getHeight());
    TSM_ASSERT("got no frame", frame);

    frameCopy = VideoPicture::make(
        ic->getPixelType(),
        ic->getWidth(),
        ic->getHeight());
    TSM_ASSERT("got no frame", frameCopy);


    // open the outbound coder
    retval = oc->open();
    TSM_ASSERT("! open coder", retval >= 0);

    // write a header to our file.
    retval = hw->container->writeHeader();
    TSM_ASSERT("! writeHeader", retval >= 0);

  }

  RefPointer<IPacket> encodingPacket = IPacket::make();
  TSM_ASSERT("! encodingPacket", encodingPacket);

  retval = encodingPacket->allocateNewPayload(200000);
  TSM_ASSERT("! allocateNewPayload", retval >= 0);

  while (hr->container->readNextPacket(packet.value()) == 0
      && numFrames < maxFrames)
  {
    VS_LOG_TRACE("packet: pts: %lld dts: %lld size: %d index: %d flags: %d duration: %d position: %lld",
        packet->getPts(),
        packet->getDts(),
        packet->getSize(),
        packet->getStreamIndex(),
        packet->getFlags(),
        packet->getDuration(),
        packet->getPosition());
    if (packet->getStreamIndex() == videoStream)
    {
      int offset = 0;

      numPackets++;

      VS_LOG_TRACE("packet: pts: %ld; dts: %ld", packet->getPts(),
          packet->getDts());
      while (offset < packet->getSize())
      {

        retval = hr->coders[videoStream]->decodeVideo(
            frame.value(),
            packet.value(),
            offset);
        TSM_ASSERT("could not decode any video", retval>0);
        num = hr->coders[videoStream]->getTimeBase();
        TSM_ASSERT_DISTANCE("time base changed", num->getDouble(), hr->expected_time_base, 0.0001);
        offset += retval;
        if (frame->isComplete())
        {
          numFrames++;
          if (frame->isKeyFrame())
            numKeyFrames++;

          // This is the key part of the test; we're going to try copying
          // the frame into a new frame;
          bool copy = frameCopy->copy(frame.value());
          TSM_ASSERT("could not copy frame", copy);

          VS_LOG_TRACE("Encoding frame %d; pts: %lld; cur_dts: %lld",
              numFrames, frameCopy->getPts(),
              hr->streams[videoStream]->getCurrentDts());

          retval = hw->coders[outStream]->encodeVideo(
              encodingPacket.value(),
              frameCopy.value(),
              -1);

          TSM_ASSERT("could not encode video", retval >= 0);
          TSM_ASSERT("could not encode video", encodingPacket->getSize() > 0);

          RefPointer<IBuffer> encodedBuffer = encodingPacket->getData();
          TSM_ASSERT("no encoded data", encodedBuffer);
          TSM_ASSERT("less data than there should be",
              encodedBuffer->getBufferSize() >=
              encodingPacket->getSize());

          // now, write the packet to disk.
          retval = hw->container->writePacket(encodingPacket.value());
          TSM_ASSERT("could not write packet", retval >= 0);

        }
      }
    }
  }
  // sigh; it turns out that to flush the encoding buffers you need to
  // ask the encoder to encode a NULL set of samples.  So, let's do that.
  retval = hw->coders[outStream]->encodeVideo(encodingPacket.value(), 0, 0);
  TSM_ASSERT("Could not encode any video", retval >= 0);
  if (retval > 0)
  {
    retval = hw->container->writePacket(encodingPacket.value());
    TSM_ASSERT("could not write packet", retval >= 0);
  }

  retval = hw->container->writeTrailer();
  TSM_ASSERT("! writeTrailer", retval >= 0);

  retval = hr->coders[videoStream]->close();
  TSM_ASSERT("could not close coder", retval >= 0);
  // open the outbound coder
  retval = hw->coders[outStream]->close();
  TSM_ASSERT("! close coder", retval >= 0);
  TSM_ASSERT("got partial last frame", frame->isComplete());
  TSM_ASSERT("could not find video packets", numPackets >0);
  TSM_ASSERT("could not decode any video", numFrames >0);
  TSM_ASSERT("could not find any key frames", numKeyFrames >0);
}
#endif // FULL_TESTS

void
VideoPictureTest::testGetAndSetPts()
{
  RefPointer<VideoPicture> frame = 0;

  frame = VideoPicture::make(PixelFormat::PIX_FMT_YUV420P, 320, 240);
  TSM_ASSERT("got no frame", frame);
  TSM_ASSERT("not a key frame", frame->isKeyFrame());
  TSM_ASSERT("not complete", !frame->isComplete());
  TSM_ASSERT_EQUALS("wrong width", frame->getWidth(), 320);
  TSM_ASSERT_EQUALS("wrong height", frame->getHeight(), 240);
  TSM_ASSERT_EQUALS("wrong pixel format", frame->getPixelType(),
      PixelFormat::PIX_FMT_YUV420P);

  frame->setComplete(true, PixelFormat::PIX_FMT_YUV420P, 320, 240, 1);
  TSM_ASSERT_EQUALS("unexpected pts", frame->getPts(), 1);
  frame->setPts(2);
  TSM_ASSERT_EQUALS("unexpected pts", frame->getPts(), 2);

}
