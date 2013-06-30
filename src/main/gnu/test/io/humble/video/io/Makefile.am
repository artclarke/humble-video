VS_PRODUCT=humble_video_io
VS_CPP_PATH=io/humble/video/io
VS_CPP_PACKAGE=io.humble.video.io
VS_CPP_NAMESPACE=io::humble::video::io
VS_TEST=1
include @top_builddir@/mk/Makefile.global

check_PROGRAMS=\
  StdioURLProtocolHandlerTest

inst_check=$(check_PROGRAMS)
inst_checkdir=$(bindir)

# Make sure we set the abs_top_srcdir environment variable so the 
# tests can auto-find their fixtures
TESTS=$(check_PROGRAMS)

StdioURLProtocolHandlerTest_SOURCES= \
  StdioURLProtocolHandlerTest.cpp \
  Main.cpp

nodist_StdioURLProtocolHandlerTest_SOURCES= \
  StdioURLProtocolHandlerTest_CXXRunner.cpp

StdioURLProtocolHandlerTest_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la

BUILT_SOURCES= \
  StdioURLProtocolHandlerTest_CXXRunner.cpp

noinst_HEADERS = \
  StdioURLProtocolHandlerTest.h

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp4
	rm -rf *.mp3
	rm -rf *.ogg
	rm -rf memcheck*.log
