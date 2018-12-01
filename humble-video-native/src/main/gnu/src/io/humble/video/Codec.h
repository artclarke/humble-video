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

#ifndef CODEC_H_
#define CODEC_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/PixelFormat.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/Rational.h>

namespace io {
namespace humble {
namespace video {

class MuxerFormat;
/**
 * A descriptor for different types of media that can be handled
 * by Humble Video.
 */
class VS_API_HUMBLEVIDEO MediaDescriptor : public io::humble::ferry::RefCounted
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(MediaDescriptor)
public:
  /**
   * The type of media contained in a container's stream
   */
  typedef enum Type
  {
    /** Usually treated as DATA */
    MEDIA_UNKNOWN = AVMEDIA_TYPE_UNKNOWN,
    /** Video data */
    MEDIA_VIDEO = AVMEDIA_TYPE_VIDEO,
    /** Audio data */
    MEDIA_AUDIO = AVMEDIA_TYPE_AUDIO,
    /** Opaque data, usually continuous */
    MEDIA_DATA = AVMEDIA_TYPE_DATA,
    /** Subtitle data */
    MEDIA_SUBTITLE = AVMEDIA_TYPE_SUBTITLE,
    /** Opaque data, usually sparse */
    MEDIA_ATTACHMENT = AVMEDIA_TYPE_ATTACHMENT,
#ifndef SWIG
    MEDIA_LAST_TYPE = AVMEDIA_TYPE_NB
#endif
  } Type;

private:
  MediaDescriptor ()
  {
  }
  virtual
  ~MediaDescriptor ()
  {
  }
};

/**
 * A profile supported by a Codec.
 */
class VS_API_HUMBLEVIDEO CodecProfile : public io::humble::ferry::RefCounted
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(CodecProfile)
public:
  /** An enum of known about Profiles. Note these
   * constants are not guaranteed to remain accessible
   * through releases, and so should be only used
   * to display meta information.
   */
  typedef enum ProfileType
  {
    PROFILE_UNKNOWN = FF_PROFILE_UNKNOWN,
    PROFILE_RESERVED = FF_PROFILE_RESERVED,

    PROFILE_AAC_MAIN = FF_PROFILE_AAC_MAIN,
    PROFILE_AAC_LOW = FF_PROFILE_AAC_LOW,
    PROFILE_AAC_SSR = FF_PROFILE_AAC_SSR,
    PROFILE_AAC_LTP = FF_PROFILE_AAC_LTP,
    PROFILE_AAC_HE = FF_PROFILE_AAC_HE,
    PROFILE_AAC_HE_V2 = FF_PROFILE_AAC_HE_V2,
    PROFILE_AAC_LD = FF_PROFILE_AAC_LD,
    PROFILE_AAC_ELD = FF_PROFILE_AAC_ELD,
    PROFILE_MPEG2_AAC_LOW = FF_PROFILE_MPEG2_AAC_LOW,
    PROFILE_MPEG2_AAC_HE = FF_PROFILE_MPEG2_AAC_HE,

    PROFILE_DTS = FF_PROFILE_DTS,
    PROFILE_DTS_ES = FF_PROFILE_DTS_ES,
    PROFILE_DTS_96_24 = FF_PROFILE_DTS_96_24,
    PROFILE_DTS_HD_HRA = FF_PROFILE_DTS_HD_HRA,
    PROFILE_DTS_HD_MA = FF_PROFILE_DTS_HD_MA,

    PROFILE_MPEG2_422 = FF_PROFILE_MPEG2_422,
    PROFILE_MPEG2_HIGH = FF_PROFILE_MPEG2_HIGH,
    PROFILE_MPEG2_SS = FF_PROFILE_MPEG2_SS,
    PROFILE_MPEG2_SNR_SCALABLE = FF_PROFILE_MPEG2_SNR_SCALABLE,
    PROFILE_MPEG2_MAIN = FF_PROFILE_MPEG2_MAIN,
    PROFILE_MPEG2_SIMPLE = FF_PROFILE_MPEG2_SIMPLE,

    PROFILE_H264_CONSTRAINED = FF_PROFILE_H264_CONSTRAINED,
    PROFILE_H264_INTRA = FF_PROFILE_H264_INTRA,

    PROFILE_H264_BASELINE = FF_PROFILE_H264_BASELINE,
    PROFILE_H264_CONSTRAINED_BASELINE = FF_PROFILE_H264_CONSTRAINED_BASELINE,
    PROFILE_H264_MAIN = FF_PROFILE_H264_MAIN,
    PROFILE_H264_EXTENDED = FF_PROFILE_H264_EXTENDED,
    PROFILE_H264_HIGH = FF_PROFILE_H264_HIGH,
    PROFILE_H264_HIGH_10 = FF_PROFILE_H264_HIGH_10,
    PROFILE_H264_HIGH_10_INTRA = FF_PROFILE_H264_HIGH_10_INTRA,
    PROFILE_H264_HIGH_422 = FF_PROFILE_H264_HIGH_422,
    PROFILE_H264_HIGH_422_INTRA = FF_PROFILE_H264_HIGH_422_INTRA,
    PROFILE_H264_HIGH_444 = FF_PROFILE_H264_HIGH_444,
    PROFILE_H264_HIGH_444_PREDICTIVE = FF_PROFILE_H264_HIGH_444_PREDICTIVE,
    PROFILE_H264_HIGH_444_INTRA = FF_PROFILE_H264_HIGH_444_INTRA,
    PROFILE_H264_CAVLC_444 = FF_PROFILE_H264_CAVLC_444,

    PROFILE_VC1_SIMPLE = FF_PROFILE_VC1_SIMPLE,
    PROFILE_VC1_MAIN = FF_PROFILE_VC1_MAIN,
    PROFILE_VC1_COMPLEX = FF_PROFILE_VC1_COMPLEX,
    PROFILE_VC1_ADVANCED = FF_PROFILE_VC1_ADVANCED,

    PROFILE_MPEG4_SIMPLE = FF_PROFILE_MPEG4_SIMPLE,
    PROFILE_MPEG4_SIMPLE_SCALABLE = FF_PROFILE_MPEG4_SIMPLE_SCALABLE,
    PROFILE_MPEG4_CORE = FF_PROFILE_MPEG4_CORE,
    PROFILE_MPEG4_MAIN = FF_PROFILE_MPEG4_MAIN,
    PROFILE_MPEG4_N_BIT = FF_PROFILE_MPEG4_N_BIT,
    PROFILE_MPEG4_SCALABLE_TEXTURE = FF_PROFILE_MPEG4_SCALABLE_TEXTURE,
    PROFILE_MPEG4_SIMPLE_FACE_ANIMATION = FF_PROFILE_MPEG4_SIMPLE_FACE_ANIMATION,
    PROFILE_MPEG4_BASIC_ANIMATED_TEXTURE = FF_PROFILE_MPEG4_BASIC_ANIMATED_TEXTURE,
    PROFILE_MPEG4_HYBRID = FF_PROFILE_MPEG4_HYBRID,
    PROFILE_MPEG4_ADVANCED_REAL_TIME = FF_PROFILE_MPEG4_ADVANCED_REAL_TIME,
    PROFILE_MPEG4_CORE_SCALABLE = FF_PROFILE_MPEG4_CORE_SCALABLE,
    PROFILE_MPEG4_ADVANCED_CODING = FF_PROFILE_MPEG4_ADVANCED_CODING,
    PROFILE_MPEG4_ADVANCED_CORE = FF_PROFILE_MPEG4_ADVANCED_CORE,
    PROFILE_MPEG4_ADVANCED_SCALABLE_TEXTURE = FF_PROFILE_MPEG4_ADVANCED_SCALABLE_TEXTURE,
    PROFILE_MPEG4_SIMPLE_STUDIO = FF_PROFILE_MPEG4_SIMPLE_STUDIO,
    PROFILE_MPEG4_ADVANCED_SIMPLE = FF_PROFILE_MPEG4_ADVANCED_SIMPLE,

    PROFILE_JPEG2000_CSTREAM_RESTRICTION_0 = FF_PROFILE_JPEG2000_CSTREAM_RESTRICTION_0,
    PROFILE_JPEG2000_CSTREAM_RESTRICTION_1 = FF_PROFILE_JPEG2000_CSTREAM_RESTRICTION_1,
    PROFILE_JPEG2000_CSTREAM_NO_RESTRICTION = FF_PROFILE_JPEG2000_CSTREAM_NO_RESTRICTION,
    PROFILE_JPEG2000_DCINEMA_2K = FF_PROFILE_JPEG2000_DCINEMA_2K,
    PROFILE_JPEG2000_DCINEMA_4K = FF_PROFILE_JPEG2000_DCINEMA_4K,

    PROFILE_HEVC_MAIN = FF_PROFILE_HEVC_MAIN,
    PROFILE_HEVC_MAIN_10 = FF_PROFILE_HEVC_MAIN_10,
    PROFILE_HEVC_MAIN_STILL_PICTURE = FF_PROFILE_HEVC_MAIN_STILL_PICTURE,
    PROFILE_HEVC_REXT = FF_PROFILE_HEVC_REXT,
  } ProfileType;
  /** Get the type for this profile. */
  virtual ProfileType
  getProfile() {
    return (ProfileType) mProfile->profile;
  }
  /** Get the name for this profile. */
  virtual const char *
  getName() {
    return mProfile->name;
  }
#ifndef SWIG
  static CodecProfile *
  make(const AVProfile * p) {
    CodecProfile* retval = 0;
    if (p) {
      retval = make();
      retval->mProfile = p;
    }
    return retval;
  }
#endif // ! SWIG
private:
  CodecProfile() :
      mProfile(0) {
  }
  virtual
  ~CodecProfile() {};
  const AVProfile* mProfile;
};

/**
 * A codec that can be used either to encode raw data into compressed bitstreams,
 * or decode compressed data into raw data, or both!
 */
class VS_API_HUMBLEVIDEO Codec : public io::humble::ferry::RefCounted
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(Codec)
public:

