VS_PRODUCT=humblevideo
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
VS_TEST=1
include @top_builddir@/mk/Makefile.global
SUBDIRS=customio

check_PROGRAMS=\
  KeyValueBagTester \
  SourceTester \
  InputFormatTester \
  OutputFormatTester \
  PropertyTester \
  RationalTester 

BUILT_SOURCES= \
  KeyValueBagTest_CXXRunner.cpp \
  SourceTest_CXXRunner.cpp \
  InputFormatTest_CXXRunner.cpp \
  OutputFormatTest_CXXRunner.cpp \
  PropertyTest_CXXRunner.cpp \
  RationalTest_CXXRunner.cpp

noinst_HEADERS = \
  KeyValueBagTest.h \
  SourceTest.h \
  InputFormatTest.h \
  OutputFormatTest.h \
  PropertyTest.h \
  RationalTest.h


inst_check=$(check_PROGRAMS)
inst_checkdir=$(bindir)

# Make sure we set the abs_top_srcdir environment variable so the 
# tests can auto-find their fixtures
TESTS=$(check_PROGRAMS)

KeyValueBagTester_SOURCES= \
  KeyValueBagTest.cpp \
  Main.cpp

nodist_KeyValueBagTester_SOURCES= \
  KeyValueBagTest_CXXRunner.cpp

KeyValueBagTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

SourceTester_SOURCES= \
  SourceTest.cpp \
  Main.cpp

nodist_SourceTester_SOURCES= \
  SourceTest_CXXRunner.cpp

SourceTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

InputFormatTester_SOURCES= \
  InputFormatTest.cpp \
  Main.cpp

nodist_InputFormatTester_SOURCES= \
  InputFormatTest_CXXRunner.cpp

InputFormatTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 
  
OutputFormatTester_SOURCES= \
  OutputFormatTest.cpp \
  Main.cpp

nodist_OutputFormatTester_SOURCES= \
  OutputFormatTest_CXXRunner.cpp

OutputFormatTester_LDADD= \
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
