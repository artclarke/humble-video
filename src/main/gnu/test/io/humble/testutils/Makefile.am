VS_PRODUCT=humble_testutils
VS_CPP_PATH=io/humble/testutils
VS_CPP_PACKAGE=io.humble.testutils
VS_CPP_NAMESPACE=io::humble::testutils
VS_TEST=1
include @top_builddir@/mk/Makefile.global

check_PROGRAMS=TrueTest
TESTS=$(check_PROGRAMS)

TrueTest_SOURCES= \
  TrueTest.cpp \
  Main.cpp

nodist_TrueTest_SOURCES= \
  TrueTest_CXXRunner.cpp 

TrueTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

BUILT_SOURCES= \
  TrueTest_CXXRunner.cpp

noinst_HEADERS = \
  TestUtils.h \
  TrueTest.h

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp3
	rm -rf *.log
	rm -rf *.xml
