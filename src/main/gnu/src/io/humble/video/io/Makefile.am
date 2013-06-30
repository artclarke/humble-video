VS_PRODUCT=humble_video_io
VS_CPP_PATH=io/humble/video/io
VS_CPP_PACKAGE=io.humble.video.io
VS_CPP_NAMESPACE=io::humble::video:IO
include @top_builddir@/mk/Makefile.global

# This is a convenience library
noinst_LTLIBRARIES = libhumble-video-io.la

libhumble_video_io_la_LIBADD=

libhumble_video_io_la_SOURCES= \
  FfmpegIO.cpp \
  StdioURLProtocolHandler.cpp \
  StdioURLProtocolManager.cpp \
  JavaURLProtocolHandler.cpp \
  JavaURLProtocolManager.cpp \
  URLProtocolHandler.cpp \
  URLProtocolManager.cpp

libhumble_video_io_ladir=$(includedir)/$(VS_CPP_PATH)
libhumble_video_io_la_HEADERS= \
  FfmpegIO.h \
  IO.h \
  StdioURLProtocolHandler.h \
  StdioURLProtocolManager.h \
  JavaURLProtocolHandler.h \
  JavaURLProtocolManager.h \
  URLProtocolHandler.h \
  URLProtocolManager.h 
