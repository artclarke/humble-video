VS_CPP_PATH=io/humble
VS_CPP_PACKAGE=io.humble
VS_CPP_NAMESPACE=io::humble

include @top_builddir@/mk/Makefile.global
# defines VS_PKG_LIBRARIES.  Must be in a file that automake does not
# process at autoreconf time.
include @top_builddir@/mk/Makefile.dependencies

SUBDIRS=ferry video

humbledir=$(includedir)/$(VS_CPP_PATH)
humble_HEADERS= \
  Humble.h

lib_LTLIBRARIES = 

lib_LTLIBRARIES+= libhumblevideo.la
libhumblevideo_la_SOURCES = HumbleVideo.cpp
libhumblevideo_la_LIBADD = \
  $(builddir)/ferry/libhumble-ferry.la \
  $(builddir)/video/libhumble-video.la

libhumblevideo_la_LDFLAGS = \
	-no-undefined -shared \
	-version-number @LIB_MAJOR_VERSION@:@LIB_MINOR_VERSION@:0 \
	$(VS_LDFLAGS) \
	$(VS_PKG_LIBRARIES)

if VS_OS_WINDOWS
libhumblevideo_la_LDFLAGS+= \
	-Wl,--add-stdcall-alias \
	-Wl,--enable-runtime-pseudo-reloc \
	-Wl,--enable-auto-image-base \
	-Wl,-Bsymbolic \
	-Wl,--warn-common \
	-Wl,--as-needed \
	-XCClinker -lgcc \
	-XCClinker -lmingw32 \
	-XCClinker -lmoldname \
	-XCClinker -lmingwex \
	-XCClinker -lmsvcrt \
	-XCClinker -ladvapi32 \
	-XCClinker -lshell32 \
	-XCClinker -luser32 \
	-XCClinker -lkernel32
else
if VS_OS_LINUX
libhumblevideo_la_LDFLAGS+= \
	-nostdlib -static-libtool-libs -XCClinker -static-libgcc
else
libhumblevideo_la_LDFLAGS+= \
	-nostdlib -static-libtool-libs -XCClinker -static-libgcc
endif
endif
