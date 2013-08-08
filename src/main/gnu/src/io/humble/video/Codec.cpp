/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Global.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/SinkFormat.h>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video
{
  using namespace io::humble::ferry;

  Codec::Codec()
  {
    VS_LOG_TRACE("Starting");
    mCodec = 0;
  }

  Codec::~Codec()
  {
    // no memory freeing required.
    mCodec = 0;
  }

  const char *
  Codec::getName() {
    const char * retval = 0;
    if (mCodec)
      retval = mCodec->name;
    return retval;
  }

  int
  Codec::getIDAsInt()
  {
    int retval = CODEC_ID_NONE;
    if (mCodec)
      retval = mCodec->id;
    return retval;
  }

  MediaDescriptor::Type
  Codec::getType()
  {
    MediaDescriptor::Type retval = (MediaDescriptor::Type) AVMEDIA_TYPE_UNKNOWN;
    if (mCodec)
      retval = (MediaDescriptor::Type) mCodec->type;
    return retval;
  }

  Codec *
  Codec::make(const AVCodec *aCodec)
  {
    Global::init();
    Codec *retval = 0;
    if (aCodec)
    {
      retval = Codec::make();
      if (retval)
        retval->mCodec = aCodec;
    }
    return retval;
  }

  Codec *
  Codec::findEncodingCodec(const Codec::ID id)
  {
    Global::init();
    return Codec::findEncodingCodecByIntID((const int)id);
  }
  Codec *
  Codec::findEncodingCodecByIntID(const int id)
  {
    Global::init();
    Codec *retval = 0;
    AVCodec *codec = 0;
    enum AVCodecID ffmpeg_id = (enum AVCodecID) id;
    Global::lock();
    codec = avcodec_find_encoder(ffmpeg_id);
    Global::unlock();
    if (codec)
      retval = Codec::make(codec);

    return retval;
  }

  Codec *
  Codec::findEncodingCodecByName(const char* name)
  {
    Global::init();
    Codec *retval = 0;
    AVCodec *codec = 0;
    if (name && *name)
    {
      Global::lock();
      codec = avcodec_find_encoder_by_name(name);
      Global::unlock();
      if (codec)
        retval = Codec::make(codec);
    }
    return retval;
  }

  Codec *
  Codec::findDecodingCodec(const Codec::ID id)
  {
    Global::init();
    return Codec::findDecodingCodecByIntID((const int)id);
  }

  Codec *
  Codec::findDecodingCodecByIntID(const int id)
  {
    Global::init();
    Codec *retval = 0;
    AVCodec *codec = 0;

    Global::lock();
    codec = avcodec_find_decoder((enum AVCodecID) id);
    Global::unlock();
    
    if (codec)
      retval = Codec::make(codec);
    return retval;
  }

  Codec *
  Codec::findDecodingCodecByName(const char* name)
  {
    Global::init();
    Codec *retval = 0;
    AVCodec *codec = 0;
    if (name && *name)
    {
      Global::lock();
      codec = avcodec_find_decoder_by_name(name);
      Global::unlock();
      if (codec)
        retval = Codec::make(codec);
    }
    return retval;
  }
  bool
  Codec::canDecode()
  {
    return av_codec_is_decoder(mCodec);
  }

  bool
  Codec::canEncode()
  {
    return av_codec_is_encoder(mCodec);
  }

  Codec*
  Codec::guessEncodingCodec(MuxerFormat* pFmt,
      const char* shortName,
      const char* url,
      const char* mimeType,
      MediaDescriptor::Type type)
  {
    Global::init();
    Codec* retval = 0;
    RefPointer<MuxerFormat> fmt = 0;
    AVOutputFormat * oFmt = 0;

    // We acquire here because the RefPointer always
    // releases.
    fmt.reset(dynamic_cast<MuxerFormat*>(pFmt), true);

    if (!fmt)
    {
      fmt = MuxerFormat::guessFormat(shortName, url, mimeType);
    }
    if (fmt)
      oFmt = fmt->getCtx();

    // Make sure at least one in put is specified.
    // The av_guess_codec function REQUIRES a
    // non null AVOutputFormat
    // It also examines url with a strcmp(), so let's
    // give it a zero-length string.
    // In reality, it ignores the other params.
    if (!url)
      url = "";

    if (oFmt)
    {
      enum AVCodecID id = av_guess_codec(oFmt, shortName, url,
          mimeType, (enum AVMediaType) type);
      retval = Codec::findEncodingCodecByIntID((int)id);
    }
    return retval;
  }

  const char *
  Codec::getLongName()
  {
    return mCodec->long_name;
  }

  int32_t
  Codec::getCapabilities()
  {
    return mCodec->capabilities;
  }
  bool
  Codec::hasCapability(CodecCapability flag)
  {
    return mCodec->capabilities&flag;
  }

  int32_t
  Codec::getNumSupportedVideoFrameRates()
  {
    int count = 0;
    for(
        const AVRational* p=mCodec->supported_framerates;
        p && !(!p->den && !p->num);
        p++
    )
      ++count;
    return count;
  }
  Rational*
  Codec::getSupportedVideoFrameRate(int32_t index)
  {
    int i = 0;
    for(
        const AVRational* p=mCodec->supported_framerates;
        p && !(!p->den && !p->num);
        p++, i++
    )
      if (index == i)
        return Rational::make(p->num,p->den);
    return 0;
  }

  int32_t
  Codec::getNumSupportedVideoPixelFormats()
  {
    if (!mCodec)
      return 0;
    int count = 0;
    for(const enum AVPixelFormat* p = mCodec->pix_fmts;
    p && (*p!=-1);
    p++)
      ++count;
    return count;

  }

  PixelFormat::Type
  Codec::getSupportedVideoPixelFormat(int32_t index)
  {
    int i = 0;
    for(const enum AVPixelFormat* p = mCodec->pix_fmts;
    p && (*p!=-1);
    p++,i++)
      if (index == i)
        return (PixelFormat::Type)*p;
    return PixelFormat::PIX_FMT_NONE;
  }

  int32_t
  Codec::getNumSupportedAudioSampleRates()
  {
    int i = 0;
    for(const int *p = mCodec->supported_samplerates;
    p && *p;
    ++p, ++i)
      ;
    return i;
  }
  int32_t
  Codec::getSupportedAudioSampleRate(int32_t index)
  {
    int i = 0;
    for(const int *p = mCodec->supported_samplerates;
    p && *p;
    p++, i++)
      if (i == index)
        return *p;
    return 0;
  }

  int32_t
  Codec::getNumSupportedAudioFormats()
  {
    int i = 0;
    for(const enum AVSampleFormat* p=mCodec->sample_fmts;
      p && (*p != -1);
      p++,i++)
      ;
    return i;
  }
  
  AudioFormat::Type
  Codec::getSupportedAudioFormat(int32_t index)
  {
    int i = 0;
    for(const enum AVSampleFormat* p=mCodec->sample_fmts;
      p && (*p != AV_SAMPLE_FMT_NONE);
      p++,i++)
      if (index == i)
        return (AudioFormat::Type)*p;
    return AudioFormat::SAMPLE_FMT_NONE;
    
  }

  int32_t
  Codec::getNumSupportedAudioChannelLayouts()
  {
    int i = 0;
    for(const uint64_t* p=mCodec->channel_layouts;
      p && *p;
      p++,i++)
      ;
    return i;
  }
  AudioChannel::Layout
  Codec::getSupportedAudioChannelLayout(int32_t index)
  {
    int i = 0;
    for(const uint64_t* p=mCodec->channel_layouts;
      p && *p;
      p++,i++)
      if (index == i)
        return (AudioChannel::Layout)*p;
    return AudioChannel::CH_LAYOUT_UNKNOWN;
  }

  int32_t
  Codec::getNumSupportedProfiles()
  {
    int i = 0;
    for(const AVProfile* p=mCodec->profiles;
      p && p->profile != FF_PROFILE_UNKNOWN;
      p++,i++)
      ;
    return i;
  }
  CodecProfile*
  Codec::getSupportedProfile(int32_t index)
  {
    int i = 0;
    for(const AVProfile* p=mCodec->profiles;
      p && p->profile != FF_PROFILE_UNKNOWN;
      p++,i++)
      if (index == i)
        return CodecProfile::make(p);
    return 0;
  }

  int32_t
  Codec::getNumInstalledCodecs()
  {
    Global::init();
    int32_t numInstalledCodecs = 0;
    AVCodec * codec = 0;
    while((codec = av_codec_next(codec))!=0)
      ++numInstalledCodecs;
    return numInstalledCodecs;
  }

  Codec*
  Codec::getInstalledCodec(int32_t index)
  {
    Global::init();
    if (index < 0)
      return 0;

    AVCodec * codec = 0;
    for(int32_t i = 0; (codec = av_codec_next(codec))!=0; i++)
    {
      if (i == index)
        return Codec::make(codec);
    }
    return 0;
  }

  Codec::ID
  Codec::getID()
  {
    // Warning; this might not be protable
    ID retval = CODEC_ID_NONE;
    int id = getIDAsInt();

    // This cast is not defined in C++ when id is
    // not in ID.  That means if we're using a
    // newer version of LIBAVCODEC this might
    // give a bogus value for a Codec we don't
    // know about.
    retval = (ID)id;
    if (id != (int) retval)
    {
      // we assume some back and forth conversion failed...
      retval = CODEC_ID_NONE;
    }
    return retval;
  }




}}}