  /**
   * When decoding, you can instruct streams to discard some
   * packets. The following table specifies which one.
   * Each subsequent enum value drops more packets than the prior.
   */
  typedef enum DiscardFlag
  {
    /** Discard nothing. */
    DISCARD_NONE = AVDISCARD_NONE,
    /** Discard useless packets like 0 size packets in avi. */
    DISCARD_DEFAULT = AVDISCARD_DEFAULT,
    /** Discard all non reference */
    DISCARD_NONREF = AVDISCARD_NONREF,
    /** Discard all bidirectional frames */
    DISCARD_BIDIR = AVDISCARD_BIDIR,
    /** Discard all non intra frames */
    DISCARD_NONINTRA = AVDISCARD_NONINTRA,
    /** Discard all frames except keyframes */
    DISCARD_NONKEY = AVDISCARD_NONKEY,
    /** discard all */
    DISCARD_ALL = AVDISCARD_ALL,
  } DiscardFlag;

  /**
   * Identify the syntax and semantics of an audio or video encoded bitstream.
   * The principle is roughly:
   * Two decoders with the same ID can decode the same streams.
   * Two encoders with the same ID can encode compatible streams.
   * There may be slight deviations from the principle due to implementation
   * details.
   *
   * From Ffmpeg 4.1
   */
  typedef enum ID
  {
    CODEC_ID_NONE = AV_CODEC_ID_NONE,

    /* video codecs */
    CODEC_ID_MPEG1VIDEO = AV_CODEC_ID_MPEG1VIDEO,
    CODEC_ID_MPEG2VIDEO = AV_CODEC_ID_MPEG2VIDEO, ///< preferred ID for MPEG-1/2 video decoding
    CODEC_ID_H261 = AV_CODEC_ID_H261,
    CODEC_ID_H263 = AV_CODEC_ID_H263,
    CODEC_ID_RV10 = AV_CODEC_ID_RV10,
    CODEC_ID_RV20 = AV_CODEC_ID_RV20,
    CODEC_ID_MJPEG = AV_CODEC_ID_MJPEG,
    CODEC_ID_MJPEGB = AV_CODEC_ID_MJPEGB,
    CODEC_ID_LJPEG = AV_CODEC_ID_LJPEG,
    CODEC_ID_SP5X = AV_CODEC_ID_SP5X,
    CODEC_ID_JPEGLS = AV_CODEC_ID_JPEGLS,
    CODEC_ID_MPEG4 = AV_CODEC_ID_MPEG4,
    CODEC_ID_RAWVIDEO = AV_CODEC_ID_RAWVIDEO,
    CODEC_ID_MSMPEG4V1 = AV_CODEC_ID_MSMPEG4V1,
    CODEC_ID_MSMPEG4V2 = AV_CODEC_ID_MSMPEG4V2,
    CODEC_ID_MSMPEG4V3 = AV_CODEC_ID_MSMPEG4V3,
    CODEC_ID_WMV1 = AV_CODEC_ID_WMV1,
    CODEC_ID_WMV2 = AV_CODEC_ID_WMV2,
    CODEC_ID_H263P = AV_CODEC_ID_H263P,
    CODEC_ID_H263I = AV_CODEC_ID_H263I,
    CODEC_ID_FLV1 = AV_CODEC_ID_FLV1,
    CODEC_ID_SVQ1 = AV_CODEC_ID_SVQ1,
    CODEC_ID_SVQ3 = AV_CODEC_ID_SVQ3,
    CODEC_ID_DVVIDEO = AV_CODEC_ID_DVVIDEO,
    CODEC_ID_HUFFYUV = AV_CODEC_ID_HUFFYUV,
    CODEC_ID_CYUV = AV_CODEC_ID_CYUV,
    CODEC_ID_H264 = AV_CODEC_ID_H264,
    CODEC_ID_INDEO3 = AV_CODEC_ID_INDEO3,
    CODEC_ID_VP3 = AV_CODEC_ID_VP3,
    CODEC_ID_THEORA = AV_CODEC_ID_THEORA,
    CODEC_ID_ASV1 = AV_CODEC_ID_ASV1,
    CODEC_ID_ASV2 = AV_CODEC_ID_ASV2,
    CODEC_ID_FFV1 = AV_CODEC_ID_FFV1,
    CODEC_ID_4XM = AV_CODEC_ID_4XM,
    CODEC_ID_VCR1 = AV_CODEC_ID_VCR1,
    CODEC_ID_CLJR = AV_CODEC_ID_CLJR,
    CODEC_ID_MDEC = AV_CODEC_ID_MDEC,
    CODEC_ID_ROQ = AV_CODEC_ID_ROQ,
    CODEC_ID_INTERPLAY_VIDEO = AV_CODEC_ID_INTERPLAY_VIDEO,
    CODEC_ID_XAN_WC3 = AV_CODEC_ID_XAN_WC3,
    CODEC_ID_XAN_WC4 = AV_CODEC_ID_XAN_WC4,
    CODEC_ID_RPZA = AV_CODEC_ID_RPZA,
    CODEC_ID_CINEPAK = AV_CODEC_ID_CINEPAK,
    CODEC_ID_WS_VQA = AV_CODEC_ID_WS_VQA,
    CODEC_ID_MSRLE = AV_CODEC_ID_MSRLE,
    CODEC_ID_MSVIDEO1 = AV_CODEC_ID_MSVIDEO1,
    CODEC_ID_IDCIN = AV_CODEC_ID_IDCIN,
    CODEC_ID_8BPS = AV_CODEC_ID_8BPS,
    CODEC_ID_SMC = AV_CODEC_ID_SMC,
    CODEC_ID_FLIC = AV_CODEC_ID_FLIC,
    CODEC_ID_TRUEMOTION1 = AV_CODEC_ID_TRUEMOTION1,
    CODEC_ID_VMDVIDEO = AV_CODEC_ID_VMDVIDEO,
    CODEC_ID_MSZH = AV_CODEC_ID_MSZH,
    CODEC_ID_ZLIB = AV_CODEC_ID_ZLIB,
    CODEC_ID_QTRLE = AV_CODEC_ID_QTRLE,
    CODEC_ID_TSCC = AV_CODEC_ID_TSCC,
    CODEC_ID_ULTI = AV_CODEC_ID_ULTI,
    CODEC_ID_QDRAW = AV_CODEC_ID_QDRAW,
    CODEC_ID_VIXL = AV_CODEC_ID_VIXL,
    CODEC_ID_QPEG = AV_CODEC_ID_QPEG,
    CODEC_ID_PNG = AV_CODEC_ID_PNG,
    CODEC_ID_PPM = AV_CODEC_ID_PPM,
    CODEC_ID_PBM = AV_CODEC_ID_PBM,
    CODEC_ID_PGM = AV_CODEC_ID_PGM,
    CODEC_ID_PGMYUV = AV_CODEC_ID_PGMYUV,
    CODEC_ID_PAM = AV_CODEC_ID_PAM,
    CODEC_ID_FFVHUFF = AV_CODEC_ID_FFVHUFF,
    CODEC_ID_RV30 = AV_CODEC_ID_RV30,
    CODEC_ID_RV40 = AV_CODEC_ID_RV40,
    CODEC_ID_VC1 = AV_CODEC_ID_VC1,
    CODEC_ID_WMV3 = AV_CODEC_ID_WMV3,
    CODEC_ID_LOCO = AV_CODEC_ID_LOCO,
    CODEC_ID_WNV1 = AV_CODEC_ID_WNV1,
    CODEC_ID_AASC = AV_CODEC_ID_AASC,
    CODEC_ID_INDEO2 = AV_CODEC_ID_INDEO2,
    CODEC_ID_FRAPS = AV_CODEC_ID_FRAPS,
    CODEC_ID_TRUEMOTION2 = AV_CODEC_ID_TRUEMOTION2,
    CODEC_ID_BMP = AV_CODEC_ID_BMP,
    CODEC_ID_CSCD = AV_CODEC_ID_CSCD,
    CODEC_ID_MMVIDEO = AV_CODEC_ID_MMVIDEO,
    CODEC_ID_ZMBV = AV_CODEC_ID_ZMBV,
    CODEC_ID_AVS = AV_CODEC_ID_AVS,
    CODEC_ID_SMACKVIDEO = AV_CODEC_ID_SMACKVIDEO,
    CODEC_ID_NUV = AV_CODEC_ID_NUV,
    CODEC_ID_KMVC = AV_CODEC_ID_KMVC,
    CODEC_ID_FLASHSV = AV_CODEC_ID_FLASHSV,
    CODEC_ID_CAVS = AV_CODEC_ID_CAVS,
    CODEC_ID_JPEG2000 = AV_CODEC_ID_JPEG2000,
    CODEC_ID_VMNC = AV_CODEC_ID_VMNC,
    CODEC_ID_VP5 = AV_CODEC_ID_VP5,
    CODEC_ID_VP6 = AV_CODEC_ID_VP6,
    CODEC_ID_VP6F = AV_CODEC_ID_VP6F,
    CODEC_ID_TARGA = AV_CODEC_ID_TARGA,
    CODEC_ID_DSICINVIDEO = AV_CODEC_ID_DSICINVIDEO,
    CODEC_ID_TIERTEXSEQVIDEO = AV_CODEC_ID_TIERTEXSEQVIDEO,
    CODEC_ID_TIFF = AV_CODEC_ID_TIFF,
    CODEC_ID_GIF = AV_CODEC_ID_GIF,
    CODEC_ID_DXA = AV_CODEC_ID_DXA,
    CODEC_ID_DNXHD = AV_CODEC_ID_DNXHD,
    CODEC_ID_THP = AV_CODEC_ID_THP,
    CODEC_ID_SGI = AV_CODEC_ID_SGI,
    CODEC_ID_C93 = AV_CODEC_ID_C93,
    CODEC_ID_BETHSOFTVID = AV_CODEC_ID_BETHSOFTVID,
    CODEC_ID_PTX = AV_CODEC_ID_PTX,
    CODEC_ID_TXD = AV_CODEC_ID_TXD,
    CODEC_ID_VP6A = AV_CODEC_ID_VP6A,
    CODEC_ID_AMV = AV_CODEC_ID_AMV,
    CODEC_ID_VB = AV_CODEC_ID_VB,
    CODEC_ID_PCX = AV_CODEC_ID_PCX,
    CODEC_ID_SUNRAST = AV_CODEC_ID_SUNRAST,
    CODEC_ID_INDEO4 = AV_CODEC_ID_INDEO4,
    CODEC_ID_INDEO5 = AV_CODEC_ID_INDEO5,
    CODEC_ID_MIMIC = AV_CODEC_ID_MIMIC,
    CODEC_ID_RL2 = AV_CODEC_ID_RL2,
    CODEC_ID_ESCAPE124 = AV_CODEC_ID_ESCAPE124,
    CODEC_ID_DIRAC = AV_CODEC_ID_DIRAC,
    CODEC_ID_BFI = AV_CODEC_ID_BFI,
    CODEC_ID_CMV = AV_CODEC_ID_CMV,
    CODEC_ID_MOTIONPIXELS = AV_CODEC_ID_MOTIONPIXELS,
    CODEC_ID_TGV = AV_CODEC_ID_TGV,
    CODEC_ID_TGQ = AV_CODEC_ID_TGQ,
    CODEC_ID_TQI = AV_CODEC_ID_TQI,
    CODEC_ID_AURA = AV_CODEC_ID_AURA,
    CODEC_ID_AURA2 = AV_CODEC_ID_AURA2,
    CODEC_ID_V210X = AV_CODEC_ID_V210X,
    CODEC_ID_TMV = AV_CODEC_ID_TMV,
    CODEC_ID_V210 = AV_CODEC_ID_V210,
    CODEC_ID_DPX = AV_CODEC_ID_DPX,
    CODEC_ID_MAD = AV_CODEC_ID_MAD,
    CODEC_ID_FRWU = AV_CODEC_ID_FRWU,
    CODEC_ID_FLASHSV2 = AV_CODEC_ID_FLASHSV2,
    CODEC_ID_CDGRAPHICS = AV_CODEC_ID_CDGRAPHICS,
    CODEC_ID_R210 = AV_CODEC_ID_R210,
    CODEC_ID_ANM = AV_CODEC_ID_ANM,
    CODEC_ID_BINKVIDEO = AV_CODEC_ID_BINKVIDEO,
    CODEC_ID_IFF_ILBM = AV_CODEC_ID_IFF_ILBM,
#define AV_CODEC_ID_IFF_BYTERUN1 AV_CODEC_ID_IFF_ILBM
    CODEC_ID_KGV1 = AV_CODEC_ID_KGV1,
    CODEC_ID_YOP = AV_CODEC_ID_YOP,
    CODEC_ID_VP8 = AV_CODEC_ID_VP8,
    CODEC_ID_PICTOR = AV_CODEC_ID_PICTOR,
    CODEC_ID_ANSI = AV_CODEC_ID_ANSI,
    CODEC_ID_A64_MULTI = AV_CODEC_ID_A64_MULTI,
    CODEC_ID_A64_MULTI5 = AV_CODEC_ID_A64_MULTI5,
    CODEC_ID_R10K = AV_CODEC_ID_R10K,
    CODEC_ID_MXPEG = AV_CODEC_ID_MXPEG,
    CODEC_ID_LAGARITH = AV_CODEC_ID_LAGARITH,
    CODEC_ID_PRORES = AV_CODEC_ID_PRORES,
    CODEC_ID_JV = AV_CODEC_ID_JV,
    CODEC_ID_DFA = AV_CODEC_ID_DFA,
    CODEC_ID_WMV3IMAGE = AV_CODEC_ID_WMV3IMAGE,
    CODEC_ID_VC1IMAGE = AV_CODEC_ID_VC1IMAGE,
    CODEC_ID_UTVIDEO = AV_CODEC_ID_UTVIDEO,
    CODEC_ID_BMV_VIDEO = AV_CODEC_ID_BMV_VIDEO,
    CODEC_ID_VBLE = AV_CODEC_ID_VBLE,
    CODEC_ID_DXTORY = AV_CODEC_ID_DXTORY,
    CODEC_ID_V410 = AV_CODEC_ID_V410,
    CODEC_ID_XWD = AV_CODEC_ID_XWD,
    CODEC_ID_CDXL = AV_CODEC_ID_CDXL,
    CODEC_ID_XBM = AV_CODEC_ID_XBM,
    CODEC_ID_ZEROCODEC = AV_CODEC_ID_ZEROCODEC,
    CODEC_ID_MSS1 = AV_CODEC_ID_MSS1,
    CODEC_ID_MSA1 = AV_CODEC_ID_MSA1,
    CODEC_ID_TSCC2 = AV_CODEC_ID_TSCC2,
    CODEC_ID_MTS2 = AV_CODEC_ID_MTS2,
    CODEC_ID_CLLC = AV_CODEC_ID_CLLC,
    CODEC_ID_MSS2 = AV_CODEC_ID_MSS2,
    CODEC_ID_VP9 = AV_CODEC_ID_VP9,
    CODEC_ID_AIC = AV_CODEC_ID_AIC,
    CODEC_ID_ESCAPE130 = AV_CODEC_ID_ESCAPE130,
    CODEC_ID_G2M = AV_CODEC_ID_G2M,
    CODEC_ID_WEBP = AV_CODEC_ID_WEBP,
    CODEC_ID_HNM4_VIDEO = AV_CODEC_ID_HNM4_VIDEO,
    CODEC_ID_HEVC = AV_CODEC_ID_HEVC,
#define AV_CODEC_ID_H265 AV_CODEC_ID_HEVC
    CODEC_ID_FIC = AV_CODEC_ID_FIC,
    CODEC_ID_ALIAS_PIX = AV_CODEC_ID_ALIAS_PIX,
    CODEC_ID_BRENDER_PIX = AV_CODEC_ID_BRENDER_PIX,
    CODEC_ID_PAF_VIDEO = AV_CODEC_ID_PAF_VIDEO,
    CODEC_ID_EXR = AV_CODEC_ID_EXR,
    CODEC_ID_VP7 = AV_CODEC_ID_VP7,
    CODEC_ID_SANM = AV_CODEC_ID_SANM,
    CODEC_ID_SGIRLE = AV_CODEC_ID_SGIRLE,
    CODEC_ID_MVC1 = AV_CODEC_ID_MVC1,
    CODEC_ID_MVC2 = AV_CODEC_ID_MVC2,
    CODEC_ID_HQX = AV_CODEC_ID_HQX,
    CODEC_ID_TDSC = AV_CODEC_ID_TDSC,
    CODEC_ID_HQ_HQA = AV_CODEC_ID_HQ_HQA,
    CODEC_ID_HAP = AV_CODEC_ID_HAP,
    CODEC_ID_DDS = AV_CODEC_ID_DDS,
    CODEC_ID_DXV = AV_CODEC_ID_DXV,
    CODEC_ID_SCREENPRESSO = AV_CODEC_ID_SCREENPRESSO,
    CODEC_ID_RSCC = AV_CODEC_ID_RSCC,
    CODEC_ID_AVS2 = AV_CODEC_ID_AVS2,

    CODEC_ID_Y41P = AV_CODEC_ID_Y41P,
    CODEC_ID_AVRP = AV_CODEC_ID_AVRP,
    CODEC_ID_012V = AV_CODEC_ID_012V,
    CODEC_ID_AVUI = AV_CODEC_ID_AVUI,
    CODEC_ID_AYUV = AV_CODEC_ID_AYUV,
    CODEC_ID_TARGA_Y216 = AV_CODEC_ID_TARGA_Y216,
    CODEC_ID_V308 = AV_CODEC_ID_V308,
    CODEC_ID_V408 = AV_CODEC_ID_V408,
    CODEC_ID_YUV4 = AV_CODEC_ID_YUV4,
    CODEC_ID_AVRN = AV_CODEC_ID_AVRN,
    CODEC_ID_CPIA = AV_CODEC_ID_CPIA,
    CODEC_ID_XFACE = AV_CODEC_ID_XFACE,
    CODEC_ID_SNOW = AV_CODEC_ID_SNOW,
    CODEC_ID_SMVJPEG = AV_CODEC_ID_SMVJPEG,
    CODEC_ID_APNG = AV_CODEC_ID_APNG,
    CODEC_ID_DAALA = AV_CODEC_ID_DAALA,
    CODEC_ID_CFHD = AV_CODEC_ID_CFHD,
    CODEC_ID_TRUEMOTION2RT = AV_CODEC_ID_TRUEMOTION2RT,
    CODEC_ID_M101 = AV_CODEC_ID_M101,
    CODEC_ID_MAGICYUV = AV_CODEC_ID_MAGICYUV,
    CODEC_ID_SHEERVIDEO = AV_CODEC_ID_SHEERVIDEO,
    CODEC_ID_YLC = AV_CODEC_ID_YLC,
    CODEC_ID_PSD = AV_CODEC_ID_PSD,
    CODEC_ID_PIXLET = AV_CODEC_ID_PIXLET,
    CODEC_ID_SPEEDHQ = AV_CODEC_ID_SPEEDHQ,
    CODEC_ID_FMVC = AV_CODEC_ID_FMVC,
    CODEC_ID_SCPR = AV_CODEC_ID_SCPR,
    CODEC_ID_CLEARVIDEO = AV_CODEC_ID_CLEARVIDEO,
    CODEC_ID_XPM = AV_CODEC_ID_XPM,
    CODEC_ID_AV1 = AV_CODEC_ID_AV1,
    CODEC_ID_BITPACKED = AV_CODEC_ID_BITPACKED,
    CODEC_ID_MSCC = AV_CODEC_ID_MSCC,
    CODEC_ID_SRGC = AV_CODEC_ID_SRGC,
    CODEC_ID_SVG = AV_CODEC_ID_SVG,
    CODEC_ID_GDV = AV_CODEC_ID_GDV,
    CODEC_ID_FITS = AV_CODEC_ID_FITS,
    CODEC_ID_IMM4 = AV_CODEC_ID_IMM4,
    CODEC_ID_PROSUMER = AV_CODEC_ID_PROSUMER,
    CODEC_ID_MWSC = AV_CODEC_ID_MWSC,
    CODEC_ID_WCMV = AV_CODEC_ID_WCMV,
    CODEC_ID_RASC = AV_CODEC_ID_RASC,

    /* various PCM "codecs" */
    CODEC_ID_FIRST_AUDIO = AV_CODEC_ID_FIRST_AUDIO,     ///< A dummy id pointing at the start of audio codecs
    CODEC_ID_PCM_S16LE = AV_CODEC_ID_PCM_S16LE,
    CODEC_ID_PCM_S16BE = AV_CODEC_ID_PCM_S16BE,
    CODEC_ID_PCM_U16LE = AV_CODEC_ID_PCM_U16LE,
    CODEC_ID_PCM_U16BE = AV_CODEC_ID_PCM_U16BE,
    CODEC_ID_PCM_S8 = AV_CODEC_ID_PCM_S8,
    CODEC_ID_PCM_U8 = AV_CODEC_ID_PCM_U8,
    CODEC_ID_PCM_MULAW = AV_CODEC_ID_PCM_MULAW,
    CODEC_ID_PCM_ALAW = AV_CODEC_ID_PCM_ALAW,
    CODEC_ID_PCM_S32LE = AV_CODEC_ID_PCM_S32LE,
    CODEC_ID_PCM_S32BE = AV_CODEC_ID_PCM_S32BE,
    CODEC_ID_PCM_U32LE = AV_CODEC_ID_PCM_U32LE,
    CODEC_ID_PCM_U32BE = AV_CODEC_ID_PCM_U32BE,
    CODEC_ID_PCM_S24LE = AV_CODEC_ID_PCM_S24LE,
    CODEC_ID_PCM_S24BE = AV_CODEC_ID_PCM_S24BE,
    CODEC_ID_PCM_U24LE = AV_CODEC_ID_PCM_U24LE,
    CODEC_ID_PCM_U24BE = AV_CODEC_ID_PCM_U24BE,
    CODEC_ID_PCM_S24DAUD = AV_CODEC_ID_PCM_S24DAUD,
    CODEC_ID_PCM_ZORK = AV_CODEC_ID_PCM_ZORK,
    CODEC_ID_PCM_S16LE_PLANAR = AV_CODEC_ID_PCM_S16LE_PLANAR,
    CODEC_ID_PCM_DVD = AV_CODEC_ID_PCM_DVD,
    CODEC_ID_PCM_F32BE = AV_CODEC_ID_PCM_F32BE,
    CODEC_ID_PCM_F32LE = AV_CODEC_ID_PCM_F32LE,
    CODEC_ID_PCM_F64BE = AV_CODEC_ID_PCM_F64BE,
    CODEC_ID_PCM_F64LE = AV_CODEC_ID_PCM_F64LE,
    CODEC_ID_PCM_BLURAY = AV_CODEC_ID_PCM_BLURAY,
    CODEC_ID_PCM_LXF = AV_CODEC_ID_PCM_LXF,
    CODEC_ID_S302M = AV_CODEC_ID_S302M,
    CODEC_ID_PCM_S8_PLANAR = AV_CODEC_ID_PCM_S8_PLANAR,
    CODEC_ID_PCM_S24LE_PLANAR = AV_CODEC_ID_PCM_S24LE_PLANAR,
    CODEC_ID_PCM_S32LE_PLANAR = AV_CODEC_ID_PCM_S32LE_PLANAR,
    CODEC_ID_PCM_S16BE_PLANAR = AV_CODEC_ID_PCM_S16BE_PLANAR,

    CODEC_ID_PCM_S64LE = AV_CODEC_ID_PCM_S64LE,
    CODEC_ID_PCM_S64BE = AV_CODEC_ID_PCM_S64BE,
    CODEC_ID_PCM_F16LE = AV_CODEC_ID_PCM_F16LE,
    CODEC_ID_PCM_F24LE = AV_CODEC_ID_PCM_F24LE,
    CODEC_ID_PCM_VIDC = AV_CODEC_ID_PCM_VIDC,

    /* various ADPCM codecs */
    CODEC_ID_ADPCM_IMA_QT = AV_CODEC_ID_ADPCM_IMA_QT,
    CODEC_ID_ADPCM_IMA_WAV = AV_CODEC_ID_ADPCM_IMA_WAV,
    CODEC_ID_ADPCM_IMA_DK3 = AV_CODEC_ID_ADPCM_IMA_DK3,
    CODEC_ID_ADPCM_IMA_DK4 = AV_CODEC_ID_ADPCM_IMA_DK4,
    CODEC_ID_ADPCM_IMA_WS = AV_CODEC_ID_ADPCM_IMA_WS,
    CODEC_ID_ADPCM_IMA_SMJPEG = AV_CODEC_ID_ADPCM_IMA_SMJPEG,
    CODEC_ID_ADPCM_MS = AV_CODEC_ID_ADPCM_MS,
    CODEC_ID_ADPCM_4XM = AV_CODEC_ID_ADPCM_4XM,
    CODEC_ID_ADPCM_XA = AV_CODEC_ID_ADPCM_XA,
    CODEC_ID_ADPCM_ADX = AV_CODEC_ID_ADPCM_ADX,
    CODEC_ID_ADPCM_EA = AV_CODEC_ID_ADPCM_EA,
    CODEC_ID_ADPCM_G726 = AV_CODEC_ID_ADPCM_G726,
    CODEC_ID_ADPCM_CT = AV_CODEC_ID_ADPCM_CT,
    CODEC_ID_ADPCM_SWF = AV_CODEC_ID_ADPCM_SWF,
    CODEC_ID_ADPCM_YAMAHA = AV_CODEC_ID_ADPCM_YAMAHA,
    CODEC_ID_ADPCM_SBPRO_4 = AV_CODEC_ID_ADPCM_SBPRO_4,
    CODEC_ID_ADPCM_SBPRO_3 = AV_CODEC_ID_ADPCM_SBPRO_3,
    CODEC_ID_ADPCM_SBPRO_2 = AV_CODEC_ID_ADPCM_SBPRO_2,
    CODEC_ID_ADPCM_THP = AV_CODEC_ID_ADPCM_THP,
    CODEC_ID_ADPCM_IMA_AMV = AV_CODEC_ID_ADPCM_IMA_AMV,
    CODEC_ID_ADPCM_EA_R1 = AV_CODEC_ID_ADPCM_EA_R1,
    CODEC_ID_ADPCM_EA_R3 = AV_CODEC_ID_ADPCM_EA_R3,
    CODEC_ID_ADPCM_EA_R2 = AV_CODEC_ID_ADPCM_EA_R2,
    CODEC_ID_ADPCM_IMA_EA_SEAD = AV_CODEC_ID_ADPCM_IMA_EA_SEAD,
    CODEC_ID_ADPCM_IMA_EA_EACS = AV_CODEC_ID_ADPCM_IMA_EA_EACS,
    CODEC_ID_ADPCM_EA_XAS = AV_CODEC_ID_ADPCM_EA_XAS,
    CODEC_ID_ADPCM_EA_MAXIS_XA = AV_CODEC_ID_ADPCM_EA_MAXIS_XA,
    CODEC_ID_ADPCM_IMA_ISS = AV_CODEC_ID_ADPCM_IMA_ISS,
    CODEC_ID_ADPCM_G722 = AV_CODEC_ID_ADPCM_G722,
    CODEC_ID_ADPCM_IMA_APC = AV_CODEC_ID_ADPCM_IMA_APC,
    CODEC_ID_ADPCM_VIMA = AV_CODEC_ID_ADPCM_VIMA,

    CODEC_ID_ADPCM_AFC = AV_CODEC_ID_ADPCM_AFC,
    CODEC_ID_ADPCM_IMA_OKI = AV_CODEC_ID_ADPCM_IMA_OKI,
    CODEC_ID_ADPCM_DTK = AV_CODEC_ID_ADPCM_DTK,
    CODEC_ID_ADPCM_IMA_RAD = AV_CODEC_ID_ADPCM_IMA_RAD,
    CODEC_ID_ADPCM_G726LE = AV_CODEC_ID_ADPCM_G726LE,
    CODEC_ID_ADPCM_THP_LE = AV_CODEC_ID_ADPCM_THP_LE,
    CODEC_ID_ADPCM_PSX = AV_CODEC_ID_ADPCM_PSX,
    CODEC_ID_ADPCM_AICA = AV_CODEC_ID_ADPCM_AICA,
    CODEC_ID_ADPCM_IMA_DAT4 = AV_CODEC_ID_ADPCM_IMA_DAT4,
    CODEC_ID_ADPCM_MTAF = AV_CODEC_ID_ADPCM_MTAF,

    /* AMR */
    CODEC_ID_AMR_NB = AV_CODEC_ID_AMR_NB,
    CODEC_ID_AMR_WB = AV_CODEC_ID_AMR_WB,

    /* RealAudio codecs*/
    CODEC_ID_RA_144 = AV_CODEC_ID_RA_144,
    CODEC_ID_RA_288 = AV_CODEC_ID_RA_288,

    /* various DPCM codecs */
    CODEC_ID_ROQ_DPCM = AV_CODEC_ID_ROQ_DPCM,
    CODEC_ID_INTERPLAY_DPCM = AV_CODEC_ID_INTERPLAY_DPCM,
    CODEC_ID_XAN_DPCM = AV_CODEC_ID_XAN_DPCM,
    CODEC_ID_SOL_DPCM = AV_CODEC_ID_SOL_DPCM,

    CODEC_ID_SDX2_DPCM = AV_CODEC_ID_SDX2_DPCM,
    CODEC_ID_GREMLIN_DPCM = AV_CODEC_ID_GREMLIN_DPCM,

    /* audio codecs */
    CODEC_ID_MP2 = AV_CODEC_ID_MP2,
    CODEC_ID_MP3 = AV_CODEC_ID_MP3, ///< preferred ID for decoding MPEG audio layer 1, 2 or 3
    CODEC_ID_AAC = AV_CODEC_ID_AAC,
    CODEC_ID_AC3 = AV_CODEC_ID_AC3,
    CODEC_ID_DTS = AV_CODEC_ID_DTS,
    CODEC_ID_VORBIS = AV_CODEC_ID_VORBIS,
    CODEC_ID_DVAUDIO = AV_CODEC_ID_DVAUDIO,
    CODEC_ID_WMAV1 = AV_CODEC_ID_WMAV1,
    CODEC_ID_WMAV2 = AV_CODEC_ID_WMAV2,
    CODEC_ID_MACE3 = AV_CODEC_ID_MACE3,
    CODEC_ID_MACE6 = AV_CODEC_ID_MACE6,
    CODEC_ID_VMDAUDIO = AV_CODEC_ID_VMDAUDIO,
    CODEC_ID_FLAC = AV_CODEC_ID_FLAC,
    CODEC_ID_MP3ADU = AV_CODEC_ID_MP3ADU,
    CODEC_ID_MP3ON4 = AV_CODEC_ID_MP3ON4,
    CODEC_ID_SHORTEN = AV_CODEC_ID_SHORTEN,
    CODEC_ID_ALAC = AV_CODEC_ID_ALAC,
    CODEC_ID_WESTWOOD_SND1 = AV_CODEC_ID_WESTWOOD_SND1,
    CODEC_ID_GSM = AV_CODEC_ID_GSM, ///< as in Berlin toast format
    CODEC_ID_QDM2 = AV_CODEC_ID_QDM2,
    CODEC_ID_COOK = AV_CODEC_ID_COOK,
    CODEC_ID_TRUESPEECH = AV_CODEC_ID_TRUESPEECH,
    CODEC_ID_TTA = AV_CODEC_ID_TTA,
    CODEC_ID_SMACKAUDIO = AV_CODEC_ID_SMACKAUDIO,
    CODEC_ID_QCELP = AV_CODEC_ID_QCELP,
    CODEC_ID_WAVPACK = AV_CODEC_ID_WAVPACK,
    CODEC_ID_DSICINAUDIO = AV_CODEC_ID_DSICINAUDIO,
    CODEC_ID_IMC = AV_CODEC_ID_IMC,
    CODEC_ID_MUSEPACK7 = AV_CODEC_ID_MUSEPACK7,
    CODEC_ID_MLP = AV_CODEC_ID_MLP,
    CODEC_ID_GSM_MS = AV_CODEC_ID_GSM_MS, /* as found in WAV */
    CODEC_ID_ATRAC3 = AV_CODEC_ID_ATRAC3,
    CODEC_ID_APE = AV_CODEC_ID_APE,
    CODEC_ID_NELLYMOSER = AV_CODEC_ID_NELLYMOSER,
    CODEC_ID_MUSEPACK8 = AV_CODEC_ID_MUSEPACK8,
    CODEC_ID_SPEEX = AV_CODEC_ID_SPEEX,
    CODEC_ID_WMAVOICE = AV_CODEC_ID_WMAVOICE,
    CODEC_ID_WMAPRO = AV_CODEC_ID_WMAPRO,
    CODEC_ID_WMALOSSLESS = AV_CODEC_ID_WMALOSSLESS,
    CODEC_ID_ATRAC3P = AV_CODEC_ID_ATRAC3P,
    CODEC_ID_EAC3 = AV_CODEC_ID_EAC3,
    CODEC_ID_SIPR = AV_CODEC_ID_SIPR,
    CODEC_ID_MP1 = AV_CODEC_ID_MP1,
    CODEC_ID_TWINVQ = AV_CODEC_ID_TWINVQ,
    CODEC_ID_TRUEHD = AV_CODEC_ID_TRUEHD,
    CODEC_ID_MP4ALS = AV_CODEC_ID_MP4ALS,
    CODEC_ID_ATRAC1 = AV_CODEC_ID_ATRAC1,
    CODEC_ID_BINKAUDIO_RDFT = AV_CODEC_ID_BINKAUDIO_RDFT,
    CODEC_ID_BINKAUDIO_DCT = AV_CODEC_ID_BINKAUDIO_DCT,
    CODEC_ID_AAC_LATM = AV_CODEC_ID_AAC_LATM,
    CODEC_ID_QDMC = AV_CODEC_ID_QDMC,
    CODEC_ID_CELT = AV_CODEC_ID_CELT,
    CODEC_ID_G723_1 = AV_CODEC_ID_G723_1,
    CODEC_ID_G729 = AV_CODEC_ID_G729,
    CODEC_ID_8SVX_EXP = AV_CODEC_ID_8SVX_EXP,
    CODEC_ID_8SVX_FIB = AV_CODEC_ID_8SVX_FIB,
    CODEC_ID_BMV_AUDIO = AV_CODEC_ID_BMV_AUDIO,
    CODEC_ID_RALF = AV_CODEC_ID_RALF,
    CODEC_ID_IAC = AV_CODEC_ID_IAC,
    CODEC_ID_ILBC = AV_CODEC_ID_ILBC,
    CODEC_ID_OPUS = AV_CODEC_ID_OPUS,
    CODEC_ID_COMFORT_NOISE = AV_CODEC_ID_COMFORT_NOISE,
    CODEC_ID_TAK = AV_CODEC_ID_TAK,
    CODEC_ID_METASOUND = AV_CODEC_ID_METASOUND,
    CODEC_ID_PAF_AUDIO = AV_CODEC_ID_PAF_AUDIO,
    CODEC_ID_ON2AVC = AV_CODEC_ID_ON2AVC,
    CODEC_ID_DSS_SP = AV_CODEC_ID_DSS_SP,
    CODEC_ID_CODEC2 = AV_CODEC_ID_CODEC2,

    CODEC_ID_FFWAVESYNTH = AV_CODEC_ID_FFWAVESYNTH,
    CODEC_ID_SONIC = AV_CODEC_ID_SONIC,
    CODEC_ID_SONIC_LS = AV_CODEC_ID_SONIC_LS,
    CODEC_ID_EVRC = AV_CODEC_ID_EVRC,
    CODEC_ID_SMV = AV_CODEC_ID_SMV,
    CODEC_ID_DSD_LSBF = AV_CODEC_ID_DSD_LSBF,
    CODEC_ID_DSD_MSBF = AV_CODEC_ID_DSD_MSBF,
    CODEC_ID_DSD_LSBF_PLANAR = AV_CODEC_ID_DSD_LSBF_PLANAR,
    CODEC_ID_DSD_MSBF_PLANAR = AV_CODEC_ID_DSD_MSBF_PLANAR,
    CODEC_ID_4GV = AV_CODEC_ID_4GV,
    CODEC_ID_INTERPLAY_ACM = AV_CODEC_ID_INTERPLAY_ACM,
    CODEC_ID_XMA1 = AV_CODEC_ID_XMA1,
    CODEC_ID_XMA2 = AV_CODEC_ID_XMA2,
    CODEC_ID_DST = AV_CODEC_ID_DST,
    CODEC_ID_ATRAC3AL = AV_CODEC_ID_ATRAC3AL,
    CODEC_ID_ATRAC3PAL = AV_CODEC_ID_ATRAC3PAL,
    CODEC_ID_DOLBY_E = AV_CODEC_ID_DOLBY_E,
    CODEC_ID_APTX = AV_CODEC_ID_APTX,
    CODEC_ID_APTX_HD = AV_CODEC_ID_APTX_HD,
    CODEC_ID_SBC = AV_CODEC_ID_SBC,
    CODEC_ID_ATRAC9 = AV_CODEC_ID_ATRAC9,

    /* subtitle codecs */
    CODEC_ID_FIRST_SUBTITLE = AV_CODEC_ID_FIRST_SUBTITLE,          ///< A dummy ID pointing at the start of subtitle codecs.
    CODEC_ID_DVD_SUBTITLE = AV_CODEC_ID_DVD_SUBTITLE,
    CODEC_ID_DVB_SUBTITLE = AV_CODEC_ID_DVB_SUBTITLE,
    CODEC_ID_TEXT = AV_CODEC_ID_TEXT,  ///< raw UTF-8 text
    CODEC_ID_XSUB = AV_CODEC_ID_XSUB,
    CODEC_ID_SSA = AV_CODEC_ID_SSA,
    CODEC_ID_MOV_TEXT = AV_CODEC_ID_MOV_TEXT,
    CODEC_ID_HDMV_PGS_SUBTITLE = AV_CODEC_ID_HDMV_PGS_SUBTITLE,
    CODEC_ID_DVB_TELETEXT = AV_CODEC_ID_DVB_TELETEXT,
    CODEC_ID_SRT = AV_CODEC_ID_SRT,

    CODEC_ID_MICRODVD = AV_CODEC_ID_MICRODVD,
    CODEC_ID_EIA_608 = AV_CODEC_ID_EIA_608,
    CODEC_ID_JACOSUB = AV_CODEC_ID_JACOSUB,
    CODEC_ID_SAMI = AV_CODEC_ID_SAMI,
    CODEC_ID_REALTEXT = AV_CODEC_ID_REALTEXT,
    CODEC_ID_STL = AV_CODEC_ID_STL,
    CODEC_ID_SUBVIEWER1 = AV_CODEC_ID_SUBVIEWER1,
    CODEC_ID_SUBVIEWER = AV_CODEC_ID_SUBVIEWER,
    CODEC_ID_SUBRIP = AV_CODEC_ID_SUBRIP,
    CODEC_ID_WEBVTT = AV_CODEC_ID_WEBVTT,
    CODEC_ID_MPL2 = AV_CODEC_ID_MPL2,
    CODEC_ID_VPLAYER = AV_CODEC_ID_VPLAYER,
    CODEC_ID_PJS = AV_CODEC_ID_PJS,
    CODEC_ID_ASS = AV_CODEC_ID_ASS,
    CODEC_ID_HDMV_TEXT_SUBTITLE = AV_CODEC_ID_HDMV_TEXT_SUBTITLE,
    CODEC_ID_TTML = AV_CODEC_ID_TTML,

    /* other specific kind of codecs (generally used for attachments) */
    CODEC_ID_FIRST_UNKNOWN = AV_CODEC_ID_FIRST_UNKNOWN,           ///< A dummy ID pointing at the start of various fake codecs.
    CODEC_ID_TTF = AV_CODEC_ID_TTF,

    CODEC_ID_SCTE_35 = AV_CODEC_ID_SCTE_35, ///< Contain timestamp estimated through PCR of program stream.
    CODEC_ID_BINTEXT = AV_CODEC_ID_BINTEXT,
    CODEC_ID_XBIN = AV_CODEC_ID_XBIN,
    CODEC_ID_IDF = AV_CODEC_ID_IDF,
    CODEC_ID_OTF = AV_CODEC_ID_OTF,
    CODEC_ID_SMPTE_KLV = AV_CODEC_ID_SMPTE_KLV,
    CODEC_ID_DVD_NAV = AV_CODEC_ID_DVD_NAV,
    CODEC_ID_TIMED_ID3 = AV_CODEC_ID_TIMED_ID3,
    CODEC_ID_BIN_DATA = AV_CODEC_ID_BIN_DATA,


    CODEC_ID_PROBE = AV_CODEC_ID_PROBE, ///< codec_id is not known (like AV_CODEC_ID_NONE) but lavf should attempt to identify it

    CODEC_ID_MPEG2TS = AV_CODEC_ID_MPEG2TS, /**< _FAKE_ codec to indicate a raw MPEG-2 TS
                                * stream (only used by libavformat) */
    CODEC_ID_MPEG4SYSTEMS = AV_CODEC_ID_MPEG4SYSTEMS, /**< _FAKE_ codec to indicate a MPEG-4 Systems
                                * stream (only used by libavformat) */
    CODEC_ID_FFMETADATA = AV_CODEC_ID_FFMETADATA,   ///< Dummy codec for streams containing only metadata information.
    CODEC_ID_WRAPPED_AVFRAME = AV_CODEC_ID_WRAPPED_AVFRAME, ///< Passthrough codec, AVFrames wrapped in AVPacket

  } ID;

