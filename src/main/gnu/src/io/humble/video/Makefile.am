VS_PRODUCT=humble-video
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
include @top_builddir@/mk/Makefile.global

SUBDIRS=customio
SUFFIXES=.i

noinst_LTLIBRARIES = libhumble-video.la

libhumble_video_la_LIBADD= $(VS_PKG_LIBRARIES)

libhumble_video_la_SOURCES= \
  PixelFormat.cpp \
  KeyValueBag.cpp \
  KeyValueBagImpl.cpp \
  Property.cpp \
  PropertyImpl.cpp \
  Rational.cpp \
  RationalImpl.cpp \
  ContainerFormat.cpp \
  Configurable.cpp \
  Container.cpp \
  Source.cpp \
  SourceImpl.cpp \
  OutputFormat.cpp \
  InputFormat.cpp \
  Global.cpp
  
nodist_libhumble_video_la_SOURCES= \
  HumbleVideo.cpp

libhumble_video_ladir=$(includedir)/$(VS_CPP_PATH)
libhumble_video_la_HEADERS= \
  HumbleVideo.h \
  Codec.h \
  FfmpegIncludes.h \
  Global.swg \
  PixelFormat.h \
  KeyValueBag.h \
  KeyValueBagImpl.h \
  KeyValueBag.swg \
  Rational.h \
  RationalImpl.h \
  Rational.swg \
  Property.h \
  PropertyImpl.h \
  Property.swg \
  ContainerFormat.h \
  ContainerFormat.swg \
  OutputFormat.h \
  OutputFormat.swg \
  InputFormat.h \
  InputFormat.swg \
  Container.h \
  Container.swg \
  Configurable.h \
  Configurable.swg \
  Source.h \
  Source.swg \
  SourceImpl.h \
  Global.h

BUILT_SOURCES= \
  HumbleVideo.cpp

HumbleVideo.cpp : $(libhumble_video_la_HEADERS)

HumbleVideo.i : $(libhumble_video_la_HEADERS)

clean-local:
	rm -f $(BUILT_SOURCES)
	if test -w "$(SWIGOUTTMPDIR)"; then rm -rf $(SWIGOUTTMPDIR); fi
