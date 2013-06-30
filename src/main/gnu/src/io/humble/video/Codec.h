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
 *
 * Codec.h
 *
 *  Created on: Jun 28, 2013
 *      Author: aclarke
 */

#ifndef CODEC_H_
#define CODEC_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/HumbleVideo.h>

namespace io {
namespace humble {
namespace video {

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
  MediaDescriptor()
  {
  }
  virtual
  ~MediaDescriptor()
  {
  }
};

/**
 * A profile supported by a {@link Codec}.
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
  } ProfileType;
  /** Get the type for this profile. */
  virtual ProfileType
  getProfile()
  {
    return (ProfileType) mProfile->profile;
  }
  /** Get the name for this profile. */
  virtual const char *
  getName()
  {
    return mProfile->name;
  }
#ifndef SWIG
  CodecProfile *
  make(AVProfile * p)
  {
    CodecProfile* retval = 0;
    if (p)
    {
      retval = make();
      retval->mProfile = p;
    }
    return retval;
  }
#endif // ! SWIG
private:
  CodecProfile() :
      mProfile(0)
  {
  }
  virtual
  ~CodecProfile();
  AVProfile* mProfile;
};

/**
 * A codec that can be used either to encode raw data into compressed bitstreams,
 * or decode compressed data into raw data, or both!
 */
class VS_API_HUMBLEVIDEO Codec : public io::humble::ferry::RefCounted
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(Codec)
public:

