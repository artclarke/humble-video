VS_PRODUCT=humblevideo
VS_CPP_PATH=io/humble/video
VS_CPP_PACKAGE=io.humble.video
VS_CPP_NAMESPACE=io::humble::video
VS_TEST=1
include @top_builddir@/mk/Makefile.global
SUBDIRS=

check_PROGRAMS=\
  videoTestProperty \
  videoTestRational 

inst_check=$(check_PROGRAMS)
inst_checkdir=$(bindir)

# Make sure we set the abs_top_srcdir environment variable so the 
# tests can auto-find their fixtures
TESTS=$(check_PROGRAMS)

videoTestProperty_SOURCES= \
  PropertyTest.cpp \
  Main.cpp

nodist_videoTestProperty_SOURCES= \
  PropertyTest_CXXRunner.cpp

videoTestProperty_LDADD= \
  $(top_builddir)/src/io/humble/libhumblevideo.la 

videoTestRational_SOURCES=\
  RationalTest.cpp \
  Main.cpp

nodist_videoTestRational_SOURCES=\
  RationalTest_CXXRunner.cpp 

videoTestRational_LDADD=\
  $(top_builddir)/src/io/humble/libhumblevideo.la 

BUILT_SOURCES= \
  PropertyTest_CXXRunner.cpp \
  RationalTest_CXXRunner.cpp

noinst_HEADERS = \
  PropertyTest.h \
  RationalTest.h

clean-local:
	rm -rf $(BUILT_SOURCES)
	rm -rf *.flv
	rm -rf *.mov
	rm -rf *.mp4
	rm -rf *.mp3
	rm -rf *.ogg
