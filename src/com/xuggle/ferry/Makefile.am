VS_PRODUCT=xuggle_ferry
VS_CPP_PATH=com/xuggle/ferry
VS_CPP_PACKAGE=com.xuggle.ferry
VS_CPP_NAMESPACE=com::xuggle::ferry
include @top_builddir@/mk/Makefile.global
SUFFIXES=.i

# This is a convenience library
noinst_LTLIBRARIES = libxuggle-ferry.la
noinst_PROGRAMS=xuggle-ferry-main

xuggle_ferry_main_SOURCES= \
  Main.cpp
xuggle_ferry_main_LDADD= libxuggle-ferry.la

libxuggle_ferry_la_SOURCES= \
  AtomicInteger.cpp \
  Buffer.cpp \
  IBuffer.cpp \
  JNIHelper.cpp \
  JNIMemoryManager.cpp \
  Logger.cpp \
  LoggerStack.cpp \
  Mutex.cpp \
  RefCounted.cpp \
  RefCountedTester.cpp

nodist_libxuggle_ferry_la_SOURCES= \
  Ferry.cpp

libxuggle_ferry_ladir=$(includedir)/$(VS_CPP_PATH)
libxuggle_ferry_la_HEADERS= \
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
  IBuffer.h \
  Buffer.h \
  JNIHelper.swg \
  IBuffer.swg \
  RefCounted.swg \
  RefPointer.h

BUILT_SOURCES = \
  Ferry.cpp
  
Ferry.cpp : $(libxuggle_ferry_la_HEADERS)

clean-local:
	if test -w "$(SWIGOUTTMPDIR)"; then rm -rf $(SWIGOUTTMPDIR); fi
	rm -f $(BUILT_SOURCES)

maintainer-clean-local:
	rm -rf $(SWIGOUTDIR)/$(VS_CPP_PATH)