  typedef enum Id
  {
    /**
     * Identify the syntax and semantics of the bitstream.
     * The principle is roughly:
     * Two decoders with the same ID can decode the same streams.
     * Two encoders with the same ID can encode compatible streams.
     * There may be slight deviations from the principle due to implementation
     * details.
     *
     * From Ffmpeg 1.2.1
     */
    ID_NONE,

    /* video codecs */
    ID_MPEG1VIDEO,
    ID_MPEG2VIDEO,
    ID_MPEG2VIDEO_XVMC,
    ID_H261,
    ID_H263,
    ID_RV10,
    ID_RV20,
    ID_MJPEG,
    ID_MJPEGB,
    ID_LJPEG,
    ID_SP5X,
    ID_JPEGLS,
    ID_MPEG4,
    ID_RAWVIDEO,
    ID_MSMPEG4V1,
    ID_MSMPEG4V2,
    ID_MSMPEG4V3,
    ID_WMV1,
    ID_WMV2,
    ID_H263P,
    ID_H263I,
    ID_FLV1,
    ID_SVQ1,
    ID_SVQ3,
    ID_DVVIDEO,
    ID_HUFFYUV,
    ID_CYUV,
    ID_H264,
    ID_INDEO3,
    ID_VP3,
    ID_THEORA,
    ID_ASV1,
    ID_ASV2,
    ID_FFV1,
    ID_4XM,
    ID_VCR1,
    ID_CLJR,
    ID_MDEC,
    ID_ROQ,
    ID_INTERPLAY_VIDEO,
    ID_XAN_WC3,
    ID_XAN_WC4,
    ID_RPZA,
    ID_CINEPAK,
    ID_WS_VQA,
    ID_MSRLE,
    ID_MSVIDEO1,
    ID_IDCIN,
    ID_8BPS,
    ID_SMC,
    ID_FLIC,
    ID_TRUEMOTION1,
    ID_VMDVIDEO,
    ID_MSZH,
    ID_ZLIB,
    ID_QTRLE,
    ID_SNOW,
    ID_TSCC,
    ID_ULTI,
    ID_QDRAW,
    ID_VIXL,
    ID_QPEG,
    ID_PNG,
    ID_PPM,
    ID_PBM,
    ID_PGM,
    ID_PGMYUV,
    ID_PAM,
    ID_FFVHUFF,
    ID_RV30,
    ID_RV40,
    ID_VC1,
    ID_WMV3,
    ID_LOCO,
    ID_WNV1,
    ID_AASC,
    ID_INDEO2,
    ID_FRAPS,
    ID_TRUEMOTION2,
    ID_BMP,
    ID_CSCD,
    ID_MMVIDEO,
    ID_ZMBV,
    ID_AVS,
    ID_SMACKVIDEO,
    ID_NUV,
    ID_KMVC,
    ID_FLASHSV,
    ID_CAVS,
    ID_JPEG2000,
    ID_VMNC,
    ID_VP5,
    ID_VP6,
    ID_VP6F,
    ID_TARGA,
    ID_DSICINVIDEO,
    ID_TIERTEXSEQVIDEO,
    ID_TIFF,
    ID_GIF,
    ID_DXA,
    ID_DNXHD,
    ID_THP,
    ID_SGI,
    ID_C93,
    ID_BETHSOFTVID,
    ID_PTX,
    ID_TXD,
    ID_VP6A,
    ID_AMV,
    ID_VB,
    ID_PCX,
    ID_SUNRAST,
    ID_INDEO4,
    ID_INDEO5,
    ID_MIMIC,
    ID_RL2,
    ID_ESCAPE124,
    ID_DIRAC,
    ID_BFI,
    ID_CMV,
    ID_MOTIONPIXELS,
    ID_TGV,
    ID_TGQ,
    ID_TQI,
    ID_AURA,
    ID_AURA2,
    ID_V210X,
    ID_TMV,
    ID_V210,
    ID_DPX,
    ID_MAD,
    ID_FRWU,
    ID_FLASHSV2,
    ID_CDGRAPHICS,
    ID_R210,
    ID_ANM,
    ID_BINKVIDEO,
    ID_IFF_ILBM,
    ID_IFF_BYTERUN1,
    ID_KGV1,
    ID_YOP,
    ID_VP8,
    ID_PICTOR,
    ID_ANSI,
    ID_A64_MULTI,
    ID_A64_MULTI5,
    ID_R10K,
    ID_MXPEG,
    ID_LAGARITH,
    ID_PRORES,
    ID_JV,
    ID_DFA,
    ID_WMV3IMAGE,
    ID_VC1IMAGE,
    ID_UTVIDEO,
    ID_BMV_VIDEO,
    ID_VBLE,
    ID_DXTORY,
    ID_V410,
    ID_XWD,
    ID_CDXL,
    ID_XBM,
    ID_ZEROCODEC,
    ID_MSS1,
    ID_MSA1,
    ID_TSCC2,
    ID_MTS2,
    ID_CLLC,
    ID_MSS2,
    ID_VP9,
    ID_BRENDER_PIX = MKBETAG('B','P','I','X'),
    ID_Y41P = MKBETAG('Y','4','1','P'),
    ID_ESCAPE130 = MKBETAG('E','1','3','0'),
    ID_EXR = MKBETAG('0','E','X','R'),
    ID_AVRP = MKBETAG('A','V','R','P'),

    ID_012V = MKBETAG('0','1','2','V'),
    ID_G2M = MKBETAG( 0 ,'G','2','M'),
    ID_AVUI = MKBETAG('A','V','U','I'),
    ID_AYUV = MKBETAG('A','Y','U','V'),
    ID_TARGA_Y216 = MKBETAG('T','2','1','6'),
    ID_V308 = MKBETAG('V','3','0','8'),
    ID_V408 = MKBETAG('V','4','0','8'),
    ID_YUV4 = MKBETAG('Y','U','V','4'),
    ID_SANM = MKBETAG('S','A','N','M'),
    ID_PAF_VIDEO = MKBETAG('P','A','F','V'),
    ID_AVRN = MKBETAG('A','V','R','n'),
    ID_CPIA = MKBETAG('C','P','I','A'),
    ID_XFACE = MKBETAG('X','F','A','C'),
    ID_SGIRLE = MKBETAG('S','G','I','R'),
    ID_MVC1 = MKBETAG('M','V','C','1'),
    ID_MVC2 = MKBETAG('M','V','C','2'),

    /* various PCM "codecs" */
    ID_FIRST_AUDIO = 0x10000,
    ID_PCM_S16LE = 0x10000,
    ID_PCM_S16BE,
    ID_PCM_U16LE,
    ID_PCM_U16BE,
    ID_PCM_S8,
    ID_PCM_U8,
    ID_PCM_MULAW,
    ID_PCM_ALAW,
    ID_PCM_S32LE,
    ID_PCM_S32BE,
    ID_PCM_U32LE,
    ID_PCM_U32BE,
    ID_PCM_S24LE,
    ID_PCM_S24BE,
    ID_PCM_U24LE,
    ID_PCM_U24BE,
    ID_PCM_S24DAUD,
    ID_PCM_ZORK,
    ID_PCM_S16LE_PLANAR,
    ID_PCM_DVD,
    ID_PCM_F32BE,
    ID_PCM_F32LE,
    ID_PCM_F64BE,
    ID_PCM_F64LE,
    ID_PCM_BLURAY,
    ID_PCM_LXF,
    ID_S302M,
    ID_PCM_S8_PLANAR,
    ID_PCM_S24LE_PLANAR = MKBETAG(24,'P','S','P'),
    ID_PCM_S32LE_PLANAR = MKBETAG(32,'P','S','P'),
    ID_PCM_S16BE_PLANAR = MKBETAG('P','S','P',16),

    /* various ADPCM codecs */
    ID_ADPCM_IMA_QT = 0x11000,
    ID_ADPCM_IMA_WAV,
    ID_ADPCM_IMA_DK3,
    ID_ADPCM_IMA_DK4,
    ID_ADPCM_IMA_WS,
    ID_ADPCM_IMA_SMJPEG,
    ID_ADPCM_MS,
    ID_ADPCM_4XM,
    ID_ADPCM_XA,
    ID_ADPCM_ADX,
    ID_ADPCM_EA,
    ID_ADPCM_G726,
    ID_ADPCM_CT,
    ID_ADPCM_SWF,
    ID_ADPCM_YAMAHA,
    ID_ADPCM_SBPRO_4,
    ID_ADPCM_SBPRO_3,
    ID_ADPCM_SBPRO_2,
    ID_ADPCM_THP,
    ID_ADPCM_IMA_AMV,
    ID_ADPCM_EA_R1,
    ID_ADPCM_EA_R3,
    ID_ADPCM_EA_R2,
    ID_ADPCM_IMA_EA_SEAD,
    ID_ADPCM_IMA_EA_EACS,
    ID_ADPCM_EA_XAS,
    ID_ADPCM_EA_MAXIS_XA,
    ID_ADPCM_IMA_ISS,
    ID_ADPCM_G722,
    ID_ADPCM_IMA_APC,
    ID_VIMA = MKBETAG('V','I','M','A'),
    ID_ADPCM_AFC = MKBETAG('A','F','C',' '),
    ID_ADPCM_IMA_OKI = MKBETAG('O','K','I',' '),

    /* AMR */
    ID_AMR_NB = 0x12000,
    ID_AMR_WB,

    /* RealAudio codecs*/
    ID_RA_144 = 0x13000,
    ID_RA_288,

    /* various DPCM codecs */
    ID_ROQ_DPCM = 0x14000,
    ID_INTERPLAY_DPCM,
    ID_XAN_DPCM,
    ID_SOL_DPCM,

    /* audio codecs */
    ID_MP2 = 0x15000,
    ID_MP3,
    ID_AAC,
    ID_AC3,
    ID_DTS,
    ID_VORBIS,
    ID_DVAUDIO,
    ID_WMAV1,
    ID_WMAV2,
    ID_MACE3,
    ID_MACE6,
    ID_VMDAUDIO,
    ID_FLAC,
    ID_MP3ADU,
    ID_MP3ON4,
    ID_SHORTEN,
    ID_ALAC,
    ID_WESTWOOD_SND1,
    ID_GSM,
    ID_QDM2,
    ID_COOK,
    ID_TRUESPEECH,
    ID_TTA,
    ID_SMACKAUDIO,
    ID_QCELP,
    ID_WAVPACK,
    ID_DSICINAUDIO,
    ID_IMC,
    ID_MUSEPACK7,
    ID_MLP,
    ID_GSM_MS,
    ID_ATRAC3,
    ID_VOXWARE,
    ID_APE,
    ID_NELLYMOSER,
    ID_MUSEPACK8,
    ID_SPEEX,
    ID_WMAVOICE,
    ID_WMAPRO,
    ID_WMALOSSLESS,
    ID_ATRAC3P,
    ID_EAC3,
    ID_SIPR,
    ID_MP1,
    ID_TWINVQ,
    ID_TRUEHD,
    ID_MP4ALS,
    ID_ATRAC1,
    ID_BINKAUDIO_RDFT,
    ID_BINKAUDIO_DCT,
    ID_AAC_LATM,
    ID_QDMC,
    ID_CELT,
    ID_G723_1,
    ID_G729,
    ID_8SVX_EXP,
    ID_8SVX_FIB,
    ID_BMV_AUDIO,
    ID_RALF,
    ID_IAC,
    ID_ILBC,
    ID_OPUS_DEPRECATED,
    ID_COMFORT_NOISE,
    ID_TAK_DEPRECATED,
    ID_FFWAVESYNTH = MKBETAG('F','F','W','S'),
#if LIBAVCODEC_VERSION_MAJOR <= 54
    ID_8SVX_RAW = MKBETAG('8','S','V','X'),
#endif
    ID_SONIC = MKBETAG('S','O','N','C'),
    ID_SONIC_LS = MKBETAG('S','O','N','L'),
    ID_PAF_AUDIO = MKBETAG('P','A','F','A'),
    ID_OPUS = MKBETAG('O','P','U','S'),
    ID_TAK = MKBETAG('t','B','a','K'),
    ID_EVRC = MKBETAG('s','e','v','c'),
    ID_SMV = MKBETAG('s','s','m','v'),

    /* subtitle codecs */
    ID_FIRST_SUBTITLE = 0x17000,
    ID_DVD_SUBTITLE = 0x17000,
    ID_DVB_SUBTITLE,
    ID_TEXT,
    ID_XSUB,
    ID_SSA,
    ID_MOV_TEXT,
    ID_HDMV_PGS_SUBTITLE,
    ID_DVB_TELETEXT,
    ID_SRT,
    ID_MICRODVD = MKBETAG('m','D','V','D'),
    ID_EIA_608 = MKBETAG('c','6','0','8'),
    ID_JACOSUB = MKBETAG('J','S','U','B'),
    ID_SAMI = MKBETAG('S','A','M','I'),
    ID_REALTEXT = MKBETAG('R','T','X','T'),
    ID_SUBVIEWER1 = MKBETAG('S','b','V','1'),
    ID_SUBVIEWER = MKBETAG('S','u','b','V'),
    ID_SUBRIP = MKBETAG('S','R','i','p'),
    ID_WEBVTT = MKBETAG('W','V','T','T'),
    ID_MPL2 = MKBETAG('M','P','L','2'),
    ID_VPLAYER = MKBETAG('V','P','l','r'),
    ID_PJS = MKBETAG('P','h','J','S'),

    /* other specific kind of codecs (generally used for attachments) */
    ID_FIRST_UNKNOWN = 0x18000,
    ID_TTF = 0x18000,
    ID_BINTEXT = MKBETAG('B','T','X','T'),
    ID_XBIN = MKBETAG('X','B','I','N'),
    ID_IDF = MKBETAG( 0 ,'I','D','F'),
    ID_OTF = MKBETAG( 0 ,'O','T','F'),
    ID_SMPTE_KLV = MKBETAG('K','L','V','A'),

    ID_PROBE = 0x19000,

    ID_MPEG2TS = 0x20000,
    ID_MPEG4SYSTEMS = 0x20001,
    ID_FFMETADATA = 0x21000,

  } Id;

