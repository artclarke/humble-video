VS_PRODUCT=humble_ferry
VS_CPP_PATH=io/humble/ferry
VS_CPP_PACKAGE=io.humble.ferry
VS_CPP_NAMESPACE=io::humble::ferry
VS_TEST=1
include @top_builddir@/mk/Makefile.global

check_PROGRAMS=\
  LoggerTester \
  RefPointerTester \
  MutexTester \
  BufferTester

TESTS=
if VS_OS_WINDOWS
else
TESTS+=$(check_PROGRAMS)
endif

LoggerTester_SOURCES= \
  LoggerTest.cpp \
  Main.cpp

nodist_LoggerTester_SOURCES= \
  LoggerTest_CXXRunner.cpp

LoggerTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la


RefPointerTester_SOURCES= \
  RefPointerTest.cpp \
  Main.cpp

nodist_RefPointerTester_SOURCES= \
  RefPointerTest_CXXRunner.cpp

RefPointerTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

MutexTester_SOURCES= \
  MutexTest.cpp \
  Main.cpp 

nodist_MutexTester_SOURCES= \
  MutexTest_CXXRunner.cpp

MutexTester_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

BufferTester_SOURCES=\
  BufferTest.cpp \
  Main.cpp

nodist_BufferTester_SOURCES=\
  BufferTest_CXXRunner.cpp

BufferTester_LDADD=\
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

all-local: $(check_PROGRAMS)

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp3
	rm -rf *.log
	rm -rf *.xml
