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

class PixelComponentDescriptor;
class PixelFormatDescriptor;

/**
 * Picture formats and various static member functions for getting
 * meta data about them.
 */
class VS_API_HUMBLEVIDEO PixelFormat : public io::humble::ferry::RefCounted
{
public:
  /**
   * Pixel format.
   *
   * @note
   * PIX_FMT_RGB32 is handled in an endian-specific manner. An RGBA
   * color is put together as:
   *  (A << 24) | (R << 16) | (G << 8) | B
   * This is stored as BGRA on little-endian CPU architectures and ARGB on
   * big-endian CPUs.
   *
   * @par
   * When the pixel format is palettized RGB (PIX_FMT_PAL8), the palettized
   * image data is stored in AVFrame.data[0]. The palette is transported in
   * AVFrame.data[1], is 1024 bytes long (256 4-byte entries) and is
   * formatted the same as in PIX_FMT_RGB32 described above (i.e., it is
   * also endian-specific). Note also that the individual RGB palette
   * components stored in AVFrame.data[1] should be in the range 0..255.
   * This is important as many custom PAL8 video codecs that were designed
   * to run on the IBM VGA graphics adapter use 6-bit palette components.
   *
   * @par
   * For all the 8bit per pixel formats, an RGB32 palette is in data[1] like
   * for pal8. This palette is filled in automatically by the function
   * allocating the picture.
   *
   * @note
   * Make sure that all newly added big-endian formats have pix_fmt & 1 == 1
   * and that all newly added little-endian formats have pix_fmt & 1 == 0.
   * This allows simpler detection of big vs little-endian.
   */
  typedef enum Type
  {
    PIX_FMT_NONE = -1,
    /** planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples) */
    PIX_FMT_YUV420P = AV_PIX_FMT_YUV420P,
    /** packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr */
    PIX_FMT_YUYV422 = AV_PIX_FMT_YUYV422,
    /** packed RGB 8:8:8, 24bpp, RGBRGB... */
    PIX_FMT_RGB24 = AV_PIX_FMT_RGB24,
    /** packed RGB 8:8:8, 24bpp, BGRBGR... */
    PIX_FMT_BGR24 = AV_PIX_FMT_BGR24,
    /** planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples) */
    PIX_FMT_YUV422P = AV_PIX_FMT_YUV422P,
    /** planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples) */
    PIX_FMT_YUV444P = AV_PIX_FMT_YUV444P,
    /** planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples) */
    PIX_FMT_YUV410P = AV_PIX_FMT_YUV410P,
    /** planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) */
    PIX_FMT_YUV411P = AV_PIX_FMT_YUV411P,
    /**        Y        ,  8bpp */
    PIX_FMT_GRAY8 = AV_PIX_FMT_GRAY8,
    /**        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb */
    PIX_FMT_MONOWHITE = AV_PIX_FMT_MONOWHITE,
    /**        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb */
    PIX_FMT_MONOBLACK = AV_PIX_FMT_MONOBLACK,
    /** 8 bit with PIX_FMT_RGB32 palette */
    PIX_FMT_PAL8 = AV_PIX_FMT_PAL8,
    /** planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV420P and setting color_range */
    PIX_FMT_YUVJ420P = AV_PIX_FMT_YUVJ420P,
    /** planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV422P and setting color_range */
    PIX_FMT_YUVJ422P = AV_PIX_FMT_YUVJ422P,
    /** planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV444P and setting color_range */
    PIX_FMT_YUVJ444P = AV_PIX_FMT_YUVJ444P,
    /** XVideo Motion Acceleration via common packet passing */
    PIX_FMT_XVMC_MPEG2_MC = AV_PIX_FMT_XVMC_MPEG2_MC,

    PIX_FMT_XVMC_MPEG2_IDCT = AV_PIX_FMT_XVMC_MPEG2_IDCT,
    /** packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1 */
    PIX_FMT_UYVY422 = AV_PIX_FMT_UYVY422,
    /** packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3 */
    PIX_FMT_UYYVYY411 = AV_PIX_FMT_UYYVYY411,
    /** packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb) */
    PIX_FMT_BGR8 = AV_PIX_FMT_BGR8,
    /** packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits */
    PIX_FMT_BGR4 = AV_PIX_FMT_BGR4,
    /** packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb) */
    PIX_FMT_BGR4_BYTE = AV_PIX_FMT_BGR4_BYTE,
    /** packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb) */
    PIX_FMT_RGB8 = AV_PIX_FMT_RGB8,
    /** packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits */
    PIX_FMT_RGB4 = AV_PIX_FMT_RGB4,
    /** packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb) */
    PIX_FMT_RGB4_BYTE = AV_PIX_FMT_RGB4_BYTE,
    /** planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V) */
    PIX_FMT_NV12 = AV_PIX_FMT_NV12,
    /** as above, but U and V bytes are swapped */
    PIX_FMT_NV21 = AV_PIX_FMT_NV21,

    /** packed ARGB 8:8:8:8, 32bpp, ARGBARGB... */
    PIX_FMT_ARGB = AV_PIX_FMT_ARGB,
    /** packed RGBA 8:8:8:8, 32bpp, RGBARGBA... */
    PIX_FMT_RGBA = AV_PIX_FMT_RGBA,
    /** packed ABGR 8:8:8:8, 32bpp, ABGRABGR... */
    PIX_FMT_ABGR = AV_PIX_FMT_ABGR,
    /** packed BGRA 8:8:8:8, 32bpp, BGRABGRA... */
    PIX_FMT_BGRA = AV_PIX_FMT_BGRA,

    /**        Y        , 16bpp, big-endian */
    PIX_FMT_GRAY16BE = AV_PIX_FMT_GRAY16BE,
    /**        Y        , 16bpp, little-endian */
    PIX_FMT_GRAY16LE = AV_PIX_FMT_GRAY16LE,
    /** planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples) */
    PIX_FMT_YUV440P = AV_PIX_FMT_YUV440P,
    /** planar YUV 4:4:0 full scale (JPEG), deprecated in favor of PIX_FMT_YUV440P and setting color_range */
    PIX_FMT_YUVJ440P = AV_PIX_FMT_YUVJ440P,
    /** planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples) */
    PIX_FMT_YUVA420P = AV_PIX_FMT_YUVA420P,
    /** H.264 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VDPAU_H264 = AV_PIX_FMT_VDPAU_H264,
    /** MPEG-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VDPAU_MPEG1 = AV_PIX_FMT_VDPAU_MPEG1,
    /** MPEG-2 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VDPAU_MPEG2 = AV_PIX_FMT_VDPAU_MPEG2,
    /** WMV3 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VDPAU_WMV3 = AV_PIX_FMT_VDPAU_WMV3,
    /** VC-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VDPAU_VC1 = AV_PIX_FMT_VDPAU_VC1,
    /** packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian */
    PIX_FMT_RGB48BE = AV_PIX_FMT_RGB48BE,
    /** packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian */
    PIX_FMT_RGB48LE = AV_PIX_FMT_RGB48LE,

    /** packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian */
    PIX_FMT_RGB565BE = AV_PIX_FMT_RGB565BE,
    /** packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian */
    PIX_FMT_RGB565LE = AV_PIX_FMT_RGB565LE,
    /** packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0 */
    PIX_FMT_RGB555BE = AV_PIX_FMT_RGB555BE,
    /** packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0 */
    PIX_FMT_RGB555LE = AV_PIX_FMT_RGB555LE,

    /** packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian */
    PIX_FMT_BGR565BE = AV_PIX_FMT_BGR565BE,
    /** packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian */
    PIX_FMT_BGR565LE = AV_PIX_FMT_BGR565LE,
    /** packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), big-endian, most significant bit to 1 */
    PIX_FMT_BGR555BE = AV_PIX_FMT_BGR555BE,
    /** packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), little-endian, most significant bit to 1 */
    PIX_FMT_BGR555LE = AV_PIX_FMT_BGR555LE,

    /** HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers */
    PIX_FMT_VAAPI_MOCO = AV_PIX_FMT_VAAPI_MOCO,
    /** HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers */
    PIX_FMT_VAAPI_IDCT = AV_PIX_FMT_VAAPI_IDCT,
    /** HW decoding through VA API, Picture.data[3] contains a vaapi_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VAAPI_VLD = AV_PIX_FMT_VAAPI_VLD,

    /** planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */
    PIX_FMT_YUV420P16LE = AV_PIX_FMT_YUV420P16LE,
    /** planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */
    PIX_FMT_YUV420P16BE = AV_PIX_FMT_YUV420P16BE,
    /** planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */
    PIX_FMT_YUV422P16LE = AV_PIX_FMT_YUV422P16LE,
    /** planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */
    PIX_FMT_YUV422P16BE = AV_PIX_FMT_YUV422P16BE,
    /** planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */
    PIX_FMT_YUV444P16LE = AV_PIX_FMT_YUV444P16LE,
    /** planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */
    PIX_FMT_YUV444P16BE = AV_PIX_FMT_YUV444P16BE,
    /** MPEG4 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers */
    PIX_FMT_VDPAU_MPEG4 = AV_PIX_FMT_VDPAU_MPEG4,
    /** HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer */
    PIX_FMT_DXVA2_VLD = AV_PIX_FMT_DXVA2_VLD,

    /** packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), little-endian, most significant bits to 0 */
    PIX_FMT_RGB444LE = AV_PIX_FMT_RGB444LE,
    /** packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), big-endian, most significant bits to 0 */
    PIX_FMT_RGB444BE = AV_PIX_FMT_RGB444BE,
    /** packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), little-endian, most significant bits to 1 */
    PIX_FMT_BGR444LE = AV_PIX_FMT_BGR444LE,
    /** packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), big-endian, most significant bits to 1 */
    PIX_FMT_BGR444BE = AV_PIX_FMT_BGR444BE,
    /** 8bit gray, 8bit alpha */
    PIX_FMT_GRAY8A = AV_PIX_FMT_GRAY8A,
    /** packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian */
    PIX_FMT_BGR48BE = AV_PIX_FMT_BGR48BE,
    /** packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian */
    PIX_FMT_BGR48LE = AV_PIX_FMT_BGR48LE,

    //the following 10 formats have the disadvantage of needing 1 format for each bit depth, thus
    //If you want to support multiple bit depths, then using AV_PIX_FMT_YUV420P16* with the bpp stored separately
    //is better
    /** planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */
    PIX_FMT_YUV420P9BE = AV_PIX_FMT_YUV420P9BE,
    /** planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */
    PIX_FMT_YUV420P9LE = AV_PIX_FMT_YUV420P9LE,
    /** planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */
    PIX_FMT_YUV420P10BE = AV_PIX_FMT_YUV420P10BE,
    /** planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */
    PIX_FMT_YUV420P10LE = AV_PIX_FMT_YUV420P10LE,
    /** planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */
    PIX_FMT_YUV422P10BE = AV_PIX_FMT_YUV422P10BE,
    /** planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */
    PIX_FMT_YUV422P10LE = AV_PIX_FMT_YUV422P10LE,
    /** planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */
    PIX_FMT_YUV444P9BE = AV_PIX_FMT_YUV444P9BE,
    /** planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */
    PIX_FMT_YUV444P9LE = AV_PIX_FMT_YUV444P9LE,
    /** planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */
    PIX_FMT_YUV444P10BE = AV_PIX_FMT_YUV444P10BE,
    /** planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */
    PIX_FMT_YUV444P10LE = AV_PIX_FMT_YUV444P10LE,
    /** planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */
    PIX_FMT_YUV422P9BE = AV_PIX_FMT_YUV422P9BE,
    /** planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */
    PIX_FMT_YUV422P9LE = AV_PIX_FMT_YUV422P9LE,
    /** hardware decoding through VDA */
    PIX_FMT_VDA_VLD = AV_PIX_FMT_VDA_VLD,

#ifdef AV_PIX_FMT_ABI_GIT_MASTER
    /** packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */
    PIX_FMT_RGBA64BE = AV_PIX_FMT_RGBA64BE,
    /** packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */
    PIX_FMT_RGBA64LE = AV_PIX_FMT_RGBA64LE,
    /** packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */
    PIX_FMT_BGRA64BE = AV_PIX_FMT_BGRA64BE,
    /** packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */
    PIX_FMT_BGRA64LE = AV_PIX_FMT_BGRA64LE,
#endif
    /** planar GBR 4:4:4 24bpp */
    PIX_FMT_GBRP = AV_PIX_FMT_GBRP,
    /** planar GBR 4:4:4 27bpp, big-endian */
    PIX_FMT_GBRP9BE = AV_PIX_FMT_GBRP9BE,
    /** planar GBR 4:4:4 27bpp, little-endian */
    PIX_FMT_GBRP9LE = AV_PIX_FMT_GBRP9LE,
    /** planar GBR 4:4:4 30bpp, big-endian */
    PIX_FMT_GBRP10BE = AV_PIX_FMT_GBRP10BE,
    /** planar GBR 4:4:4 30bpp, little-endian */
    PIX_FMT_GBRP10LE = AV_PIX_FMT_GBRP10LE,
    /** planar GBR 4:4:4 48bpp, big-endian */
    PIX_FMT_GBRP16BE = AV_PIX_FMT_GBRP16BE,
    /** planar GBR 4:4:4 48bpp, little-endian */
    PIX_FMT_GBRP16LE = AV_PIX_FMT_GBRP16LE,

    /** planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples) */
    PIX_FMT_YUVA422P_LIBAV = AV_PIX_FMT_YUVA422P_LIBAV,

    /** planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples) */
    PIX_FMT_YUVA444P_LIBAV = AV_PIX_FMT_YUVA444P_LIBAV,

    /** planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian */
    PIX_FMT_YUVA420P9BE = AV_PIX_FMT_YUVA420P9BE,
    /** planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian */
    PIX_FMT_YUVA420P9LE = AV_PIX_FMT_YUVA420P9LE,
    /** planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian */
    PIX_FMT_YUVA422P9BE = AV_PIX_FMT_YUVA422P9BE,
    /** planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian */
    PIX_FMT_YUVA422P9LE = AV_PIX_FMT_YUVA422P9LE,
    /** planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian */
    PIX_FMT_YUVA444P9BE = AV_PIX_FMT_YUVA444P9BE,
    /** planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian */
    PIX_FMT_YUVA444P9LE = AV_PIX_FMT_YUVA444P9LE,
    /** planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian) */
    PIX_FMT_YUVA420P10BE = AV_PIX_FMT_YUVA420P10BE,
    /** planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian) */
    PIX_FMT_YUVA420P10LE = AV_PIX_FMT_YUVA420P10LE,
    /** planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian) */
    PIX_FMT_YUVA422P10BE = AV_PIX_FMT_YUVA422P10BE,
    /** planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian) */
    PIX_FMT_YUVA422P10LE = AV_PIX_FMT_YUVA422P10LE,
    /** planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian) */
    PIX_FMT_YUVA444P10BE = AV_PIX_FMT_YUVA444P10BE,
    /** planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian) */
    PIX_FMT_YUVA444P10LE = AV_PIX_FMT_YUVA444P10LE,
    /** planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian) */
    PIX_FMT_YUVA420P16BE = AV_PIX_FMT_YUVA420P16BE,
    /** planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian) */
    PIX_FMT_YUVA420P16LE = AV_PIX_FMT_YUVA420P16LE,
    /** planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian) */
    PIX_FMT_YUVA422P16BE = AV_PIX_FMT_YUVA422P16BE,
    /** planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian) */
    PIX_FMT_YUVA422P16LE = AV_PIX_FMT_YUVA422P16LE,
    /** planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian) */
    PIX_FMT_YUVA444P16BE = AV_PIX_FMT_YUVA444P16BE,
    /** planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian) */
    PIX_FMT_YUVA444P16LE = AV_PIX_FMT_YUVA444P16LE,

    /** HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface */
    PIX_FMT_VDPAU = AV_PIX_FMT_VDPAU,

    /** packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0 */
    PIX_FMT_XYZ12LE = AV_PIX_FMT_XYZ12LE,
    /** packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0 */
    PIX_FMT_XYZ12BE = AV_PIX_FMT_XYZ12BE,

#ifndef AV_PIX_FMT_ABI_GIT_MASTER
    /** packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */
    PIX_FMT_RGBA64BE = AV_PIX_FMT_RGBA64BE,
    /** packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */
    PIX_FMT_RGBA64LE = AV_PIX_FMT_RGBA64LE,
    /** packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian */
    PIX_FMT_BGRA64BE = AV_PIX_FMT_BGRA64BE,
    /** packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian */
    PIX_FMT_BGRA64LE = AV_PIX_FMT_BGRA64LE,
#endif
    /** packed RGB 8:8:8, 32bpp, 0RGB0RGB...*/
    PIX_FMT_0RGB = AV_PIX_FMT_0RGB,
    /** packed RGB 8:8:8, 32bpp, RGB0RGB0... */
    PIX_FMT_RGB0 = AV_PIX_FMT_RGB0,
    /** packed BGR 8:8:8, 32bpp, 0BGR0BGR... */
    PIX_FMT_0BGR = AV_PIX_FMT_0BGR,
    /** packed BGR 8:8:8, 32bpp, BGR0BGR0... */
    PIX_FMT_BGR0 = AV_PIX_FMT_BGR0,
    /** planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples) */
    PIX_FMT_YUVA444P = AV_PIX_FMT_YUVA444P,
    /** planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples) */
    PIX_FMT_YUVA422P = AV_PIX_FMT_YUVA422P,

    /** planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */
    PIX_FMT_YUV420P12BE = AV_PIX_FMT_YUV420P12BE,
    /** planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */
    PIX_FMT_YUV420P12LE = AV_PIX_FMT_YUV420P12LE,
    /** planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian */
    PIX_FMT_YUV420P14BE = AV_PIX_FMT_YUV420P14BE,
    /** planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian */
    PIX_FMT_YUV420P14LE = AV_PIX_FMT_YUV420P14LE,
    /** planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */
    PIX_FMT_YUV422P12BE = AV_PIX_FMT_YUV422P12BE,
    /** planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */
    PIX_FMT_YUV422P12LE = AV_PIX_FMT_YUV422P12LE,
    /** planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian */
    PIX_FMT_YUV422P14BE = AV_PIX_FMT_YUV422P14BE,
    /** planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian */
    PIX_FMT_YUV422P14LE = AV_PIX_FMT_YUV422P14LE,
    /** planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */
    PIX_FMT_YUV444P12BE = AV_PIX_FMT_YUV444P12BE,
    /** planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */
    PIX_FMT_YUV444P12LE = AV_PIX_FMT_YUV444P12LE,
    /** planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian */
    PIX_FMT_YUV444P14BE = AV_PIX_FMT_YUV444P14BE,
    /** planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian */
    PIX_FMT_YUV444P14LE = AV_PIX_FMT_YUV444P14LE,
    /** planar GBR 4:4:4 36bpp, big-endian */
    PIX_FMT_GBRP12BE = AV_PIX_FMT_GBRP12BE,
    /** planar GBR 4:4:4 36bpp, little-endian */
    PIX_FMT_GBRP12LE = AV_PIX_FMT_GBRP12LE,
    /** planar GBR 4:4:4 42bpp, big-endian */
    PIX_FMT_GBRP14BE = AV_PIX_FMT_GBRP14BE,
    /** planar GBR 4:4:4 42bpp, little-endian */
    PIX_FMT_GBRP14LE = AV_PIX_FMT_GBRP14LE,
    /** planar GBRA 4:4:4:4 32bpp */
    PIX_FMT_GBRAP = AV_PIX_FMT_GBRAP,
    /** planar GBRA 4:4:4:4 64bpp, big-endian */
    PIX_FMT_GBRAP16BE = AV_PIX_FMT_GBRAP16BE,
    /** planar GBRA 4:4:4:4 64bpp, little-endian */
    PIX_FMT_GBRAP16LE = AV_PIX_FMT_GBRAP16LE,
    /** planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of PIX_FMT_YUV411P and setting color_range */
    PIX_FMT_YUVJ411P = AV_PIX_FMT_YUVJ411P,
    /** number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions */
    PIX_FMT_NB = AV_PIX_FMT_NB,
  } Type;

