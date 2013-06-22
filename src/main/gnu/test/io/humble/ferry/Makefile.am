VS_PRODUCT=humble_ferry
VS_CPP_PATH=io/humble/ferry
VS_CPP_PACKAGE=io.humble.ferry
VS_CPP_NAMESPACE=io::humble::ferry
VS_TEST=1
include @top_builddir@/mk/Makefile.global

check_PROGRAMS=ferryTestLogger ferryTestRefPointer ferryTestMutex  ferryTestBuffer

TESTS=$(check_PROGRAMS)

ferryTestLogger_SOURCES= \
  LoggerTest.cpp \
  Main.cpp

nodist_ferryTestLogger_SOURCES= \
  LoggerTest_CXXRunner.cpp

ferryTestLogger_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la


ferryTestRefPointer_SOURCES= \
  RefPointerTest.cpp \
  Main.cpp

nodist_ferryTestRefPointer_SOURCES= \
  RefPointerTest_CXXRunner.cpp

ferryTestRefPointer_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

ferryTestMutex_SOURCES= \
  MutexTest.cpp \
  Main.cpp 

nodist_ferryTestMutex_SOURCES= \
  MutexTest_CXXRunner.cpp

ferryTestMutex_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

ferryTestBuffer_SOURCES=\
  BufferTest.cpp \
  Main.cpp

nodist_ferryTestBuffer_SOURCES=\
  BufferTest_CXXRunner.cpp

ferryTestBuffer_LDADD=\
  $(top_builddir)/src/io/humble/libhumblevideo.la


BUILT_SOURCES= \
  LoggerTest_CXXRunner.cpp \
  BufferTest_CXXRunner.cpp \
  RefPointerTest_CXXRunner.cpp \
  MutexTest_CXXRunner.cpp 

noinst_HEADERS= \
  LoggerTest.h \
  BufferTest.h \
  MutexTest.h \
  RefPointerTest.h

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp3
	rm -rf memcheck*.log
