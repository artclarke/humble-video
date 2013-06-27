VS_PRODUCT=humble-video
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
include @top_builddir@/mk/Makefile.global

SUBDIRS=
SUFFIXES=.i

noinst_LTLIBRARIES = libhumblevideo.la

libhumblevideo_la_LIBADD= $(VS_PKG_LIBRARIES)

libhumblevideo_la_SOURCES= \
  IPixelFormat.cpp \
  IProperty.cpp \
  IRational.cpp 

nodist_libhumblevideo_la_SOURCES= \
  HumbleVideo.cpp

libhumblevideo_ladir=$(includedir)/$(VS_CPP_PATH)
libhumblevideo_la_HEADERS= \
  HumbleVideo.h \
  FfmpegIncludes.h \
  Global.h \
  Global.swg \
  IPixelFormat.h \
  IProperty.h \
  IRational.h \
  IRational.swg \
  PixelFormat.h \
  Property.h \
  Rational.h 

BUILT_SOURCES= \
  HumbleVideo.cpp

HumbleVideo.cpp : $(libhumblevideo_la_HEADERS)

clean-local:
	rm -f $(BUILT_SOURCES)
	if test -w "$(SWIGOUTTMPDIR)"; then rm -rf $(SWIGOUTTMPDIR); fi
