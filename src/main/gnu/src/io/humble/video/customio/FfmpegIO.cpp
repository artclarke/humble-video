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
 * Container.h
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */

#include <stdexcept>

// For EINTR
#include <errno.h>

#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/video/customio/FfmpegIO.h>
#include <io/humble/video/customio/JavaURLProtocolManager.h>

using namespace io::humble::ferry;
using namespace io::humble::video::customio;

// we need to have a non-exception raising  interrupt check here
#undef VS_CHECK_INTERRUPT
#define VS_CHECK_INTERRUPT(retval, __COND__) do { \
    if (__COND__) { \
      JNIHelper* helper = JNIHelper::getHelper(); \
      if (helper && helper->isInterrupted()) \
        (retval) = EINTR > 0 ? -EINTR : EINTR; \
        } \
} while(0)

VS_API_HUMBLE_VIDEO_CUSTOMIO void VS_API_CALL Java_io_humble_video_customio_FfmpegIO_init(JNIEnv *env, jclass)
{
  JavaVM* vm=0;
  if (!io::humble::ferry::JNIHelper::sGetVM()) {
    env->GetJavaVM(&vm);
    io::humble::ferry::JNIHelper::sSetVM(vm);
  }
}

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1registerProtocolHandler(
    JNIEnv *jenv, jclass, jstring aProtoName, jobject aProtoMgr)
{
  int retval = -1;
  const char *protoName= NULL;
  try {
    protoName = jenv->GetStringUTFChars(aProtoName, NULL);
    if (protoName != NULL)
    {
      // Yes, we deliberately leak the URLProtocolManager...
      JavaURLProtocolManager::registerProtocol(protoName, aProtoMgr);
      retval = 0;
    }
  }
  catch(std::exception & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      if (cls)
        jenv->ThrowNew(cls, e.what());
    }
    retval = -1;
  }
  catch(...)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      if (cls)
        jenv->ThrowNew(cls, "Unhandled and unknown native exception");
    }
    retval = -1;
  }
  if (protoName != NULL) {
    jenv->ReleaseStringUTFChars(aProtoName, protoName);
    protoName = NULL;
  }
  return retval;
}

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1open(
    JNIEnv * jenv, jclass, jobject handle, jstring url, jint flags)
{
  URLProtocolHandler*handleVal= NULL;
  const char *nativeURL= NULL;
  jint retval = -1;

  nativeURL = jenv->GetStringUTFChars(url, NULL);
  if (nativeURL != NULL)
  {
    try
    {
      handleVal = URLProtocolManager::findHandler(nativeURL, flags, 0);
      if (!handleVal)
        throw std::runtime_error("could not find protocol manager for url");
      retval = handleVal->url_open(nativeURL, flags);
      VS_CHECK_INTERRUPT(retval, 1);
    }
    catch(std::exception & e)
    {
      // we don't let a native exception override a java exception
      if (!jenv->ExceptionCheck())
      {
        jclass cls=jenv->FindClass("java/lang/RuntimeException");
        jenv->ThrowNew(cls, e.what());
      }
      retval = -1;
    }
    catch(...)
    {
      // we don't let a native exception override a java exception
      if (!jenv->ExceptionCheck())
      {
        jclass cls=jenv->FindClass("java/lang/RuntimeException");
        jenv->ThrowNew(cls, "Unhandled and unknown native exception");
      }
      retval = -1;
    }

    // regardless of what the function returns, set the pointer
    JNIHelper::sSetPointer(handle, handleVal);
  }

  // free the string
  if (nativeURL != NULL)
  {
    jenv->ReleaseStringUTFChars(url, nativeURL);
    nativeURL = NULL;
  }
  return retval;
}

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1read(
    JNIEnv *jenv, jclass, jobject handle, jbyteArray javaBuf, jint buflen)
{
  URLProtocolHandler* handleVal= NULL;

  jint retval = -1;

  handleVal = (URLProtocolHandler*)JNIHelper::sGetPointer(handle);

  jbyte *byteArray = jenv->GetByteArrayElements(javaBuf, NULL);
  try
  {
    if (handleVal)
      retval = handleVal->url_read((uint8_t*)byteArray, buflen);
    VS_CHECK_INTERRUPT(retval, retval < 0 || retval < buflen);
  }
  catch(std::exception & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, e.what());
    }
    retval = -1;
  }
  catch(...)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, "Unhandled and unknown native exception");
    }
    retval = -1;
  }
  jenv->ReleaseByteArrayElements(javaBuf, byteArray, 0);
  return retval;
}

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1write(
    JNIEnv *jenv, jclass, jobject handle, jbyteArray javaBuf, jint buflen)
{
  URLProtocolHandler* handleVal= NULL;

  jint retval = -1;

  handleVal = (URLProtocolHandler*)JNIHelper::sGetPointer(handle);

  jbyte *byteArray = jenv->GetByteArrayElements(javaBuf, NULL);
  try
  {
    if (handleVal)
      retval = handleVal->url_write((uint8_t*)byteArray, buflen);
    VS_CHECK_INTERRUPT(retval, retval < 0 || retval != buflen);
  }
  catch(std::exception & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, e.what());
    }
    retval = -1;
  }
  catch(...)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, "Unhandled and unknown native exception");
    }
    retval = -1;
  }
  jenv->ReleaseByteArrayElements(javaBuf, byteArray, 0);
  return retval;
}

VS_API_HUMBLE_VIDEO_CUSTOMIO jlong VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1seek(
    JNIEnv *jenv, jclass, jobject handle, jlong position, jint whence)
{
  URLProtocolHandler* handleVal= NULL;

  jint retval = -1;

  handleVal = (URLProtocolHandler*)JNIHelper::sGetPointer(handle);
  try
  {
    if (handleVal)
      retval = handleVal->url_seek((int64_t)position, whence);
    VS_CHECK_INTERRUPT(retval, 1);
  }
  catch(std::exception & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, e.what());
    }
    retval = -1;
  }
  catch(...)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, "Unhandled and unknown native exception");
    }
    retval = -1;
  }
  return retval;
}

VS_API_HUMBLE_VIDEO_CUSTOMIO jint VS_API_CALL Java_io_humble_video_customio_FfmpegIO_native_1url_1close(
    JNIEnv *jenv, jclass, jobject handle)
{
  URLProtocolHandler* handleVal= NULL;

  jint retval = -1;

  handleVal = (URLProtocolHandler*)JNIHelper::sGetPointer(handle);
  try
  {
    if (handleVal)
      retval = handleVal->url_close();
    VS_CHECK_INTERRUPT(retval, 1);
  }
  catch(std::exception & e)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, e.what());
    }
    retval = -1;
  }
  catch(...)
  {
    // we don't let a native exception override a java exception
    if (!jenv->ExceptionCheck())
    {
      jclass cls=jenv->FindClass("java/lang/RuntimeException");
      jenv->ThrowNew(cls, "Unhandled and unknown native exception");
    }
    retval = -1;
  }
  return retval;
}