  /**
   * Return the pixel format corresponding to name.
   *
   * If there is no pixel format with name name, then looks for a
   * pixel format with the name corresponding to the native endian
   * format of name.
   * For example in a little-endian system, first looks for "gray16",
   * then for "gray16le".
   *
   * Finally if no pixel format has been found, returns AV_PIX_FMT_NONE.
   */
  static PixelFormat::Type
  getFormat(const char* name) {
    return
        name && *name ?
            (PixelFormat::Type) av_get_pix_fmt(name) : PixelFormat::PIX_FMT_NONE;
  }

  /**
   * Return the short name for a pixel format, NULL in case pix_fmt is
   * unknown.
   *
   */
  static const char*
  getFormatName(PixelFormat::Type pix_fmt) {
    return av_get_pix_fmt_name((enum AVPixelFormat) pix_fmt);
  }

  /**
   * @return a pixel format descriptor for provided pixel format or NULL if
   * this pixel format is unknown.
   */
  static PixelFormatDescriptor*
  getDescriptor(PixelFormat::Type pix_fmt);

  /**
   * Returns the total number of pixel format descriptors known to humble video.
   */
  static int32_t
  getNumInstalledFormats();

  /**
   * Returns the 'i'th pixel format descriptor that is known to humble video
   *
   * @param i The i'th pixel format descriptor in the list of installed descriptors.
   */
  static PixelFormatDescriptor*
  getInstalledFormatDescriptor(int32_t i);

