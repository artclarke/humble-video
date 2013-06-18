ACLOCAL_AMFLAGS=-I m4
include $(abs_top_builddir)/Makefile.global
EXTRA_DIST=COPYING
# Order matters; dependency tree is:
#
# libz---------------------------------------------------------------------
#  |                                                                       |
# openssl                                                    ogg         tiff
#    |                                                        |            |
#  -----                                             -------------------  spandsp
#  |   |                                            |      |     |      |  |
#  | rtmp vo-aacenc opencore-amr mp3lame x264 vpx speex vorbis theora   |  |
#  |   |      |          |          |     |    |    |      |     |      |  |
#  ---------------------------------------------------------------------   |
#                                   |                                      |
#                               ffmpeg     liboil                          |
#                                   |         |                            |
#                                   ---------------------------------------
#                                                        |
#                                                      xuggle
SUBDIRS=
#if VS_CAPTIVE_ZLIB
SUBDIRS+= zlib
#endif

# For futures; include in static builds for now so users
# don't complain later when they increase the size of the jars
#SUBDIRS+= libtiff spandsp

#SUBDIRS+= liboil

SUBDIRS+= \
  libvpx openssl librtmp libvo-aacenc libopencore-amr \
  libogg libspeex libvorbis libtheora libmp3lame

if VS_ENABLE_GPL
SUBDIRS+=libx264
endif
SUBDIRS+=ffmpeg

# We delete the libtool generated .la files because they cause link errors for xuggle
all-local:
	@-find "$(vs_captive_prefix)" -name '*.la' | xargs rm 
