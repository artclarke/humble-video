VS_PRODUCT=humblevideo
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
VS_TEST=1
include @top_builddir@/mk/Makefile.global
SUBDIRS=customio

check_PROGRAMS=\
  IndexEntryTester \
  ErrorTester \
  PacketTester \
  KeyValueBagTester \
  SourceTester \
  SourceFormatTester \
  SourceStreamTester \
  SinkFormatTester \
  PropertyTester \
  RationalTester 

BUILT_SOURCES= \
  IndexEntryTest_CXXRunner.cpp \
  ErrorTest_CXXRunner.cpp \
  PacketTest_CXXRunner.cpp \
  KeyValueBagTest_CXXRunner.cpp \
  SourceTest_CXXRunner.cpp \
  SourceFormatTest_CXXRunner.cpp \
  SourceStreamTest_CXXRunner.cpp \
  SinkFormatTest_CXXRunner.cpp \
  PropertyTest_CXXRunner.cpp \
  RationalTest_CXXRunner.cpp

noinst_HEADERS = \
  IndexEntryTest.h \
  ErrorTest.h \
  PacketTest.h \
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

PacketTester_SOURCES= \
  PacketTest.cpp \
  Main.cpp

nodist_PacketTester_SOURCES= \
  PacketTest_CXXRunner.cpp

PacketTester_LDADD= \
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
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp4
	rm -rf *.mp3
	rm -rf *.ogg
	rm -rf *.log
	rm -rf *.xml
