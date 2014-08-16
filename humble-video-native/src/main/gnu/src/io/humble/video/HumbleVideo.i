/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
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

%module Video
%{

// HumbleVideo.i: Start generated code
// >>>>>>>>>>>>>>>>>>>>>>>>>>>
#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/video/KeyValueBag.h>
#include <io/humble/video/Property.h>
#include <io/humble/video/Rational.h>
#include <io/humble/video/PixelFormat.h>
#include <io/humble/video/Media.h>
#include <io/humble/video/MediaRaw.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/MediaSubtitle.h>
#include <io/humble/video/MediaPacket.h>
#include <io/humble/video/Configurable.h>
#include <io/humble/video/MediaResampler.h>
#include <io/humble/video/MediaPictureResampler.h>
#include <io/humble/video/MediaAudioResampler.h>
#include <io/humble/video/Global.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/ContainerFormat.h>
#include <io/humble/video/MuxerFormat.h>
#include <io/humble/video/DemuxerFormat.h>
#include <io/humble/video/IndexEntry.h>
#include <io/humble/video/ContainerStream.h>
#include <io/humble/video/Coder.h>
#include <io/humble/video/Decoder.h>
#include <io/humble/video/Encoder.h>
#include <io/humble/video/Container.h>
#include <io/humble/video/DemuxerStream.h>
#include <io/humble/video/Demuxer.h>
#include <io/humble/video/MuxerStream.h>
#include <io/humble/video/Muxer.h>
#include <io/humble/video/FilterType.h>
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/Filter.h>
#include <io/humble/video/FilterLink.h>
#include <io/humble/video/FilterEndPoint.h>
#include <io/humble/video/FilterSource.h>
#include <io/humble/video/FilterAudioSource.h>
#include <io/humble/video/FilterPictureSource.h>
#include <io/humble/video/FilterSink.h>
#include <io/humble/video/FilterAudioSink.h>
#include <io/humble/video/FilterPictureSink.h>

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
import io.humble.ferry.Buffer;
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

%include "enums.swg"

%import <io/humble/ferry/Ferry.i>
/** Now we override the exception handler to have module specific exceptions */

%define HUMBLE_HANDLE_EXCEPTION( NOTUSED )
  // HumbleVideo.i: Start generated code
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>
  try
  {
    $action
  }
  catch(std::exception & e)
  {
    io::humble::video::Global::catchException(e);
    return $null;
  }
  catch(...)
  {
    std::runtime_error e("Unhandled and unknown native exception");
    io::humble::ferry::JNIHelper::throwJavaException(jenv, "java/lang/RuntimeException", e);
    return $null;
  }
  
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<
  // HumbleVideo.i: End generated code

%enddef

%define HUMBLE_JAVA_EXCEPTION( JTYPE, MATCH )
%javaexception(JTYPE) MATCH {
  HUMBLE_HANDLE_EXCEPTION(not_used)
}
%enddef

%exception
{
  HUMBLE_HANDLE_EXCEPTION($action)
}

%include <io/humble/video/HumbleVideo.h>
%include <io/humble/video/PixelFormat.h>
%include <io/humble/video/Rational.swg>
%include <io/humble/video/Global.swg>
%include <io/humble/video/KeyValueBag.swg>
%include <io/humble/video/Property.swg>
%include <io/humble/video/Media.swg>
%include <io/humble/video/MediaRaw.swg>
%include <io/humble/video/MediaAudio.swg>
%include <io/humble/video/MediaPicture.swg>
%include <io/humble/video/MediaSubtitle.swg>
%include <io/humble/video/MediaPacket.swg>
%include <io/humble/video/Configurable.swg>
%include <io/humble/video/MediaResampler.h>
%include <io/humble/video/MediaPictureResampler.swg>
%include <io/humble/video/MediaAudioResampler.h>
%include <io/humble/video/Codec.swg>
%include <io/humble/video/Coder.swg>
%include <io/humble/video/ContainerFormat.swg>
%include <io/humble/video/MuxerFormat.swg>
%include <io/humble/video/DemuxerFormat.swg>
%include <io/humble/video/IndexEntry.swg>
%include <io/humble/video/Decoder.swg>
%include <io/humble/video/Encoder.swg>
%include <io/humble/video/ContainerStream.swg>
%include <io/humble/video/Container.swg>
%include <io/humble/video/MuxerStream.swg>
%include <io/humble/video/DemuxerStream.swg>
%include <io/humble/video/Muxer.swg>
%include <io/humble/video/Demuxer.swg>
%include <io/humble/video/FilterType.swg>
%include <io/humble/video/FilterGraph.swg>
%include <io/humble/video/Filter.swg>
%include <io/humble/video/FilterLink.swg>
%include <io/humble/video/FilterEndPoint.swg>
%include <io/humble/video/FilterSource.swg>
%include <io/humble/video/FilterAudioSource.swg>
%include <io/humble/video/FilterPictureSource.swg>
%include <io/humble/video/FilterSink.swg>
%include <io/humble/video/FilterAudioSink.swg>
%include <io/humble/video/FilterPictureSink.swg>