  /**
   * Capabilities supported by a codec.
   */
  typedef enum CodecCapability
  {
    /** Decoder can use draw_horiz_band callback. */
    CAP_DRAW_HORIZ_BAND = AV_CODEC_CAP_DRAW_HORIZ_BAND,
    /**
     * Codec uses get_buffer() for allocating buffers and supports custom allocators.
     * If not set, it might not use get_buffer() at all or use operations that
     * assume the buffer was allocated by avcodec_default_get_buffer.
     */
    CAP_DR1 = AV_CODEC_CAP_DR1,
    CAP_TRUNCATED = AV_CODEC_CAP_TRUNCATED,

    /**
     * Encoder or decoder requires flushing with NULL input at the end in order to
     * give the complete and correct output.
     *
     * NOTE: If this flag is not set, the codec is guaranteed to never be fed with
     *       with NULL data. The user can still send NULL data to the public encode
     *       or decode function, but libavcodec will not pass it along to the codec
     *       unless this flag is set.
     *
     * Decoders:
     * The decoder has a non-zero delay and needs to be fed with avpkt->data=NULL,
     * avpkt->size=0 at the end to get the delayed data until the decoder no longer
     * returns frames.
     *
     * Encoders:
     * The encoder needs to be fed with NULL data at the end of encoding until the
     * encoder no longer returns data.
     *
     * NOTE: Setting this
     *       flag also means that the encoder must set the pts and duration for
     *       each output packet. If this flag is not set, the pts and duration will
     *       be determined by libavcodec from the input frame.
     */
    CAP_DELAY = AV_CODEC_CAP_DELAY,
    /**
     * Codec can be fed a final frame with a smaller size.
     * This can be used to prevent truncation of the last audio samples.
     */
    CAP_SMALL_LAST_FRAME = AV_CODEC_CAP_SMALL_LAST_FRAME,

    /**
     * Codec can output multiple frames per AVPacket
     * Normally demuxers return one frame at a time, demuxers which do not do
     * are connected to a parser to split what they return into proper frames.
     * This flag is reserved to the very rare category of codecs which have a
     * bitstream that cannot be split into frames without timeconsuming
     * operations like full decoding. Demuxers carring such bitstreams thus
     * may return multiple frames in a packet. This has many disadvantages like
     * prohibiting stream copy in many cases thus it should only be considered
     * as a last resort.
     */
    CAP_SUBFRAMES = AV_CODEC_CAP_SUBFRAMES,
    /**
     * Codec is experimental and is thus avoided in favor of non experimental
     * encoders
     */
    CAP_EXPERIMENTAL = AV_CODEC_CAP_EXPERIMENTAL,
    /**
     * Codec should fill in channel configuration and samplerate instead of container
     */
    CAP_CHANNEL_CONF = AV_CODEC_CAP_CHANNEL_CONF,

    /**
     * Codec supports frame-level multithreading.
     */
    CAP_FRAME_THREADS = AV_CODEC_CAP_FRAME_THREADS,

    /**
     * Codec supports slice-based (or partition-based) multithreading.
     */
    CAP_SLICE_THREADS = AV_CODEC_CAP_SLICE_THREADS,
    /**
     * Codec supports changed parameters at any point.
     */
    CAP_PARAM_CHANGE = AV_CODEC_CAP_PARAM_CHANGE,
    /**
     * Codec supports avctx->thread_count == 0 (auto).
     */
    CAP_AUTO_THREADS = AV_CODEC_CAP_AUTO_THREADS,
    /**
     * Audio encoder supports receiving a different number of samples in each call.
     */
    CAP_VARIABLE_FRAME_SIZE = AV_CODEC_CAP_VARIABLE_FRAME_SIZE,

    /**
     * Decoder is not a preferred choice for probing.
     * This indicates that the decoder is not a good choice for probing.
     * It could for example be an expensive to spin up hardware decoder,
     * or it could simply not provide a lot of useful information about
     * the stream.
     * A decoder marked with this flag should only be used as last resort
     * choice for probing.
     */
    CAP_AVOID_PROBING = AV_CODEC_CAP_AVOID_PROBING,
    /**
     * Codec is intra only.
     */
    CAP_INTRA_ONLY = AV_CODEC_CAP_INTRA_ONLY,
    /**
     * Codec is lossless.
     */
    CAP_LOSSLESS = AV_CODEC_CAP_LOSSLESS,

    /**
     * Codec is backed by a hardware implementation. Typically used to
     * identify a non-hwaccel hardware decoder. For information about hwaccels, use
     * avcodec_get_hw_config() instead.
     */
    CAP_HARDWARE = AV_CODEC_CAP_HARDWARE,

    /**
     * Codec is potentially backed by a hardware implementation, but not
     * necessarily. This is used instead of AV_CODEC_CAP_HARDWARE, if the
     * implementation provides some sort of internal fallback.
     */
    CAP_HYBRID = AV_CODEC_CAP_HYBRID,
  } CodecCapability;

