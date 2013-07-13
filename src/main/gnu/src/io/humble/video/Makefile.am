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
  AVBufferSupport.cpp \
  PixelFormat.cpp \
  VideoPicture.cpp \
  VideoPictureImpl.cpp \
  KeyValueBag.cpp \
  KeyValueBagImpl.cpp \
  Property.cpp \
  PropertyImpl.cpp \
  Rational.cpp \
  RationalImpl.cpp \
  Codec.cpp \
  Media.cpp \
  MediaRaw.cpp \
  MediaAudio.cpp \
  MediaAudioImpl.cpp \
  MediaPicture.cpp \
  MediaPictureImpl.cpp \
  IndexEntry.cpp \
  IndexEntryImpl.cpp \
  Error.cpp \
  ErrorImpl.cpp \
  MediaPacket.cpp \
  MediaPacketImpl.cpp \
  ContainerFormat.cpp \
  Configurable.cpp \
  ContainerStream.cpp \
  Container.cpp \
  SourceFormat.cpp \
  Source.cpp \
  SourceImpl.cpp \
  SourceStream.cpp \
  SourceStreamImpl.cpp \
  SinkFormat.cpp \
  Global.cpp
  
nodist_libhumble_video_la_SOURCES= \
  HumbleVideo.cpp

libhumble_video_ladir=$(includedir)/$(VS_CPP_PATH)
libhumble_video_la_HEADERS= \
  HumbleVideo.h \
  AVBufferSupport.h \
  Codec.h \
  Codec.swg \
  FfmpegIncludes.h \
  Global.swg \
  PixelFormat.h \
  VideoPicture.h \
  VideoPicture.swg \
  VideoPictureImpl.h \
  KeyValueBag.h \
  KeyValueBagImpl.h \
  KeyValueBag.swg \
  Rational.h \
  RationalImpl.h \
  Rational.swg \
  Media.h \
  Media.swg \
  MediaRaw.h \
  MediaRaw.swg \
  MediaAudio.h \
  MediaAudioImpl.h \
  MediaAudio.swg \
  MediaPicture.h \
  MediaPictureImpl.h \
  MediaPicture.swg \
  IndexEntry.h \
  IndexEntryImpl.h \
  IndexEntry.swg \
  Error.h \
  ErrorImpl.h \
  Error.swg \
  MediaPacket.h \
  MediaPacketImpl.h \
  MediaPacket.swg \
  Property.h \
  PropertyImpl.h \
  Property.swg \
  ContainerFormat.h \
  ContainerFormat.swg \
  SinkFormat.h \
  SinkFormat.swg \
  SourceFormat.h \
  SourceFormat.swg \
  Configurable.h \
  Configurable.swg \
  ContainerStream.h \
  ContainerStream.swg \
  Container.h \
  Container.swg \
  SourceStream.h \
  SourceStream.swg \
  SourceStreamImpl.h \
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
