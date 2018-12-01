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
  Mingw64Fixes.c \
  VideoExceptions.cpp \
  BitStreamFilter.cpp \
  AVBufferSupport.cpp \
  PixelFormat.cpp \
  KeyValueBag.cpp \
  KeyValueBagImpl.cpp \
  Property.cpp \
  PropertyImpl.cpp \
  Rational.cpp \
  RationalImpl.cpp \
  Codec.cpp \
  Media.cpp \
  MediaRaw.cpp \
  MediaResampler.cpp \
  MediaAudio.cpp \
  MediaAudioResampler.cpp \
  MediaPicture.cpp \
  MediaPictureImpl.cpp \
  MediaPictureResampler.cpp \
  MediaPictureResamplerImpl.cpp \
  MediaSubtitle.cpp \
  MediaSubtitleImpl.cpp \
  MediaParameters.cpp \
  IndexEntry.cpp \
  IndexEntryImpl.cpp \
  MediaPacket.cpp \
  MediaPacketImpl.cpp \
  ContainerFormat.cpp \
  Configurable.cpp \
  Coder.cpp \
  Decoder.cpp \
  Encoder.cpp \
  ContainerStream.cpp \
  Container.cpp \
  DemuxerFormat.cpp \
  Muxer.cpp \
  MuxerStream.cpp \
  Demuxer.cpp \
  DemuxerImpl.cpp \
  DemuxerStream.cpp \
  MuxerFormat.cpp \
  FilterType.cpp \
  FilterGraph.cpp \
  Filter.cpp \
  FilterLink.cpp \
  FilterEndPoint.cpp \
  FilterSource.cpp \
  FilterAudioSource.cpp \
  FilterPictureSource.cpp \
  FilterSink.cpp \
  FilterAudioSink.cpp \
  FilterPictureSink.cpp \
  Global.cpp
  
nodist_libhumble_video_la_SOURCES= \
  HumbleVideo.cpp

libhumble_video_ladir=$(includedir)/$(VS_CPP_PATH)
libhumble_video_la_HEADERS= \
  HumbleVideo.h \
  VideoExceptions.h \
  BitStreamFilter.h \
  AVBufferSupport.h \
  Codec.h \
  Codec.swg \
  FfmpegIncludes.h \
  Global.swg \
  PixelFormat.h \
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
  MediaAudio.swg \
  MediaResampler.h \
  MediaAudioResampler.h \
  MediaPicture.h \
  MediaPictureImpl.h \
  MediaPicture.swg \
  MediaPictureResampler.h \
  MediaPictureResamplerImpl.h \
  MediaPictureResampler.swg \
  MediaSubtitle.h \
  MediaSubtitleImpl.h \
  MediaSubtitle.swg \
  MediaParameters.h \
  MediaParameters.swg \
  Processor.h \
  Processor.swg \
  IndexEntry.h \
  IndexEntryImpl.h \
  IndexEntry.swg \
  MediaPacket.h \
  MediaPacketImpl.h \
  MediaPacket.swg \
  Property.h \
  PropertyImpl.h \
  Property.swg \
  ContainerFormat.h \
  ContainerFormat.swg \
  MuxerFormat.h \
  MuxerFormat.swg \
  DemuxerFormat.h \
  DemuxerFormat.swg \
  Configurable.h \
  Configurable.swg \
  Coder.h \
  Coder.swg \
  Decoder.h \
  Decoder.swg \
  Encoder.h \
  Encoder.swg \
  ContainerStream.h \
  ContainerStream.swg \
  Container.h \
  Container.swg \
  DemuxerStream.h \
  DemuxerStream.swg \
  Muxer.h \
  Muxer.swg \
  MuxerStream.h \
  MuxerStream.swg \
  Demuxer.h \
  Demuxer.swg \
  DemuxerImpl.h \
  FilterType.h \
  FilterGraph.h \
  Filter.h \
  FilterLink.h \
  FilterEndPoint.h \
  FilterSource.h \
  FilterAudioSource.h \
  FilterPictureSource.h \
  FilterSink.h \
  FilterAudioSink.h \
  FilterPictureSink.h \
  Global.h

BUILT_SOURCES= \
  HumbleVideo.cpp

HumbleVideo.cpp : $(libhumble_video_la_HEADERS)

HumbleVideo.i : $(libhumble_video_la_HEADERS)

clean-local:
	rm -f $(BUILT_SOURCES)
	if test -w "$(SWIGOUTTMPDIR)"; then rm -rf $(SWIGOUTTMPDIR); fi