  /**
   * Checks if this codec has the given capability.
   * @param c Capability to check.
   */
  virtual bool
  hasCapability(CodecCapability c);

  /**
   * Get a bitmask of the supported CodecCapability flags.
   */
  virtual int32_t
  getCapabilities();

  /**
   * Get the name of the codec.
   * @return The name of this Codec.
   */
  virtual const char *
  getName();

  /**
   * Get the ID of this codec, as an integer.
   * @return the ID of this codec, as an integer.
   */
  virtual int
  getIDAsInt();

  /**
   * Get the ID of this codec as an enumeration.
   * @return the ID of this codec, an enum ID
   */
  virtual ID
  getID();

  /**
   * Get the type of this codec.
   * @return The type of this Codec, as a enum Type
   */
  virtual MediaDescriptor::Type
  getType();

  /**
   * Can this codec be used for decoding?
   * @return Can this Codec decode?
   */
  virtual bool
  canDecode();

  /**
   * Can this codec be used for encoding?
   * @return Can this Codec encode?
   */
  virtual bool
  canEncode();

  /**
   * Find a codec that can be used for encoding.
   * @param id The id of the codec
   * @return the codec, or null if we can't find it.
   *
   */
  static Codec *
  findEncodingCodec(Codec::ID id);
  /**
   * Find a codec that can be used for encoding.
   * @param id The id of the codec, as an integer.
   * @return the codec, or null if we can't find it.
   */
  static Codec *
  findEncodingCodecByIntID(int id);
  /**
   * Find a codec that can be used for encoding.
   * @param id The id of the codec, as a FFMPEG short-name string
   *   (for example, "mpeg4").
   * @return the codec, or null if we can't find it.
   */
  static Codec *
  findEncodingCodecByName(const char*id);
  /**
   * Find a codec that can be used for decoding.
   * @param id The id of the codec
   * @return the codec, or null if we can't find it.
   */
  static Codec *
  findDecodingCodec(Codec::ID id);
  /**
   * Find a codec that can be used for decoding.
   * @param id The id of the codec, as an integer
   * @return the codec, or null if we can't find it.
   */
  static Codec *
  findDecodingCodecByIntID(int id);
  /**
   * Find a codec that can be used for decoding.
   * @param id The id of the codec, as a FFMPEG short-name string
   *   (for example, "mpeg4")
   * @return the codec, or null if we can't find it.
   */
  static Codec *
  findDecodingCodecByName(const char*id);

