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

#ifndef IPIXELFORMAT_H_
#define IPIXELFORMAT_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefCounted.h>

#ifdef PixelFormat
#undef PixelFormat
#endif // PixelFormat

namespace io {
namespace humble {
namespace video {
// From libavutil Version 49.6.0
//class IVideoPicture;

/**
 * Information about how video data is formatted in an {@link IVideoPicture} object.
 *
 * This specifies the color space and how many bits pixel data takes.  It also
 * includes some utility methods for dealing with {@link Type#YUV420P} data; the
 * most common type of encoding used in video files I've run across.
 */
#ifdef PixelFormat
#undef PixelFormat
#endif // PixelFormat

class VS_API_HUMBLEVIDEO PixelFormat : public io::humble::ferry::RefCounted
{
public:
  /**
   * Pixel format. Notes:
   *
   * RGB32 is handled in an endian-specific manner. A RGBA
   * color is put together as:
   *  (A << 24) | (R << 16) | (G << 8) | B
   * This is stored as BGRA on little endian CPU architectures and ARGB on
   * big endian CPUs.
   *
   * When the pixel format is palettized RGB (PAL8), the palettized
   * image data is stored in AVFrame.data[0]. The palette is transported in
   * AVFrame.data[1] and, is 1024 bytes long (256 4-byte entries) and is
   * formatted the same as in RGB32 described above (i.e., it is
   * also endian-specific). Note also that the individual RGB palette
   * components stored in AVFrame.data[1] should be in the range 0..255.
   * This is important as many custom PAL8 video codecs that were designed
   * to run on the IBM VGA graphics adapter use 6-bit palette components.
   */
  typedef enum Type
  {
    /** None */FMT_NONE = -1,
    /**  planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples) */FMT_YUV420P,
    /**  packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr */FMT_YUYV422,
    /**  packed RGB 8:8:8, 24bpp, RGBRGB... */FMT_RGB24,
    /**  packed RGB 8:8:8, 24bpp, BGRBGR... */FMT_BGR24,
    /**  planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples) */FMT_YUV422P,
    /**  planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples) */FMT_YUV444P,
    /**  planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples) */FMT_YUV410P,
    /**  planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) */FMT_YUV411P,
    /**         Y        ,  8bpp */FMT_GRAY8,
    /**         Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb */FMT_MONOWHITE,
    /**         Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb */FMT_MONOBLACK,
    /**  8 bit with PIX_FMT_RGB32 palette */FMT_PAL8,
    /**  planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV420P and setting color_range */FMT_YUVJ420P,
    /**  planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV422P and setting color_range */FMT_YUVJ422P,
    /**  planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV444P and setting color_range */FMT_YUVJ444P,
    /**  XVideo Motion Acceleration via common packet passing */FMT_XVMC_MPEG2_MC,
    FMT_XVMC_MPEG2_IDCT,
    /**  packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1 */FMT_UYVY422,
    /**  packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3 */FMT_UYYVYY411,
    /**  packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb) */FMT_BGR8,
    /**  packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits */FMT_BGR4,
    /**  packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb) */FMT_BGR4_BYTE,
    /**  packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb) */FMT_RGB8,
    /**  packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits */FMT_RGB4,
    /**  packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb) */FMT_RGB4_BYTE,
    /**  planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V) */FMT_NV12,
    /**  as above, but U and V bytes are swapped */FMT_NV21,

    /**  packed ARGB 8:8:8:8, 32bpp, ARGBARGB... */FMT_ARGB,
    /**  packed RGBA 8:8:8:8, 32bpp, RGBARGBA... */FMT_RGBA,
    /**  packed ABGR 8:8:8:8, 32bpp, ABGRABGR... */FMT_ABGR,
    /**  packed BGRA 8:8:8:8, 32bpp, BGRABGRA... */FMT_BGRA,

    /**         Y        , 16bpp, big-endian */FMT_GRAY16BE,
    /**         Y        , 16bpp, little-endian */FMT_GRAY16LE,
    /**  planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples) */FMT_YUV440P,
    /**  planar YUV 4:4:0 full scale (JPEG), deprecated in favor of PIX_FMT_YUV440P and setting color_range */FMT_YUVJ440P,
    /**  planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples) */FMT_YUVA420P,
    /**  H.264 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VDPAU_H264,
    /**  MPEG-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VDPAU_MPEG1,
    /**  MPEG-2 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VDPAU_MPEG2,
    /**  WMV3 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VDPAU_WMV3,
    /**  VC-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VDPAU_VC1,
    /**  packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian */FMT_RGB48BE,
    /**  packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian */FMT_RGB48LE,

    /**  packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian */FMT_RGB565BE,
    /**  packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian */FMT_RGB565LE,
    /**  packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0 */FMT_RGB555BE,
    /**  packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0 */FMT_RGB555LE,

    /**  packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian */FMT_BGR565BE,
    /**  packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian */FMT_BGR565LE,
    /**  packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), big-endian, most significant bit to 1 */FMT_BGR555BE,
    /**  packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), little-endian, most significant bit to 1 */FMT_BGR555LE,

    /**  HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers */FMT_VAAPI_MOCO,
    /**  HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers */FMT_VAAPI_IDCT,
    /**  HW decoding through VA API, Picture.data[3] contains a vaapi_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VAAPI_VLD,

    /**  planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */FMT_YUV420P16LE,
    /**  planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */FMT_YUV420P16BE,
    /**  planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */FMT_YUV422P16LE,
    /**  planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */FMT_YUV422P16BE,
    /**  planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */FMT_YUV444P16LE,
    /**  planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */FMT_YUV444P16BE,
    /**  MPEG4 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */FMT_VDPAU_MPEG4,
    /**  HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer */FMT_DXVA2_VLD,

    /**  packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), little-endian, most significant bits to 0 */FMT_RGB444LE,
    /**  packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), big-endian, most significant bits to 0 */FMT_RGB444BE,
    /**  packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), little-endian, most significant bits to 1 */FMT_BGR444LE,
    /**  packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), big-endian, most significant bits to 1 */FMT_BGR444BE,
    /**  8bit gray, 8bit alpha */FMT_GRAY8A,
    /**  packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian */FMT_BGR48BE,
    /**  packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian */FMT_BGR48LE,

    //the following 10 formats have the disadvantage of needing 1 format for each bit depth, thus
    //If you want to support multiple bit depths, then using FMT_YUV420P16* with the bpp stored separately
    //is better
    /**  planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */FMT_YUV420P9BE,
    /**  planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */FMT_YUV420P9LE,
    /**  planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */FMT_YUV420P10BE,
    /**  planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */FMT_YUV420P10LE,
    /**  planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */FMT_YUV422P10BE,
    /**  planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */FMT_YUV422P10LE,
    /**  planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */FMT_YUV444P9BE,
    /**  planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */FMT_YUV444P9LE,
    /**  planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */FMT_YUV444P10BE,
    /**  planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */FMT_YUV444P10LE,
    /**  planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */FMT_YUV422P9BE,
    /**  planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */FMT_YUV422P9LE,
    /**  hardware decoding through VDA */FMT_VDA_VLD,

#ifdef FMT_ABI_GIT_MASTER
    /**  packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */FMT_RGBA64BE,
    /**  packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */FMT_RGBA64LE,
    /**  packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */FMT_BGRA64BE,
    /**  packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */FMT_BGRA64LE,
#endif
    /**  planar GBR 4:4:4 24bpp */FMT_GBRP,
    /**  planar GBR 4:4:4 27bpp, big-endian */FMT_GBRP9BE,
    /**  planar GBR 4:4:4 27bpp, little-endian */FMT_GBRP9LE,
    /**  planar GBR 4:4:4 30bpp, big-endian */FMT_GBRP10BE,
    /**  planar GBR 4:4:4 30bpp, little-endian */FMT_GBRP10LE,
    /**  planar GBR 4:4:4 48bpp, big-endian */FMT_GBRP16BE,
    /**  planar GBR 4:4:4 48bpp, little-endian */FMT_GBRP16LE,

    /**  planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples) */FMT_YUVA422P_LIBAV,
    /**  planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples) */FMT_YUVA444P_LIBAV,

    /**  planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian */FMT_YUVA420P9BE,
    /**  planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian */FMT_YUVA420P9LE,
    /**  planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian */FMT_YUVA422P9BE,
    /**  planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian */FMT_YUVA422P9LE,
    /**  planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian */FMT_YUVA444P9BE,
    /**  planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian */FMT_YUVA444P9LE,
    /**  planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian) */FMT_YUVA420P10BE,
    /**  planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian) */FMT_YUVA420P10LE,
    /**  planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian) */FMT_YUVA422P10BE,
    /**  planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian) */FMT_YUVA422P10LE,
    /**  planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian) */FMT_YUVA444P10BE,
    /**  planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian) */FMT_YUVA444P10LE,
    /**  planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian) */FMT_YUVA420P16BE,
    /**  planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian) */FMT_YUVA420P16LE,
    /**  planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian) */FMT_YUVA422P16BE,
    /**  planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian) */FMT_YUVA422P16LE,
    /**  planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian) */FMT_YUVA444P16BE,
    /**  planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian) */FMT_YUVA444P16LE,

    /**  HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface */FMT_VDPAU,

#ifndef FMT_ABI_GIT_MASTER
    /**  packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */FMT_RGBA64BE = 0x123,
    /**  packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */FMT_RGBA64LE,
    /**  packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */FMT_BGRA64BE,
    /**  packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */FMT_BGRA64LE,
#endif
    /**  packed RGB 8:8:8, 32bpp, 0RGB0RGB... */FMT_0RGB = 0x123 + 4,
    /**  packed RGB 8:8:8, 32bpp, RGB0RGB0... */FMT_RGB0,
    /**  packed BGR 8:8:8, 32bpp, 0BGR0BGR... */FMT_0BGR,
    /**  packed BGR 8:8:8, 32bpp, BGR0BGR0... */FMT_BGR0,
    /**  planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples) */FMT_YUVA444P,
    /**  planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples) */FMT_YUVA422P,

    /**  planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */FMT_YUV420P12BE,
    /**  planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */FMT_YUV420P12LE,
    /**  planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */FMT_YUV420P14BE,
    /**  planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */FMT_YUV420P14LE,
    /**  planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */FMT_YUV422P12BE,
    /**  planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */FMT_YUV422P12LE,
    /**  planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */FMT_YUV422P14BE,
    /**  planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */FMT_YUV422P14LE,
    /**  planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */FMT_YUV444P12BE,
    /**  planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */FMT_YUV444P12LE,
    /**  planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */FMT_YUV444P14BE,
    /**  planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */FMT_YUV444P14LE,
    /**  planar GBR 4:4:4 36bpp, big-endian */FMT_GBRP12BE,
    /**  planar GBR 4:4:4 36bpp, little-endian */FMT_GBRP12LE,
    /**  planar GBR 4:4:4 42bpp, big-endian */FMT_GBRP14BE,
    /**  planar GBR 4:4:4 42bpp, little-endian */FMT_GBRP14LE,
  } Type;

