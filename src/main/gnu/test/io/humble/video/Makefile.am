VS_PRODUCT=humblevideo
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
VS_TEST=1
include @top_builddir@/mk/Makefile.global
SUBDIRS=io

check_PROGRAMS=\
  InputFormatTest \
  OutputFormatTest \
  PropertyTest \
  RationalTest 

BUILT_SOURCES= \
  InputFormatTest_CXXRunner.cpp \
  OutputFormatTest_CXXRunner.cpp \
  PropertyTest_CXXRunner.cpp \
  RationalTest_CXXRunner.cpp

noinst_HEADERS = \
  InputFormatTest.h \
  OutputFormatTest.h \
  PropertyTest.h \
  RationalTest.h


inst_check=$(check_PROGRAMS)
inst_checkdir=$(bindir)

# Make sure we set the abs_top_srcdir environment variable so the 
# tests can auto-find their fixtures
TESTS=$(check_PROGRAMS)

InputFormatTest_SOURCES= \
  InputFormatTest.cpp \
  Main.cpp

nodist_InputFormatTest_SOURCES= \
  InputFormatTest_CXXRunner.cpp

InputFormatTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 
  
OutputFormatTest_SOURCES= \
  OutputFormatTest.cpp \
  Main.cpp

nodist_OutputFormatTest_SOURCES= \
  OutputFormatTest_CXXRunner.cpp

OutputFormatTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 


PropertyTest_SOURCES= \
  PropertyTest.cpp \
  Main.cpp

nodist_PropertyTest_SOURCES= \
  PropertyTest_CXXRunner.cpp

PropertyTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

RationalTest_SOURCES=\
  RationalTest.cpp \
  Main.cpp

nodist_RationalTest_SOURCES=\
  RationalTest_CXXRunner.cpp 

RationalTest_LDADD=\
  $(top_builddir)/src/io/humble/libhumblevideo.la 

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp4
	rm -rf *.mp3
	rm -rf *.ogg