  /**
   * Ask us to guess an encoding codec based on the inputs
   * passed in.
   * <p>
   * You must pass in at least one non null fmt, shortName,
   * url or mime_type.
   * </p>
   * @param fmt A SinkFormat for the container you'll want to encode into.
   * @param shortName The FFMPEG short name of the codec (e.g. "mpeg4").
   * @param url The URL you'll be writing packets to.
   * @param mimeType The mime type of the container.
   * @param type The codec type.
   * @return the codec, or null if we can't find it.
   */
  static Codec*
  guessEncodingCodec(MuxerFormat* fmt, const char *shortName, const char*url,
      const char*mimeType, MediaDescriptor::Type type);

  /**
   * Get the long name for this codec.
   *
   * @return the long name.
   */
  virtual const char *
  getLongName();

  /**
   * Capability flags
   */

  /**
   * Get the number of installed codecs on this system.
   * @return the number of installed codecs.
   */
  static int32_t
  getNumInstalledCodecs();

  /**
   * Get the ICodec at the given index.
   *
   * @param index the index in our list
   *
   * @return the codec, or null if index < 0 or index >=
   *   #getNumInstalledCodecs()
   */
  static Codec*
  getInstalledCodec(int32_t index);

  /**
   * Get the number of frame rates this codec supports for encoding.
   * Not all codecs will report this number.
   * @return the number or 0 if we don't know.
   */
  virtual int32_t
  getNumSupportedVideoFrameRates();

