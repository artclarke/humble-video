/*******************************************************************************
 * Copyright (c) 2008, 2010 Xuggle Inc.  All rights reserved.
 *  
 * This file is part of Xuggle-Xuggler-Main.
 *
 * Xuggle-Xuggler-Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xuggle-Xuggler-Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Xuggle-Xuggler-Main.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include "Logger.h"
#include "JNIHelper.h"
#include <cstring>

#include <iostream>

namespace com { namespace xuggle { namespace ferry {
  bool Logger :: mInitialized = false;
  bool Logger :: mGlobalIsLogging[5] = { true, true, true, true, true };
  jclass Logger :: mClass = 0;
  jmethodID Logger :: mGetLoggerMethod = 0;
  jmethodID Logger :: mLogMethod = 0;

  Mutex* Logger :: mClassMutex = 0;

  /*
   * This method formats the message with
   * file and line number if given.
   */
  static
  int formatMsg(char * msgBuf, int bufLen,
      const char *aFilename, int lineNo,
      const char *aFormat, va_list ap)
  {
    int bytesWritten = 0;
    if (msgBuf && bufLen > 0) {
      *msgBuf = 0;
      if (aFormat && *aFormat)
      {
        bytesWritten += vsnprintf(msgBuf, bufLen, aFormat, ap);

        if (aFilename && *aFilename) {
          bufLen -= bytesWritten;
          msgBuf += bytesWritten;

          bytesWritten += snprintf(msgBuf, bufLen, " (%s:%d)",
              aFilename, lineNo);
        }
      }
    }
    return bytesWritten;
  }
  Logger :: Logger(const char* loggerName, jobject javaLogger)
  {
    Logger::init();
    for(unsigned int i =0 ; i< sizeof(mIsLogging)/sizeof(bool); i++)
      mIsLogging[i] = true;

    strncpy(mLoggerName, loggerName, sizeof(mLoggerName));

    mJavaLogger = 0;
    JNIEnv *env=JNIHelper::sGetEnv();
    if (env && javaLogger)
    {
      mJavaLogger = env->NewGlobalRef(javaLogger);
    }
  }

  Logger :: ~Logger()
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    if (env)
    {
      if (mJavaLogger)
        env->DeleteGlobalRef(mJavaLogger);
    }
    mJavaLogger = 0;
  }

  bool
  Logger :: init()
  {
    if (!mInitialized)
    {
      JNIHelper::sRegisterInitializationCallback(initJavaBindings,0);
      mInitialized=true;
    }
    return mInitialized;
  }

  void
  Logger :: initJavaBindings(JavaVM*, void*)
  {
    JNIEnv *env=JNIHelper::sGetEnv();
    if (env)
    {
      // We're inside a JVM, let's get to work.
      jclass cls=env->FindClass("com/xuggle/ferry/NativeLogger");
      if (cls)
      {
        // and find all our methods

        mGetLoggerMethod = env->GetStaticMethodID(cls,
            "getLogger",
            "(Ljava/lang/String;)Lcom/xuggle/ferry/NativeLogger;"
        );
        mLogMethod = env->GetMethodID(cls,
            "log",
            "(ILjava/lang/String;)Z"
        );

        // keep a reference around
        mClass = (jclass)env->NewWeakGlobalRef(cls);

        if (!mClassMutex) {
          mClassMutex = Mutex::make();
        }
      }
    }
  }
  void
  Logger :: shutdownJavaBindings(JavaVM* vm, void*closure)
  {
    Logger* pLogger = (Logger*)closure;
    if (!vm && pLogger)
    {
      // we're shutting down but we were never in the vm; kill this closure.
      delete pLogger;
    }
  }


  Logger *
  Logger :: getLogger(const char * loggerName)
  {

    Logger* retval = 0;
    if (loggerName && *loggerName)
    {
      if (mGetLoggerMethod)
      {
        JNIEnv *env=JNIHelper::sGetEnv();
        if (env)
        {
          jobject javaLogger = 0;
          jstring jLoggerName = env->NewStringUTF(loggerName);
          // Time to get a Java Logger
          javaLogger = env->CallStaticObjectMethod(mClass,
              mGetLoggerMethod, jLoggerName);
          env->DeleteLocalRef(jLoggerName);
          jLoggerName = 0;

          retval = new Logger(loggerName, javaLogger);

          if (javaLogger)
            env->DeleteLocalRef(javaLogger);
          javaLogger = 0;
        }
      } else {
        retval = new Logger(loggerName, 0);
      }

    }
    return retval;
  }

  Logger*
  Logger :: getStaticLogger(const char *aLoggerName)
  {
    Logger* logger = 0;

    logger = Logger::getLogger(aLoggerName);
    if (logger)
    {
        // Register a function to kill it when the JNIHelper says we
        // can
        JNIHelper::sRegisterTerminationCallback(Logger::shutdownJavaBindings,
            logger);
    }
    return logger;
  }

  void
  Logger :: setIsLogging(Level level, bool value)
  {
    mIsLogging[level] = value;
  }

  void
  Logger :: setGlobalIsLogging(Level level, bool value)
  {
    mGlobalIsLogging[level] = value;
  }

  bool
  Logger :: doLog(Level level, const char *msg)
  {
    bool didLog = false;
    if (mGlobalIsLogging[level] && mIsLogging[level])
    {
      if (mClassMutex)
      {
        didLog = this->doJavaLog(level, msg);
      } else {
        didLog = this->doNativeLog(level, msg);
      }
    }

    if (!didLog)
      // turn off logging so we don't keep trying to log
      // here.
      mIsLogging[level] = didLog;

    return didLog;
  }

  bool
  Logger :: doNativeLog(Level level, const char *msg)
  {
    bool didLog = false;
    const char * levelStr = "";
    switch(level)
    {
    case LEVEL_ERROR:
      levelStr="ERROR ";
      didLog = true;
      break;
    case LEVEL_WARN:
      levelStr = "WARN ";
      didLog = true;
      break;
    case LEVEL_INFO:
      levelStr = "INFO ";
      didLog = true;
      break;
    case LEVEL_DEBUG:
      levelStr = "DEBUG ";
      didLog = true;
      break;
    case LEVEL_TRACE:
      levelStr = "TRACE ";
      didLog = false;
      break;
    }
    if (didLog && msg && *msg)
    {
      std::cerr << levelStr << mLoggerName
      << " - " << msg;
      int len = strlen(msg);
      if (msg[len-1] != '\n')
        std::cerr << "\n";
    }
    return didLog;
  }

  bool
  Logger :: isLogging(Level level)
  {
    return mGlobalIsLogging[level] && mIsLogging[level];
  }

  bool
  Logger :: isGlobalLogging(Level level)
  {
    return mGlobalIsLogging[level];
  }

  const char*
  Logger :: getName()
  {
    return mLoggerName;
  }

  bool
  Logger :: doJavaLog(Level level, const char* msg)
  {
    bool retval = false;
    JNIEnv *env=JNIHelper::sGetEnv();
    if (env)
    {
      if (!mJavaLogger && mClassMutex)
      {
        // acquire our mutex log
        mClassMutex->lock();
        // check it again in case someone made
        // a java logger while we waited for the lock.
        if (!mJavaLogger)
        {
          // this logger was initialized BEFORE
          // the class was initialized.  Let's reset it.
          Logger *jlogger = Logger::getLogger(mLoggerName);
          if (jlogger)
          {
            mJavaLogger = jlogger->mJavaLogger;
            jlogger->mJavaLogger = 0;
            delete jlogger;
          }
        }
        // release the mutex
        mClassMutex->unlock();
      }
      if (mJavaLogger)
      {
        jstring javaMsg = env->NewStringUTF(msg);
        retval = env->CallBooleanMethod(mJavaLogger,
            mLogMethod, (int)level, javaMsg);
        env->DeleteLocalRef(javaMsg);
        javaMsg = 0;
      }
    }
    return retval;
  }

  bool
  Logger :: log(const char* filename, int line, Level level, const char *fmt, ...)
  {
    bool didLog = false;
    va_list ap;

    va_start(ap, fmt);
    didLog = this->logVA(filename, line, level, fmt, ap);
    va_end(ap);
    return didLog;
  }

  bool
  Logger :: logVA(const char* filename, int line, Level level, const char *fmt, va_list ap)
  {
    bool didLog = false;
    if (mGlobalIsLogging[level] && mIsLogging[level])
    {
      char msg[cMaxLogMessageLength+1];
      formatMsg(msg, sizeof(msg), filename, line, fmt, ap);
      didLog = this->doLog(level, msg);
    }
    return didLog;
  }

#define VS_LOGGER_CONVENIENCE_METHOD(__FUNCNAME, __LEVEL) \
  bool \
  Logger :: __FUNCNAME(const char* filename, int line, const char * fmt, ...) \
  { \
  Level level = __LEVEL; \
  bool didLog = false; \
  va_list ap; \
  va_start(ap, fmt); \
  if (mGlobalIsLogging[level] && mIsLogging[level]) \
  { \
    char msg[cMaxLogMessageLength+1]; \
    formatMsg(msg, sizeof(msg), filename, line, fmt, ap); \
    didLog = this->doLog(level, msg); \
  } \
  va_end(ap); \
  return didLog; \
  }
  VS_LOGGER_CONVENIENCE_METHOD(error, LEVEL_ERROR)
  VS_LOGGER_CONVENIENCE_METHOD(warn, LEVEL_WARN)
  VS_LOGGER_CONVENIENCE_METHOD(info, LEVEL_INFO)
  VS_LOGGER_CONVENIENCE_METHOD(debug, LEVEL_DEBUG)
  VS_LOGGER_CONVENIENCE_METHOD(trace, LEVEL_TRACE)
}}}
