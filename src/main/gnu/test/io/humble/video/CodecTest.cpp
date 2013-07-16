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

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/video/Global.h>
#include "CodecTest.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

void
CodecTest :: setUp()
{
  codec = 0;
}

void
CodecTest :: tearDown()
{
  codec = 0;
}

void
CodecTest :: testCreationAndDescruction()
{
  codec = Codec::findDecodingCodec(Codec::CODEC_ID_NELLYMOSER);
  TSM_ASSERT("could not find NELLYMOSER decoder", codec);

  codec = Codec::findEncodingCodec(Codec::CODEC_ID_MP3);
  TSM_ASSERT("could not find MP3 encoder", codec);

  // should not find these.
  codec = Codec::findEncodingCodec(Codec::CODEC_ID_RV40);
  TSM_ASSERT("could find Real Audio encoder, but we thought FFMPEG couldn't support that", !codec);

}

void
CodecTest :: testFindByName()
{
  codec = Codec::findDecodingCodecByName("nellymoser");
  TSM_ASSERT("could not find NELLYMOSER decoder", codec);

  codec = Codec::findEncodingCodecByName("libmp3lame");
  TSM_ASSERT("could not find MP3 encoder", codec);


  // should not find these.
  codec = Codec::findEncodingCodecByName("xan_wc3");
  TSM_ASSERT("could find xan_wc3 encoder", !codec);

  codec = Codec::findDecodingCodecByName("adts");
  TSM_ASSERT("could find ADTS Decoder", !codec);

}

void
CodecTest :: testInvalidArguments()
{
  // should not find these.
  codec = Codec::findEncodingCodecByName(0);
  TSM_ASSERT("could find null encoder", !codec);

  codec = Codec::findEncodingCodecByName("");
  TSM_ASSERT("could find empty encoder", !codec);

  codec = Codec::findDecodingCodecByName(0);
  TSM_ASSERT("could find null Decoder", !codec);

  codec = Codec::findDecodingCodecByName("");
  TSM_ASSERT("could find empty Decoder", !codec);

}

void
CodecTest :: testGuessEncodingCodecs()
{
  // should not find these.
  codec = Codec::guessEncodingCodec(0, 0, "file.mov", 0,
      MediaDescriptor::MEDIA_VIDEO);
  TSM_ASSERT("could not find mpeg4 codec", codec);
  codec = Codec::guessEncodingCodec(0, 0, "file.flv", 0,
      MediaDescriptor::MEDIA_VIDEO);
  TSM_ASSERT("could not find flv codec", codec);
  codec = Codec::guessEncodingCodec(0, 0, "file.flv", 0,
      MediaDescriptor::MEDIA_AUDIO);
  TSM_ASSERT("could not find flv codec", codec);
  codec = Codec::guessEncodingCodec(0, 0, 0, 0,
      MediaDescriptor::MEDIA_AUDIO);
  TSM_ASSERT("could find codec", !codec);

}

void
CodecTest :: testGetInstalledCodecs()
{
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_INFO, false);

  int num = Codec::getNumInstalledCodecs();
  TSM_ASSERT("should be > 1", num > 1);
  for(int i = 0; i < num; i++)
  {
    codec = Codec::getInstalledCodec(i);
    TSM_ASSERT("should be valid", codec);
    if (codec) {
      VS_LOG_DEBUG("%s: %s",
          codec->getName(), codec->getLongName());
    }
  }
  TSM_ASSERT("could fail quietly",
      0 == Codec::getInstalledCodec(-1));
  TSM_ASSERT("could fail quietly",
      0 == Codec::getInstalledCodec(0x7FFFFFFF));
}

void
CodecTest :: testHasCapability()
{
  codec = Codec::findEncodingCodecByName("nellymoser");
  TSM_ASSERT("got codec", codec);
  int32_t capabilities = codec->getCapabilities();
  TSM_ASSERT("this codec has some set", capabilities != 0);
  TSM_ASSERT("should have delay set", 
      codec->hasCapability(Codec::CAP_DELAY));
}