  typedef enum YUVColorComponent
  {
    YUV_Y = 0, YUV_U = 1, YUV_V = 2,
  } YUVColorComponent;

#if 0
  /**
   * Returns the byte for the coordinates at x and y for the color component c.
   *
   * @param frame The frame to get the byte from
   * @param x X coordinate in pixels, where 0 is the left hand edge of the image.
   * @param y Y coordinate in pixels, where 0 is the top edge of the image.
   * @param c YUVColor component
   *
   * @throws std::exception frame is null, the coordinates are invalid, or if the pixel format is not YUV420P
   *
   * @return the pixel byte for that x, y, c combination
   */
  static unsigned char
  getYUV420PPixel(IVideoPicture *frame, int x, int y,
      YUVColorComponent c);

  /**
   * Sets the value of the color component c at the coordinates x and y in the given frame.
   *
   * @param frame The frame to set the byte in
   * @param x X coordinate in pixels, where 0 is the left hand edge of the image.
   * @param y Y coordinate in pixels, where 0 is the top edge of the image.
   * @param c YUVColor component to set
   * @param value The new value of that pixel color component
   *
   * @throws std::exception frame is null, the coordinates are invalid, or if the pixel format is not YUV420P
   */
  static void
  setYUV420PPixel(IVideoPicture *frame, int x, int y, YUVColorComponent c,
      unsigned char value);

  /**
   * For a given x and y in a frame, and a given color components, this method
   * tells you how far into the actual data you'd have to go to find the byte that
   * represents that color/coordinate combination.
   *
   * @param frame The frame to get the byte from
   * @param x X coordinate in pixels, where 0 is the left hand edge of the image.
   * @param y Y coordinate in pixels, where 0 is the top edge of the image.
   * @param c YUVColor component
   *
   * @throws std::exception frame is null, the coordinates are invalid, or if the pixel format is not YUV420P
   *
   * @return the offset in bytes, starting from the start of the frame data, where
   *   the data for this pixel resides.
   */
  static int
  getYUV420PPixelOffset(IVideoPicture *frame, int x, int y,
      YUVColorComponent c);
#endif // 0
protected:
  PixelFormat();
  virtual
  ~PixelFormat();

};
}
}
}

#endif /*IPIXELFORMAT_H_*/