  /**
   * Capabilities supported by a codec.
   */
  typedef enum CodecCapability
  {
    /** Decoder can use draw_horiz_band callback. */
    CAP_DRAW_HORIZ_BAND = CODEC_CAP_DRAW_HORIZ_BAND,
    /**
     * Codec uses get_buffer() for allocating buffers and supports custom allocators.
     * If not set, it might not use get_buffer() at all or use operations that
     * assume the buffer was allocated by avcodec_default_get_buffer.
     */
    CAP_DR1 = CODEC_CAP_DR1, CAP_TRUNCATED = CODEC_CAP_TRUNCATED,
    /* Codec can export data for HW decoding (XvMC). */
    CAP_HWACCEL = CODEC_CAP_HWACCEL,
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
    CAP_DELAY = CODEC_CAP_DELAY,
    /**
     * Codec can be fed a final frame with a smaller size.
     * This can be used to prevent truncation of the last audio samples.
     */
    CAP_SMALL_LAST_FRAME = CODEC_CAP_SMALL_LAST_FRAME,
    /**
     * Codec can export data for HW decoding (VDPAU).
     */
    CAP_HWACCEL_VDPAU = CODEC_CAP_HWACCEL_VDPAU,
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
    CAP_SUBFRAMES = CODEC_CAP_SUBFRAMES,
    /**
     * Codec is experimental and is thus avoided in favor of non experimental
     * encoders
     */
    CAP_EXPERIMENTAL = CODEC_CAP_EXPERIMENTAL,
    /**
     * Codec should fill in channel configuration and samplerate instead of container
     */
    CAP_CHANNEL_CONF = CODEC_CAP_CHANNEL_CONF,

    /**
     * Codec is able to deal with negative linesizes
     */
    CAP_NEG_LINESIZES = CODEC_CAP_NEG_LINESIZES,

    /**
     * Codec supports frame-level multithreading.
     */
    CAP_FRAME_THREADS = CODEC_CAP_FRAME_THREADS,
    /**
     * Codec supports slice-based (or partition-based) multithreading.
     */
    CAP_SLICE_THREADS = CODEC_CAP_SLICE_THREADS,
    /**
     * Codec supports changed parameters at any point.
     */
    CAP_PARAM_CHANGE = CODEC_CAP_PARAM_CHANGE,
    /**
     * Codec supports avctx->thread_count == 0 (auto).
     */
    CAP_AUTO_THREADS = CODEC_CAP_AUTO_THREADS,
    /**
     * Audio encoder supports receiving a different number of samples in each call.
     */
    CAP_VARIABLE_FRAME_SIZE = CODEC_CAP_VARIABLE_FRAME_SIZE,
    /**
     * Codec is intra only.
     */
    CAP_INTRA_ONLY = CODEC_CAP_INTRA_ONLY,
    /**
     * Codec is lossless.
     */
    CAP_LOSSLESS = CODEC_CAP_LOSSLESS,
  } CodecCapability;