void
CodecTest :: testGetSupportedVideoFramRates()
{
  codec = Codec::findEncodingCodecByName("mpeg2video");
  TSM_ASSERT("got codec", codec);
  int32_t numFrameRates = codec->getNumSupportedVideoFrameRates();
  TSM_ASSERT("should be more than one", numFrameRates > 0);
  for(int i = 0; i < numFrameRates; i++)
  {
    RefPointer<Rational> rate = codec->getSupportedVideoFrameRate(i);
    TSM_ASSERT("should be non null", rate);
    TSM_ASSERT("should be valid number", rate->getDouble());
  }
  TSM_ASSERT("should fail quietly",
      0 == codec->getSupportedVideoFrameRate(-1));
  TSM_ASSERT("should fail quietly",
      0 == codec->getSupportedVideoFrameRate(0x7FFFFFFF));
  
}

void
CodecTest :: testGetSupportedVideoPixelFormats()
{
  codec = Codec::findEncodingCodecByName("ffvhuff");
  TSM_ASSERT("got codec", codec);
  int32_t num= codec->getNumSupportedVideoPixelFormats();
  TSM_ASSERT("should be more than one", num > 0);
  for(int i = 0; i < num; i++)
  {
    PixelFormat::Type type = codec->getSupportedVideoPixelFormat(i);
    TSM_ASSERT("should be non null", type != PixelFormat::PIX_FMT_NONE);
  }
  TSM_ASSERT("should fail quietly",
      PixelFormat::PIX_FMT_NONE == codec->getSupportedVideoPixelFormat(-1));
  TSM_ASSERT("should fail quietly",
      PixelFormat::PIX_FMT_NONE == codec->getSupportedVideoPixelFormat(0x7FFFFFFF));
}

void
CodecTest :: testGetSupportedAudioSampleRates()
{
  codec = Codec::findEncodingCodecByName("nellymoser");
  TSM_ASSERT("got codec", codec);
  int32_t num= codec->getNumSupportedAudioSampleRates();
  TSM_ASSERT("no one supports this yet", num == 0);
  TSM_ASSERT("should fail quietly",
      codec->getSupportedAudioSampleRate(-1)==0);
  TSM_ASSERT("should fail quietly", 
      codec->getSupportedAudioSampleRate(0x7FFFFFFF)==0);
}

void
CodecTest :: testGetSupportedAudioSampleFormats()
{
  codec = Codec::findDecodingCodecByName("aac");
  TSM_ASSERT("got codec", codec);
  int32_t num= codec->getNumSupportedAudioFormats();
  TSM_ASSERT("should be more than none", num > 0);
  for(int i = 0; i < num; i++)
  {
    AudioFormat::Type fmt = codec->getSupportedAudioFormat(i);
    TSM_ASSERT("should be non null", fmt != AudioFormat::SAMPLE_FMT_NONE);
  }
  TSM_ASSERT("should fail quietly",
      AudioFormat::SAMPLE_FMT_NONE == codec->getSupportedAudioFormat(-1));
  TSM_ASSERT("should fail quietly",
      AudioFormat::SAMPLE_FMT_NONE == codec->getSupportedAudioFormat(0x7FFFFFFF));
}

void
CodecTest :: testGetSupportedAudioChannelLayouts()
{
  codec = Codec::findEncodingCodecByName("ac3");
  TSM_ASSERT("got codec", codec);
  int32_t num= codec->getNumSupportedAudioChannelLayouts();
  TSM_ASSERT("should be more than none", num > 0);
  for(int i = 0; i < num; i++)
  {
    int64_t layout = codec->getSupportedAudioChannelLayout(i);
    TSM_ASSERT("should be non null", 0 != layout);
  }
  TSM_ASSERT("should fail quietly",
      0 == codec->getSupportedAudioChannelLayout(-1));
  TSM_ASSERT("should fail quietly",
      0 == codec->getSupportedAudioChannelLayout(0x7FFFFFFF));
}

void
CodecTest::testGetSupportedProfiles()
{
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_INFO, false);

  codec = Codec::findDecodingCodec(Codec::CODEC_ID_MPEG4);
  TS_ASSERT(codec);
  int32_t num = codec->getNumSupportedProfiles();
  TS_ASSERT(num > 0);
  for (int i = 0; i < num; i++) {
    RefPointer<CodecProfile> p = codec->getSupportedProfile(i);
    TS_ASSERT(p);
    VS_LOG_DEBUG("Profile: %s", p->getName());
  }
  TS_ASSERT(!codec->getSupportedProfile(-1));
  TS_ASSERT(!codec->getSupportedProfile(0x7FFFFFFF));
}

void
CodecTest :: testEncodePCM()
{
  codec = Codec::findEncodingCodecByName("pcm_s16le");
  TSM_ASSERT("", codec);
  TSM_ASSERT("", codec->canEncode());
}