  /**
   * @return number of planes in pix_fmt, a negative ERROR if pix_fmt is not a
   * valid pixel format.
   */
  static int32_t
  getNumPlanes(PixelFormat::Type pix_fmt) {
    return av_pix_fmt_count_planes((enum AVPixelFormat) pix_fmt);
  }

  /**
   * Utility function to swap the endianness of a pixel format.
   *
   * @param[in]  pix_fmt the pixel format
   *
   * @return pixel format with swapped endianness if it exists,
   * otherwise AV_PIX_FMT_NONE
   */
  static PixelFormat::Type
  swapEndianness(PixelFormat::Type pix_fmt) {
    return (PixelFormat::Type) av_pix_fmt_swap_endianness(
        (enum AVPixelFormat) pix_fmt);
  }

private:
  // make private
  PixelFormat() {
  }
  virtual
  ~PixelFormat() {
  }
};

class PixelComponentDescriptor : public io::humble::ferry::RefCounted
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(PixelComponentDescriptor)
public:
  /** which of the 4 planes contains the component */
  uint16_t
  getPlane() {
    return mCtx->plane;
  }

  /**
   * Number of elements between 2 horizontally consecutive pixels minus 1.
   * Elements are bits for bitstream formats, bytes otherwise.
   */
  uint16_t
  getStepMinus1() {
    return mCtx->step_minus1;
  }

  /**
   * Number of elements before the component of the first pixel plus 1.
   * Elements are bits for bitstream formats, bytes otherwise.
   */
  uint16_t
  getOffsetPlus1() {
    return mCtx->offset_plus1;
  }
  /** number of least significant bits that must be shifted away to get the value */
  uint16_t
  getShift() {
    return mCtx->shift;
  }
  /** number of bits in the component minus 1 */
  uint16_t
  getDepthMinus1() {
    return mCtx->depth_minus1;
  }