  /**
   * Checks if this codec has the given capability.
   * @param c Capability to check.
   */
  virtual bool
  hasCapability(CodecCapability c)
  {
    bool retval = false;
    if (mCodec) retval = mCodec->capabilities & c;
    return retval;
  }

protected:
  Codec*
  make(AVCodec* codec)
  {
    Codec* retval = 0;
    if (codec)
    {
      retval = make();
      if (retval)
      {
        retval->mCodec = codec;
      }
    }
    return retval;
  }
private:
  Codec() :
      mCodec(0)
  {
  }
  virtual
  ~Codec()
  {
  }

  AVCodec* mCodec;
};

/**
 * Meta-data that describes a {@link Codec} object.
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
  hasProperty(CodecProperty p)
  {
    bool retval = false;
    if (mDescriptor) retval = p & mDescriptor->props;
    return retval;
  }

  /**
   * Name of the codec. Unique for each codec. Will contain only
   * alphanumeric and _ characters.
   */
  virtual const char*
  getName()
  {
    return mDescriptor->name;
  }
  /**
   * A more descriptive name for this codec.
   */
  virtual const char*
  getLongName()
  {
    return mDescriptor->long_name;
  }
  /**
   * A bit mask of {@link Codec.Properties} this codec has.
   */
  virtual int32_t
  getProperties()
  {
    return mDescriptor->props;
  }

  /** Get the codec ID for this descriptor */
  virtual Codec::Id
  getId()
  {
    return (Codec::Id) mDescriptor->id;
  }

  virtual MediaDescriptor::Type
  getType()
  {
    return (MediaDescriptor::Type) mDescriptor->type;
  }
  /**
   * Get the descriptor for the given id.
   */
  static CodecDescriptor* make(Codec::Id id)
  {
    const AVCodecDescriptor* d = avcodec_descriptor_get((enum AVCodecID)id);
    return make(d);
  }

#ifndef SWIG
  static CodecDescriptor*
  make(const AVCodecDescriptor *d)
  {
    CodecDescriptor* retval = 0;
    if (d)
    {
      retval = make();
      retval->mDescriptor = d;
    }
    return retval;
  }
#endif // ! SWIG
private:
  CodecDescriptor() :
      mDescriptor(0)
  {
  }
  virtual
  ~CodecDescriptor()
  {
  }
  const AVCodecDescriptor *mDescriptor;
};

}
}
}

#endif /* CODEC_H_ */