  /**
   * Return the supported frame rate at the given index.
   *
   * @param index the index in our list.
   *
   * @return the frame rate, or null if unknown, if index <0 or
   *   if index >= #getNumSupportedVideoFrameRates()
   */
  virtual Rational*
  getSupportedVideoFrameRate(int32_t index);

  /**
   * Get the number of supported video pixel formats this codec supports
   * for encoding.  Not all codecs will report this.
   *
   * @return the number or 0 if we don't know.
   */
  virtual int32_t
  getNumSupportedVideoPixelFormats();

  /**
   * Return the supported video pixel format at the given index.
   *
   * @param index the index in our list.
   *
   * @return the pixel format, or IPixelFormat.Type#NONE if unknown,
   *   if index <0 or
   *   if index >= #getNumSupportedVideoPixelFormats()
   */
  virtual PixelFormat::Type
  getSupportedVideoPixelFormat(int32_t index);

  /**
   * Get the number of different audio sample rates this codec supports
   * for encoding.  Not all codecs will report this.
   *
   * @return the number or 0 if we don't know.
   */
  virtual int32_t
  getNumSupportedAudioSampleRates();

  /**
   * Return the support audio sample rate at the given index.
   *
   * @param index the index in our list.
   *
   * @return the sample rate, or 0 if unknown, index < 0 or
   *   index >= #getNumSupportedAudioSampleRates()
   */
  virtual int32_t
  getSupportedAudioSampleRate(int32_t index);

