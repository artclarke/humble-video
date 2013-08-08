VS_PRODUCT=humble_video_customio
VS_CPP_PATH=io/humble/video/customio
VS_CPP_PACKAGE=io.humble.video.customio
VS_CPP_NAMESPACE=io::humble::video:customio
include @top_builddir@/mk/Makefile.global

# This is a convenience library
noinst_LTLIBRARIES = libhumble-video-customio.la

libhumble_video_customio_la_LIBADD=

libhumble_video_customio_la_SOURCES= \
  FfmpegIO.cpp \
  StdioURLProtocolHandler.cpp \
  StdioURLProtocolManager.cpp \
  JavaURLProtocolHandler.cpp \
  JavaURLProtocolManager.cpp \
  URLProtocolHandler.cpp \
  URLProtocolManager.cpp

libhumble_video_customio_ladir=$(includedir)/$(VS_CPP_PATH)
libhumble_video_customio_la_HEADERS= \
  FfmpegIO.h \
  CustomIO.h \
  StdioURLProtocolHandler.h \
  StdioURLProtocolManager.h \
  JavaURLProtocolHandler.h \
  JavaURLProtocolManager.h \
  URLProtocolHandler.h \
  URLProtocolManager.h 
