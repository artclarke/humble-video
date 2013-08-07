/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
/*
 * FilterGraphTest.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: aclarke
 */

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/video/Source.h>
#include <io/humble/video/SourceStream.h>
#include <io/humble/video/Decoder.h>
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/FilterAudioSource.h>
#include <io/humble/video/FilterAudioSink.h>
#include <io/humble/video/FilterPictureSource.h>
#include <io/humble/video/FilterPictureSink.h>
#include "FilterGraphTest.h"
#include "lodepng.h"

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(io.humble.video);

FilterGraphTest::FilterGraphTest() {
}

FilterGraphTest::~FilterGraphTest() {
}

void
FilterGraphTest::testCreation() {
  RefPointer<FilterGraph> graph = FilterGraph::make();
  TS_ASSERT(graph);
}

void
FilterGraphTest::testAddIO() {
  RefPointer<FilterGraph> graph = FilterGraph::make();
  TS_ASSERT(graph);
  int32_t sampleRate = 22050;
  AudioChannel::Layout layout = AudioChannel::CH_LAYOUT_STEREO;
  AudioFormat::Type sampleFormat = AudioFormat::SAMPLE_FMT_S32P;
  int32_t width = 1024;
  int32_t height = 768;
  PixelFormat::Type pixelFormat = PixelFormat::PIX_FMT_YUV420P;

  RefPointer<FilterAudioSource> asource = graph->addAudioSource("ain",
      sampleRate, layout, sampleFormat, 0);
  TS_ASSERT(asource);
  RefPointer<FilterPictureSource> psource = graph->addPictureSource("pin",
      width, height, pixelFormat, 0, 0);
  TS_ASSERT(psource);
  RefPointer<FilterAudioSink> asink = graph->addAudioSink("aout", sampleRate,
      layout, sampleFormat);
  TS_ASSERT(asink);
  RefPointer<FilterPictureSink> psink = graph->addPictureSink("pout",
      PixelFormat::PIX_FMT_GRAY8);
  TS_ASSERT(psink);

  graph->open("[pin]scale=78:24[pout];[ain]atempo=1.2[aout]");
  {
    // get the string
    char* s = graph->getDisplayString();
    av_free(s);
    LoggerStack stack;
    stack.setGlobalLevel(Logger::LEVEL_DEBUG, false);
    VS_LOG_DEBUG("\nGraph: %s\n", s);
  }
}

void
FilterGraphTest::writePicture(const char* prefix, int32_t* frameNo,
    MediaPicture* picture) {
  TS_ASSERT(picture && picture->isComplete());
  if (!((*frameNo) % 30)) {
    char filename[2048];
    // write data as PGM file.
    snprintf(filename, sizeof(filename), "%s-%06d.png", prefix, *frameNo);
    // only write every n'th frame to save disk space
    RefPointer<Buffer> buf = picture->getData(0);
    uint8_t* data = (uint8_t*) buf->getBytes(0, picture->getDataPlaneSize(0));
    lodepng_encode32_file(filename, data, picture->getWidth(),
        picture->getHeight());
  }

  (*frameNo)++;
}

void
FilterGraphTest::testFilterVideo() {

//  TS_SKIP("Not yet implemented");

//  TestData::Fixture* fixture = mFixtures.getFixture(
//      "bigbuckbunny_h264_aac_5.1.mp4");
  TestData::Fixture* fixture=mFixtures.getFixture("ucl_h264_aac.mp4");
  TS_ASSERT(fixture);
  char filepath[2048];
  mFixtures.fillPath(fixture, filepath, sizeof(filepath));

  RefPointer<Source> source = Source::make();

  source->open(filepath, 0, false, true, 0, 0);

  int32_t numStreams = source->getNumStreams();
  TS_ASSERT_EQUALS(fixture->num_streams, numStreams);

  RefPointer<Decoder> decoder;
  int32_t streamToDecode = -1;
  // find first video stream
  for (int i = 0; i < numStreams; i++) {
    RefPointer<SourceStream> stream = source->getSourceStream(i);
    TS_ASSERT(stream);
    decoder = stream->getDecoder();
    TS_ASSERT(decoder);
    if (decoder->getCodecType() == MediaDescriptor::MEDIA_VIDEO) {
      streamToDecode = i;
      break;
    }
  }
  TS_ASSERT(streamToDecode >= 0);

  decoder->open(0, 0);

  // now, let's start a decoding loop.
  RefPointer<MediaPacket> packet = MediaPacket::make();

  RefPointer<MediaPicture> picture = MediaPicture::make(decoder->getWidth(),
      decoder->getHeight(), decoder->getPixelFormat());

  // let's create our graph
  RefPointer<FilterGraph> graph = FilterGraph::make();

  RefPointer<Rational> timeBase = decoder->getTimeBase();
  RefPointer<Rational> aspectR = decoder->getPropertyAsRational("aspect");

  // now for something crazy
  RefPointer<FilterPictureSource> filterSource;
  filterSource = graph->addPictureSource("in", decoder->getWidth(),
      decoder->getHeight(), decoder->getPixelFormat(), timeBase.value(),
      aspectR.value());

  RefPointer<MediaPicture> filterPicture = MediaPicture::make(480 * 2, // the filter below will double the size
  360 * 2, PixelFormat::PIX_FMT_RGBA);

  // add our inputs and outputs
  RefPointer<FilterPictureSink> filterSink = graph->addPictureSink("out",
  // make the filter do the conversion for us.
      filterPicture->getFormat());
  // and open our graph
  graph->open(
      "[in]scale=w=480:h=360[scaled];[scaled]split=4[0][1][2][3];[0]pad=iw*2:ih*2[a];[1]negate[b];[2]hflip[c];[3]edgedetect[d];[a][b]overlay=w[x];[x][c]overlay=0:h[y];[y][d]overlay=w:h");

  {
    LoggerStack stack;
    stack.setGlobalLevel(Logger::LEVEL_DEBUG, false);
    // for shits and giggles, let's print out the string
    char* graphStr = graph->getDisplayString();
    VS_LOG_DEBUG("Graph String:\n%s\n", graphStr);
    VS_LOG_DEBUG("(%d x %d) (%d)", filterPicture->getWidth(),
        filterPicture->getHeight(), filterPicture->getFormat());
    av_freep(&graphStr);
  }

  int32_t frameNo = 0;
  while (source->read(packet.value()) >= 0) {
    // got a packet; now we try to decode it.
    if (packet->getStreamIndex() == streamToDecode && packet->isComplete()) {
      int32_t bytesRead = 0;
      int32_t byteOffset = 0;
      do {
        bytesRead = decoder->decodeVideo(picture.value(), packet.value(),
            byteOffset);
        if (picture->isComplete()) {
          filterSource->addPicture(picture.value());
          // now pull pictures
          while (filterSink->getPicture(filterPicture.value()) >= 0)
            writePicture("FilterGraphTest_testFilterVideo", &frameNo,
                filterPicture.value());
        }
        byteOffset += bytesRead;
      } while (byteOffset < packet->getSize());
      // now, handle the case where bytesRead is 0; we need to flush any
      // cached packets
      do {
        decoder->decodeVideo(picture.value(), 0, 0);
        if (picture->isComplete()) {
          filterSource->addPicture(picture.value());
          // now pull pictures
          while (filterSink->getPicture(filterPicture.value()) >= 0)
            writePicture("FilterGraphTest_testFilterVideo", &frameNo,
                filterPicture.value());
        }
      } while (picture->isComplete());
    }

  }
  source->close();
}
