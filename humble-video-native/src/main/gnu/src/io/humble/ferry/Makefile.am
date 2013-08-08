VS_PRODUCT=humble_ferry
VS_CPP_PATH=io/humble/ferry
VS_CPP_PACKAGE=io.humble.ferry
VS_CPP_NAMESPACE=io::humble::ferry
include @top_builddir@/mk/Makefile.global
SUFFIXES=.i

# This is a convenience library
noinst_LTLIBRARIES = libhumble-ferry.la
noinst_PROGRAMS=humble-ferry-main

humble_ferry_main_SOURCES= \
  Main.cpp
humble_ferry_main_LDADD= libhumble-ferry.la

libhumble_ferry_la_SOURCES= \
  AtomicInteger.cpp \
  BufferImpl.cpp \
  HumbleException.cpp \
  Buffer.cpp \
  JNIHelper.cpp \
  JNIMemoryManager.cpp \
  Logger.cpp \
  LoggerStack.cpp \
  Mutex.cpp \
  RefCounted.cpp \
  RefCountedTester.cpp

nodist_libhumble_ferry_la_SOURCES= \
  Ferry.cpp

libhumble_ferry_ladir=$(includedir)/$(VS_CPP_PATH)
libhumble_ferry_la_HEADERS= \
  AtomicInteger.h \
  config.h \
  Ferry.h \
  JNIHelper.h \
  JNIMemoryManager.h \
  Logger.h \
  LoggerStack.h \
  Mutex.h \
  RefCounted.h \
  RefCountedTester.h \
  Ferry.i \
  Buffer.h \
  BufferImpl.h \
  HumbleException.h \
  JNIHelper.swg \
  Buffer.swg \
  RefCounted.swg \
  RefPointer.h

BUILT_SOURCES = \
  Ferry.cpp
  
Ferry.cpp : $(libhumble_ferry_la_HEADERS)

clean-local:
	rm -f $(BUILT_SOURCES)
	rm -rf $(SWIGOUTTMPDIR)/$(VS_CPP_PATH)
