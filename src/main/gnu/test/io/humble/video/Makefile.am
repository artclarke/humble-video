VS_PRODUCT=humblevideo
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
VS_TEST=1
include @top_builddir@/mk/Makefile.global
SUBDIRS=customio

check_PROGRAMS=\
  FilterTypeTester \
  FilterGraphTester \
  MediaAudioResamplerTester \
  MediaPictureResamplerTester \
  DecoderTester \
  PixelFormatTester \
  CodecTester \
  IndexEntryTester \
  ErrorTester \
  MediaPacketTester \
  MediaAudioTester \
  MediaPictureTester \
  KeyValueBagTester \
  SourceTester \
  SourceFormatTester \
  SourceStreamTester \
  SinkFormatTester \
  PropertyTester \
  RationalTester 

BUILT_SOURCES= \
  FilterTypeTest_CXXRunner.cpp \
  FilterGraphTest_CXXRunner.cpp \
  MediaAudioResamplerTest_CXXRunner.cpp \
  MediaPictureResamplerTest_CXXRunner.cpp \
  DecoderTest_CXXRunner.cpp \
  PixelFormatTest_CXXRunner.cpp \
  CodecTest_CXXRunner.cpp \
  IndexEntryTest_CXXRunner.cpp \
  ErrorTest_CXXRunner.cpp \
  MediaPacketTest_CXXRunner.cpp \
  MediaAudioTest_CXXRunner.cpp \
  MediaPictureTest_CXXRunner.cpp \
  KeyValueBagTest_CXXRunner.cpp \
  SourceTest_CXXRunner.cpp \
  SourceFormatTest_CXXRunner.cpp \
  SourceStreamTest_CXXRunner.cpp \
  SinkFormatTest_CXXRunner.cpp \
  PropertyTest_CXXRunner.cpp \
  RationalTest_CXXRunner.cpp

noinst_HEADERS = \
  FilterTypeTest.h \
  FilterGraphTest.h \
  MediaAudioResamplerTest.h \
  MediaPictureResamplerTest.h \
  DecoderTest.h \
  PixelFormatTest.h \
  CodecTest.h \
  IndexEntryTest.h \
  ErrorTest.h \
  MediaPacketTest.h \
  MediaAudioTest.h \
  MediaPictureTest.h \
  KeyValueBagTest.h \
  SourceTest.h \
  SourceFormatTest.h \
  SourceStreamTest.h \
  SinkFormatTest.h \
  PropertyTest.h \
  RationalTest.h


inst_check=$(check_PROGRAMS)
inst_checkdir=$(bindir)

# Make sure we set the abs_top_srcdir environment variable so the 
# tests can auto-find their fixtures
TESTS=$(check_PROGRAMS)

FilterGraphTester_SOURCES= \
  FilterGraphTest.cpp \
  TestData.cpp \
  lodepng.cpp \
  Main.cpp

nodist_FilterGraphTester_SOURCES= \
  FilterGraphTest_CXXRunner.cpp

FilterGraphTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

FilterTypeTester_SOURCES= \
  FilterTypeTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_FilterTypeTester_SOURCES= \
  FilterTypeTest_CXXRunner.cpp

FilterTypeTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

MediaAudioResamplerTester_SOURCES= \
  MediaAudioResamplerTest.cpp \
  TestData.cpp \
  lodepng.cpp \
  Main.cpp

nodist_MediaAudioResamplerTester_SOURCES= \
  MediaAudioResamplerTest_CXXRunner.cpp

MediaAudioResamplerTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

MediaPictureResamplerTester_SOURCES= \
  MediaPictureResamplerTest.cpp \
  TestData.cpp \
  lodepng.cpp \
  Main.cpp

nodist_MediaPictureResamplerTester_SOURCES= \
  MediaPictureResamplerTest_CXXRunner.cpp

MediaPictureResamplerTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

DecoderTester_SOURCES= \
  DecoderTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_DecoderTester_SOURCES= \
  DecoderTest_CXXRunner.cpp

DecoderTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

PixelFormatTester_SOURCES= \
  PixelFormatTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_PixelFormatTester_SOURCES= \
  PixelFormatTest_CXXRunner.cpp

PixelFormatTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

CodecTester_SOURCES= \
  CodecTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_CodecTester_SOURCES= \
  CodecTest_CXXRunner.cpp

CodecTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

IndexEntryTester_SOURCES= \
  IndexEntryTest.cpp \
  Main.cpp

nodist_IndexEntryTester_SOURCES= \
  IndexEntryTest_CXXRunner.cpp

IndexEntryTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

ErrorTester_SOURCES= \
  ErrorTest.cpp \
  Main.cpp

nodist_ErrorTester_SOURCES= \
  ErrorTest_CXXRunner.cpp

ErrorTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

MediaPacketTester_SOURCES= \
  MediaPacketTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_MediaPacketTester_SOURCES= \
  MediaPacketTest_CXXRunner.cpp

MediaPacketTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

MediaAudioTester_SOURCES= \
  MediaAudioTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_MediaAudioTester_SOURCES= \
  MediaAudioTest_CXXRunner.cpp

MediaAudioTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

MediaPictureTester_SOURCES= \
  MediaPictureTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_MediaPictureTester_SOURCES= \
  MediaPictureTest_CXXRunner.cpp

MediaPictureTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

KeyValueBagTester_SOURCES= \
  KeyValueBagTest.cpp \
  Main.cpp

nodist_KeyValueBagTester_SOURCES= \
  KeyValueBagTest_CXXRunner.cpp

KeyValueBagTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

SourceStreamTester_SOURCES= \
  SourceStreamTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_SourceStreamTester_SOURCES= \
  SourceStreamTest_CXXRunner.cpp

SourceStreamTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 
  
SourceTester_SOURCES= \
  SourceTest.cpp \
  TestData.cpp \
  Main.cpp

nodist_SourceTester_SOURCES= \
  SourceTest_CXXRunner.cpp

SourceTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

SourceFormatTester_SOURCES= \
  SourceFormatTest.cpp \
  Main.cpp

nodist_SourceFormatTester_SOURCES= \
  SourceFormatTest_CXXRunner.cpp

SourceFormatTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 
  
SinkFormatTester_SOURCES= \
  SinkFormatTest.cpp \
  Main.cpp

nodist_SinkFormatTester_SOURCES= \
  SinkFormatTest_CXXRunner.cpp

SinkFormatTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 


PropertyTester_SOURCES= \
  PropertyTest.cpp \
  Main.cpp

nodist_PropertyTester_SOURCES= \
  PropertyTest_CXXRunner.cpp

PropertyTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

RationalTester_SOURCES=\
  RationalTest.cpp \
  Main.cpp

nodist_RationalTester_SOURCES=\
  RationalTest_CXXRunner.cpp 

RationalTester_LDADD=\
  $(top_builddir)/src/io/humble/libhumblevideo.la 

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.au
	rm -rf *.pgm
	rm -rf *.png
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp4
	rm -rf *.mp3
	rm -rf *.ogg
	rm -rf *.log
	rm -rf *.xml
