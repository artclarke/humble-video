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

#include <jni.h>
#include <io/humble/video/customio/CustomIO.h>

/* Header for class io_humble_video_io_FfmpegIO */

#ifndef _FFMPEG_IO_H
#define _FFMPEG_IO_H
#ifdef __cplusplus
extern "C" {
#endif

VS_API_HUMBLE_VIDEO_CUSTOMIO void VS_API_CALL Java_io_humble_video_customio_FfmpegIO_init(JNIEnv *env, jclass);

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1registerProtocolHandler
  (JNIEnv *, jclass, jstring, jobject);

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1open
  (JNIEnv *, jclass, jobject, jstring, jint);

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1read
  (JNIEnv *, jclass, jobject, jbyteArray, jint);

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1write
  (JNIEnv *, jclass, jobject, jbyteArray, jint);

VS_API_HUMBLE_VIDEO_CUSTOMIO jlong VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1seek
  (JNIEnv *, jclass, jobject, jlong, jint);

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1close
  (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif
#endif // _FFMPEG_IO_H
