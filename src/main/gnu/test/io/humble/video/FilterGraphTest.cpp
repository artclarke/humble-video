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
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/FilterAudioSource.h>
#include <io/humble/video/FilterAudioSink.h>
#include <io/humble/video/FilterPictureSource.h>
#include <io/humble/video/FilterPictureSink.h>
#include "FilterGraphTest.h"

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
  RefPointer<FilterAudioSink> asink = graph->addAudioSink("aout",
      sampleRate, layout, sampleFormat);
  TS_ASSERT(asink);
  RefPointer<FilterPictureSink> psink = graph->addPictureSink("pout",
      width, height, PixelFormat::PIX_FMT_GRAY8);
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
