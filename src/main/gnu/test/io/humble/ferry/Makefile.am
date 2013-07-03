VS_PRODUCT=humble_ferry
VS_CPP_PATH=io/humble/ferry
VS_CPP_PACKAGE=io.humble.ferry
VS_CPP_NAMESPACE=io::humble::ferry
VS_TEST=1
include @top_builddir@/mk/Makefile.global

check_PROGRAMS=LoggerTest RefPointerTest MutexTest BufferTest

TESTS=$(check_PROGRAMS)

LoggerTest_SOURCES= \
  LoggerTest.cpp \
  Main.cpp

nodist_LoggerTest_SOURCES= \
  LoggerTest_CXXRunner.cpp

LoggerTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la


RefPointerTest_SOURCES= \
  RefPointerTest.cpp \
  Main.cpp

nodist_RefPointerTest_SOURCES= \
  RefPointerTest_CXXRunner.cpp

RefPointerTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

MutexTest_SOURCES= \
  MutexTest.cpp \
  Main.cpp 

nodist_MutexTest_SOURCES= \
  MutexTest_CXXRunner.cpp

MutexTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

BufferTest_SOURCES=\
  BufferTest.cpp \
  Main.cpp

nodist_BufferTest_SOURCES=\
  BufferTest_CXXRunner.cpp

BufferTest_LDADD=\
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
	rm -rf *.log
	rm -rf *.xml