#ifndef SWIG
  static PixelComponentDescriptor*
  make(const AVComponentDescriptor* ctx);
#endif // ! SWIG
private:
  PixelComponentDescriptor() :
      mCtx(0) {
  }
  virtual
  ~PixelComponentDescriptor() {
  }
  const AVComponentDescriptor* mCtx;
};

/**
 * Descriptor that unambiguously describes how the bits of a pixel are
 * stored in the up to 4 data planes of an image. It also stores the
 * subsampling factors and number of components.
 *
 * @note This is separate of the colorspace (RGB, YCbCr, YPbPr, JPEG-style YUV
 *       and all the YUV variants) AVPixFmtDescriptor just stores how values
 *       are stored not what these values represent.
 */
class PixelFormatDescriptor : public io::humble::ferry::RefCounted
{
VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(PixelFormatDescriptor)
public:
  typedef enum Flag
  {
    /**
     * Pixel format is big-endian.
     */
    PIX_FMT_FLAG_BE = AV_PIX_FMT_FLAG_BE,
    /**
     * Pixel format has a palette in data[1], values are indexes in this palette.
     */
    PIX_FMT_FLAG_PAL = AV_PIX_FMT_FLAG_PAL,
    /**
     * All values of a component are bit-wise packed end to end.
     */
    PIX_FMT_FLAG_BITSTREAM = AV_PIX_FMT_FLAG_BITSTREAM,
    /**
     * Pixel format is an HW accelerated format.
     */
    PIX_FMT_FLAG_HWACCEL = AV_PIX_FMT_FLAG_HWACCEL,
    /**
     * At least one pixel component is not in the first data plane.
     */
    PIX_FMT_FLAG_PLANAR = AV_PIX_FMT_FLAG_PLANAR,
    /**
     * The pixel format contains RGB-like data (as opposed to YUV/grayscale).
     */
    PIX_FMT_FLAG_RGB = AV_PIX_FMT_FLAG_RGB,
    /**
     * The pixel format is "pseudo-paletted". This means that FFmpeg treats it as
     * paletted internally, but the palette is generated by the decoder and is not
     * stored in the file.
     */
    PIX_FMT_FLAG_PSEUDOPAL = AV_PIX_FMT_FLAG_PSEUDOPAL,
    /**
     * The pixel format has an alpha channel.
     */
    PIX_FMT_FLAG_ALPHA = AV_PIX_FMT_FLAG_ALPHA,
  } Flag;

