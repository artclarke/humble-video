/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include <cstdlib>
#include <cstring>

#include "MuxerFormatTest.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/MuxerFormat.h>

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

MuxerFormatTest::MuxerFormatTest()
{
}

MuxerFormatTest::~MuxerFormatTest()
{
}

void
MuxerFormatTest::setUp()
{

}

void
MuxerFormatTest::tearDown() {

}
void
MuxerFormatTest::testCreateMuxerFormat() {
  RefPointer<MuxerFormat> format;
  format = MuxerFormat::guessFormat("flv", 0, 0);
  VS_LOG_DEBUG("Pointer: %p", format.value());
  VS_LOG_DEBUG("Name: %s", format->getName());
  VS_LOG_DEBUG("Long Name: %s", format->getLongName());
  VS_LOG_DEBUG("Extensions: %s", format->getExtensions());
  VS_LOG_DEBUG("MimeType: %s", format->getMimeType());
  int32_t n = format->getNumSupportedCodecs();
  VS_LOG_DEBUG("# Supported Codecs: %d", n);
  for(int32_t i = 0; i < n; i++) {
    Codec::ID id = format->getSupportedCodecId(i);
    RefPointer<CodecDescriptor> d = CodecDescriptor::make(id);
    VS_LOG_DEBUG("  ID: %d, Tag: %d",
        id,
        format->getSupportedCodecTag(i));
    VS_LOG_DEBUG("    Name: %s", d->getName());
    VS_LOG_DEBUG("    Type: %d", d->getType());
    VS_LOG_DEBUG("    Long Name: %s", d->getLongName());
    VS_LOG_DEBUG("    Properties: %d", d->getProperties());
  }
  TSM_ASSERT("", strcmp("flv", format->getName()) == 0);
  format = MuxerFormat::guessFormat(0, "foo.flv", 0);
  TSM_ASSERT("", strcmp("flv", format->getName()) == 0);
  format = MuxerFormat::guessFormat(0, 0, "video/x-flv");
  TSM_ASSERT("", strcmp("flv", format->getName()) == 0);

  /** make sure default codec stuff works */
  format = MuxerFormat::guessFormat("mp4", 0, 0);
  TSM_ASSERT("", format);

  Codec::ID id = Codec::CODEC_ID_NONE;

  id = format->getDefaultAudioCodecId();
  TSM_ASSERT_EQUALS("", id, Codec::CODEC_ID_AAC);

  id = format->getDefaultVideoCodecId();
  TSM_ASSERT_EQUALS("", id, Codec::CODEC_ID_H264);

  id = format->getDefaultSubtitleCodecId();
  TSM_ASSERT_EQUALS("", id, Codec::CODEC_ID_NONE);

  id = format->guessCodec("mp4", 0, 0, MediaDescriptor::MEDIA_AUDIO);
  TSM_ASSERT_EQUALS("", id, Codec::CODEC_ID_AAC);

  id = format->guessCodec("mp4", 0, 0, MediaDescriptor::MEDIA_VIDEO);
  TSM_ASSERT_EQUALS("", id, Codec::CODEC_ID_H264);

  id = format->guessCodec("mp4", 0, 0, MediaDescriptor::MEDIA_SUBTITLE);
  TSM_ASSERT_EQUALS("", id, Codec::CODEC_ID_NONE);

}

void
MuxerFormatTest::testInstallation() {
  int32_t n = MuxerFormat::getNumFormats();
  TSM_ASSERT("", n > 0);

  for(int32_t i = 0; i < n; i++) {
    RefPointer<MuxerFormat> f = MuxerFormat::getFormat(i);
    VS_LOG_DEBUG("Name: %s; Description: %s", f->getName(), f->getLongName());
  }
}