  /**
   * Get the number of different audio sample formats this codec supports
   * for encoding.  Not all codecs will report this.
   *
   * @return the number or 0 if we don't know.
   */
  virtual int32_t
  getNumSupportedAudioFormats();

  /**
   * Get the supported sample format at this index.
   *
   * @param index the index in our list.
   *
   * @return the format, or AudioFormat.Type.SAMPLE_FMT_NONE if
   *   unknown, index < 0 or index >=
   *   #getNumSupportedAudioSampleFormats().
   */
  virtual AudioFormat::Type
  getSupportedAudioFormat(int32_t index);

  /**
   * Get the number of different audio channel layouts this codec supports
   * for encoding.  Not all codecs will report this.
   *
   *
   * @return the number or 0 if we don't know.
   */
  virtual int32_t
  getNumSupportedAudioChannelLayouts();

  /**
   * Get the supported audio channel layout at this index.
   *
   * The value returned is a bit flag representing the different
   * types of audio layout this codec can support.  Test the values
   * by bit-comparing them to the AudioChannel.Layout
   * enum types.
   *
   * @param index the index
   *
   * @return the channel layout, or 0 if unknown, index < 0 or
   *   index >= #getNumSupportedAudioChannelLayouts.
   */
  virtual AudioChannel::Layout
  getSupportedAudioChannelLayout(int32_t index);