  /** Get the name of this pixel descriptor */
  const char *
  getName() {
    return mCtx->name;
  }

  /** The number of components each pixel has, (1-4) */
  uint8_t
  getNumComponents() {
    return mCtx->nb_components;
  }

  /**
   * Amount to shift the luma width right to find the chroma width.
   * For YV12 this is 1 for example.
   * chroma_width = -((-luma_width) >> log2_chroma_w)
   * The note above is needed to ensure rounding up.
   * This value only refers to the chroma components.
   */
  uint8_t
  getLog2ChromaWidth() {
    return mCtx->log2_chroma_w;
  }

  /**
   * Amount to shift the luma height right to find the chroma height.
   * For YV12 this is 1 for example.
   * chroma_height= -((-luma_height) >> log2_chroma_h)
   * The note above is needed to ensure rounding up.
   * This value only refers to the chroma components.
   */
  uint8_t
  getLog2ChromaHeight() {
    return mCtx->log2_chroma_h;
  }

  /**
   * Get the flags for this Pixel Format. This is a bitmask of the {@link PixelFormatDescriptor.Flag} enum values.
   */
  uint8_t
  getFlags() {
    return mCtx->flags;
  }

  /**
   * Is the given flag set on this format?
   */
  bool
  getFlag(PixelFormatDescriptor::Flag flag) {
    return mCtx->flags & flag;
  }

