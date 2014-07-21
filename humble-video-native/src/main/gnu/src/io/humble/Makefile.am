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
# A C file to force GCC to link
libhumblevideo_la_SOURCES = HumbleVideo.c
libhumblevideo_la_LIBADD = \
  $(builddir)/ferry/libhumble-ferry.la \
  $(builddir)/video/customio/libhumble-video-customio.la \
  $(builddir)/video/libhumble-video.la

# But because we're forcing C linkage, we need to tell the linker
# about which STDCXX and which exception handling library to use.
libhumblevideo_la_LDFLAGS = \
	-no-undefined -shared \
	-version-number @LIB_MAJOR_VERSION@:@LIB_MINOR_VERSION@:0 \
	$(VS_LDFLAGS) \
	$(VS_PKG_LIBRARIES) \
	@VS_LIBSTDCXX@ \
	@VS_LIBGCC_EH@

if VS_OS_WINDOWS
libhumblevideo_la_LDFLAGS+= \
	-all-static \
	-Wl,--add-stdcall-alias \
	-Wl,--enable-runtime-pseudo-reloc \
	-Wl,--enable-auto-image-base \
	-Wl,-Bsymbolic \
	-Wl,-Bstatic \
	-XCClinker -static \
	-Wl,--warn-common \
	-Wl,--as-needed \
	-static \
	-nodefaultlib \
	-static-libtool-libs \
	-static-libgcc -static-libstdc++ \
	@VS_LIBGCC@ \
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
# Should be Mac OS X
libhumblevideo_la_LDFLAGS+= \
	-nostdlib -static-libtool-libs
endif
endif