  /**
   * Get the number of supported CodecProfiles this codec
   * supports.
   */
  virtual int32_t
  getNumSupportedProfiles();

  /**
   * Get the supported CodecProfile at this index.
   *
   * @param index the index
   * @return A CodecProfile, or null if unknown, index < 0
   *   or lindex >= #getNumSupportedProfiles.
   */
  virtual CodecProfile*
  getSupportedProfile(int32_t index);

#ifndef SWIG
  static Codec*
  make(const AVCodec* codec);

  const AVCodec* getCtx() { return mCodec; }
#endif // ! SWIG
private:
  Codec();
  virtual
  ~Codec();

  const AVCodec* mCodec;
};

/**
 * Meta-data that describes a Codec object.
 */
class VS_API_HUMBLEVIDEO CodecDescriptor : public io::humble::ferry::RefCounted
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(CodecDescriptor)
public:
  /** Properties supported by a codec. */
  typedef enum CodecProperty
  {

    /**
     * Codec uses only intra compression.
     * Video codecs only.
     */
    PROP_INTRA_ONLY = AV_CODEC_PROP_INTRA_ONLY,
    /**
     * Codec supports lossy compression. Audio and video codecs only.
     * @note a codec may support both lossy and lossless
     * compression modes
     */
    PROP_LOSSY = AV_CODEC_PROP_LOSSY,
    /**
     * Codec supports lossless compression. Audio and video codecs only.
     */
    PROP_LOSSLESS = AV_CODEC_PROP_LOSSLESS,
    /**
     * Subtitle codec is bitmap based
     */
    PROP_BITMAP_SUB = AV_CODEC_PROP_BITMAP_SUB,
  } CodecProperty;
  /**
   * Checks if this codec has the given property.
   * @param p property to check
   */
  virtual bool
  hasProperty(CodecProperty p) {
    bool retval = false;
    if (mDescriptor) retval = p & mDescriptor->props;
    return retval;
  }

  /**
   * Name of the codec. Unique for each codec. Will contain only
   * alphanumeric and _ characters.
   */
  virtual const char*
  getName() {
    return mDescriptor->name;
  }
  /**
   * A more descriptive name for this codec.
   */
  virtual const char*
  getLongName() {
    return mDescriptor->long_name;
  }
  /**
   * A bit mask of Codec.Properties this codec has.
   */
  virtual int32_t
  getProperties() {
    return mDescriptor->props;
  }

  /** Get the codec ID for this descriptor */
  virtual Codec::ID
  getId() {
    return (Codec::ID) mDescriptor->id;
  }

  virtual MediaDescriptor::Type
  getType() {
    return (MediaDescriptor::Type) mDescriptor->type;
  }
  /**
   * Get the descriptor for the given id.
   */
  static CodecDescriptor*
  make(Codec::ID id);

#ifndef SWIG
  static CodecDescriptor*
  make(const AVCodecDescriptor *d) {
    CodecDescriptor* retval = 0;
    if (d) {
      retval = make();
      retval->mDescriptor = d;
    }
    return retval;
  }
#endif // ! SWIG
private:
  CodecDescriptor() :
      mDescriptor(0) {
  }
  virtual
  ~CodecDescriptor() {
  }
  const AVCodecDescriptor *mDescriptor;

};

}}}
#endif /* CODEC_H_ */