  /**
   * Return the number of bits per pixel used by the pixel format
   * described by pixdesc. Note that this is not the same as the number
   * of bits per sample.
   *
   * The returned number of bits refers to the number of bits actually
   * used for storing the pixel information, that is padding bits are
   * not counted.
   */
  int32_t
  getBitsPerPixel() {
    return av_get_bits_per_pixel(mCtx);
  }

  /**
   * Return the number of bits per pixel for the pixel format
   * described by pixdesc, including any padding or unused bits.
   */
  int32_t
  getPaddedBitsPerPixel() {
    return av_get_padded_bits_per_pixel(mCtx);
  }

  /**
   * Parameters that describe how pixels are packed.
   * If the format has 2 or 4 components, then alpha is last.
   * If the format has 1 or 2 components, then luma is 0.
   * If the format has 3 or 4 components,
   * if the RGB flag is set then 0 is red, 1 is green and 2 is blue;
   * otherwise 0 is luma, 1 is chroma-U and 2 is chroma-V.
   */
  PixelComponentDescriptor*
  getComponentDescriptor(int32_t component);

  /**
   * @return an PixelFormat id described by desc, or PixelFormat.Type.PIX_FMT_NONE if desc
   * is not a valid pointer to a pixel format descriptor.
   */
  PixelFormat::Type
  getFormat() {
    return (PixelFormat::Type) av_pix_fmt_desc_get_id(mCtx);
  }
#ifndef SWIG
  static PixelFormatDescriptor*
  make(const AVPixFmtDescriptor* ctx);
#endif // ! SWIG
private:
  PixelFormatDescriptor() :
      mCtx(0) {
  }
  virtual
  ~PixelFormatDescriptor() {
  }
  const AVPixFmtDescriptor* mCtx;

};

}
}
}

#endif /*IPIXELFORMAT_H_*/
