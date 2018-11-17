VO_COMMON = common
ENC_SRC = aacenc/src

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -I$(top_srcdir)/aacenc/inc -I$(top_srcdir)/aacenc/basic_op -I$(top_srcdir)/common/include

if ARMV7NEON
    AM_CFLAGS += -DARMV5E -DARMV7Neon -DARM_INASM -DARMV5_INASM -DARMV6_INASM
else
if ARMV5E
    AM_CFLAGS += -DARMV5E -DARM_INASM -DARMV5_INASM
endif
endif

aacencincludedir = $(includedir)/vo-aacenc
aacencinclude_HEADERS = $(top_srcdir)/$(VO_COMMON)/include/*.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vo-aacenc.pc

lib_LTLIBRARIES = libvo-aacenc.la

libvo_aacenc_la_LDFLAGS = -version-info @VO_AACENC_VERSION@ -no-undefined -export-symbols $(top_srcdir)/vo-aacenc.sym

libvo_aacenc_la_SOURCES = \
    $(VO_COMMON)/cmnMemory.c \
    aacenc/basic_op/basicop2.c \
    aacenc/basic_op/oper_32b.c \
    $(ENC_SRC)/aac_rom.c \
    $(ENC_SRC)/aacenc.c \
    $(ENC_SRC)/aacenc_core.c \
    $(ENC_SRC)/adj_thr.c \
    $(ENC_SRC)/band_nrg.c \
    $(ENC_SRC)/bit_cnt.c \
    $(ENC_SRC)/bitbuffer.c \
    $(ENC_SRC)/bitenc.c \
    $(ENC_SRC)/block_switch.c \
    $(ENC_SRC)/channel_map.c \
    $(ENC_SRC)/dyn_bits.c \
    $(ENC_SRC)/grp_data.c \
    $(ENC_SRC)/interface.c \
    $(ENC_SRC)/line_pe.c \
    $(ENC_SRC)/memalign.c \
    $(ENC_SRC)/ms_stereo.c \
    $(ENC_SRC)/pre_echo_control.c \
    $(ENC_SRC)/psy_configuration.c \
    $(ENC_SRC)/psy_main.c \
    $(ENC_SRC)/qc_main.c \
    $(ENC_SRC)/quantize.c \
    $(ENC_SRC)/sf_estim.c \
    $(ENC_SRC)/spreading.c \
    $(ENC_SRC)/stat_bits.c \
    $(ENC_SRC)/tns.c \
    $(ENC_SRC)/transform.c

if ARMV7NEON
    libvo_aacenc_la_SOURCES += \
        $(ENC_SRC)/asm/ARMV5E/AutoCorrelation_v5.s \
        $(ENC_SRC)/asm/ARMV5E/CalcWindowEnergy_v5.s \
        $(ENC_SRC)/asm/ARMV5E/band_nrg_v5.s \
        $(ENC_SRC)/asm/ARMV7/PrePostMDCT_v7.s \
        $(ENC_SRC)/asm/ARMV7/R4R8First_v7.s \
        $(ENC_SRC)/asm/ARMV7/Radix4FFT_v7.s
else
if ARMV5E
    libvo_aacenc_la_SOURCES += \
        $(ENC_SRC)/asm/ARMV5E/AutoCorrelation_v5.s \
        $(ENC_SRC)/asm/ARMV5E/CalcWindowEnergy_v5.s \
        $(ENC_SRC)/asm/ARMV5E/band_nrg_v5.s \
        $(ENC_SRC)/asm/ARMV5E/PrePostMDCT_v5.s \
        $(ENC_SRC)/asm/ARMV5E/R4R8First_v5.s \
        $(ENC_SRC)/asm/ARMV5E/Radix4FFT_v5.s
endif
endif

noinst_HEADERS = $(top_srcdir)/aacenc/inc/*.h $(top_srcdir)/aacenc/basic_op/*.h

if EXAMPLE
bin_PROGRAMS = aac-enc$(EXEEXT)

aac_enc_LDADD = libvo-aacenc.la
aac_enc_SOURCES = aac-enc.c wavreader.c

noinst_HEADERS += wavreader.h
endif


EXTRA_DIST = $(top_srcdir)/NOTICE \
             $(top_srcdir)/vo-aacenc.sym \
             $(top_srcdir)/aacenc/NOTICE \
             $(top_srcdir)/aacenc/MODULE_LICENSE_APACHE2 \
             $(top_srcdir)/aacenc/*.cpp $(top_srcdir)/aacenc/*.h \
             $(top_srcdir)/aacenc/*.mk \
             $(top_srcdir)/aacenc/SampleCode $(top_srcdir)/aacenc/doc \
             $(top_srcdir)/aacenc/*.txt $(top_srcdir)/common/*.mk \
             $(top_srcdir)/common/NOTICE \
             $(top_srcdir)/common/MODULE_LICENSE_APACHE2

