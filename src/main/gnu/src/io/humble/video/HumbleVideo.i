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

%module Video
%{

// HumbleVideo.i: Start generated code
// >>>>>>>>>>>>>>>>>>>>>>>>>>>
#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/video/IProperty.h>
#include <io/humble/video/IPixelFormat.h>
#include <io/humble/video/Global.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/IRational.h>
#include <io/humble/video/ContainerFormat.h>
#include <io/humble/video/OutputFormat.h>
#include <io/humble/video/InputFormat.h>

/*
#include <io/humble/video/ITimeValue.h>
#include <io/humble/video/IMetaData.h>
#include <io/humble/video/IMediaData.h>
#include <io/humble/video/IAudioSamples.h>
#include <io/humble/video/ICodec.swg>
#include <io/humble/video/IPacket.h>
#include <io/humble/video/IIndexEntry.h>
#include <io/humble/video/IAudioResampler.h>
#include <io/humble/video/IVideoPicture.h>
#include <io/humble/video/IVideoResampler.h>
#include <io/humble/video/IStreamCoder.h>
#include <io/humble/video/IStream.h>
#include <io/humble/video/IContainerFormat.h>
#include <io/humble/video/IContainer.h>
#include <io/humble/video/IMediaDataWrapper.h>
#include <io/humble/video/IError.h>
*/

using namespace VS_CPP_NAMESPACE;

extern "C" {

SWIGEXPORT void JNICALL
Java_io_humble_video_Video_init(JNIEnv *env, jclass)
{
  JavaVM* vm=0;
  if (!io::humble::ferry::JNIHelper::sGetVM()) {
    env->GetJavaVM(&vm);
    io::humble::ferry::JNIHelper::sSetVM(vm);
  }
}

}


// <<<<<<<<<<<<<<<<<<<<<<<<<<<
// HumbleVideo.i: End generated code

%}

%pragma(java) jniclassimports=%{
%}

%pragma(java) moduleimports=%{
/**
 * Internal Only.
 * 
 */
%}

%pragma(java) modulecode=%{
  static {
    // Force the JNI library to load
    VideoJNI.noop();
  }

  /**
   * Method to force loading of all native methods in the library.
   */
  public static void load() {}

  /**
   * Internal Only.  Do not use.
   */
  public native static void init();

%}

%pragma(java) jniclasscode=%{
// HumbleVideo.i: Start generated code
// >>>>>>>>>>>>>>>>>>>>>>>>>>>

  static {
    io.humble.ferry.Ferry.init();
    io.humble.video.Video.init();
    io.humble.video.Global.init();
  }
  public static void noop() {
    // Here only to force JNI library to load
  }
  
// <<<<<<<<<<<<<<<<<<<<<<<<<<<
// HumbleVideo.i: End generated code
  
%}
// As per 1.17, we now make sure we generate proper Java enums on
// classes
%include "enums.swg"

%import <io/humble/ferry/Ferry.i>

%include <io/humble/video/HumbleVideo.h>
%include <io/humble/video/Global.swg>
%include <io/humble/video/Codec.swg>
%include <io/humble/video/IProperty.swg>
%include <io/humble/video/IPixelFormat.h>
%include <io/humble/video/IRational.swg>
%include <io/humble/video/ContainerFormat.swg>
%include <io/humble/video/OutputFormat.swg>
%include <io/humble/video/InputFormat.swg>

/*
%include <io/humble/video/ITimeValue.h>
%include <io/humble/video/IMetaData.swg>
%include <io/humble/video/IMediaData.swg>
%include <io/humble/video/IPacket.swg>
%include <io/humble/video/IAudioSamples.swg>
%include <io/humble/video/IVideoPicture.swg>
%include <io/humble/video/ICodec.swg>
%include <io/humble/video/IAudioResampler.h>
%include <io/humble/video/IVideoResampler.swg>
%include <io/humble/video/IStreamCoder.swg>
%include <io/humble/video/IIndexEntry.swg>
%include <io/humble/video/IStream.swg>
%include <io/humble/video/IContainerFormat.swg>
%include <io/humble/video/IContainer.swg>
%include <io/humble/video/IMediaDataWrapper.swg>
%include <io/humble/video/IError.swg>
*/
