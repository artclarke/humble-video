include $(abs_top_builddir)/Makefile.global

# This file assume the container folder contains the following files:
# README : A file saying (at least) what revision we have captive
# csrc : The actual source we'll contingure and build
# incarcerate : A wrapper script that configures the captive package
#             : NOTE that captive packages must support VPATH builds
#             : or the captive_configure needs to force that.

# This is the main magic.  An "all" build MUST install; otherwise
# successive builds that depend on this captive project will fail.
# But, we install into a fake DESTDIR, and the container for this
# make file will then copy the fake install to the real install
# later when we're asked to install for real.
# This has the downside that every make does a captive install, but
# them's the breaks.

# This Makefile is different than standard -- it relies on the incarcerate
# script choosing the correct type of Makefile -- libz has very non standard
# build rules

PREFIX=$(VS_CROSS_PREFIX)
SHARED_MODE=0
DESTDIR=$(vs_captive_prefix)

export SHARED_MODE
export PREFIX
export CC=@CC@
export AR=@AR@
export RANLIB=@RANLIB@
export CFLAGS=@CFLAGS@
export LDFLAGS=@LDFLAGS@
export SHELL=@SHELL@
export CPP=@CPP@

all-local: incarcerate-package
	@echo "Incarcerating package $(srcdir) to fake DESTDIR=$(DESTDIR)"
	@cd csrc && \
	  ( $(MAKE) $(AM_MAKEFLAGS) -f Makefile -q SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" all || \
	    (echo "Out of date files; re-running make" && $(MAKE) $(AM_MAKEFLAGS) -f Makefile SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" all && \
	     $(MAKE) $(AM_MAKEFLAGS) -f Makefile DESTDIR=$(DESTDIR) SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" INCLUDE_PATH="$(DESTDIR)$(includedir)" LIBRARY_PATH="$(DESTDIR)$(libdir)" BINARY_PATH="$(DESTDIR)$(bindir)" install && \
	     cd .. && if test -f install.sh; then \
	       DESTDIR="$(DESTDIR)" sh install.sh; \
	     fi \
	   )\
	)
	@echo "Incarceration complete: $(srcdir)"

dist-hook:
	cp -r $(srcdir)/csrc $(distdir)/csrc
	rm -rf `find $(distdir)/csrc -name '.svn'`

install-exec-hook: incarcerate-package
	@echo "make install-exec-hook; DESTDIR=$(DESTDIR)"

install-data-hook: incarcerate-package
	@echo "make install-data-hook; DESTDIR=$(DESTDIR)"
	cd csrc && $(MAKE) $(AM_MAKEFLAGS) -f Makefile DESTDIR=$(DESTDIR) SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" INCLUDE_PATH="$(DESTDIR)$(includedir)" LIBRARY_PATH="$(DESTDIR)$(libdir)" BINARY_PATH="$(DESTDIR)$(bindir)" install
	if test -f $(builddir)/install.sh; then \
	  DESTDIR="$(DESTDIR)" sh $(builddir)/install.sh; \
	fi

uninstall-local: incarcerate-package
	if test -f $(builddir)/uninstall.sh; then \
	  DESTDIR="$(DESTDIR)" sh $(builddir)/uninstall.sh; \
	fi
	cd csrc && $(MAKE) $(AM_MAKEFLAGS) -f Makefile DESTDIR=$(DESTDIR) SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" INCLUDE_PATH="$(DESTDIR)$(includedir)" LIBRARY_PATH="$(DESTDIR)$(libdir)" BINARY_PATH="$(DESTDIR)$(bindir)" uninstall

check: incarcerate-package
	@:

clean-local: incarcerate-package
	if test -f $(builddir)/uninstall.sh; then \
	  DESTDIR="$(DESTDIR)" sh $(builddir)/uninstall.sh; \
	fi
	cd csrc && $(MAKE) $(AM_MAKEFLAGS) -f Makefile SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" clean && $(MAKE) $(AM_MAKEFLAGS) -f Makefile DESTDIR=$(DESTDIR) SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" INCLUDE_PATH="$(DESTDIR)$(includedir)" LIBRARY_PATH="$(DESTDIR)$(libdir)" BINARY_PATH="$(DESTDIR)$(bindir)" uninstall

distclean-local: incarcerate-package
	cd csrc && $(MAKE) $(AM_MAKEFLAGS) -f Makefile SHARED_MODE=$(SHARED_MODE) PREFIX="$(PREFIX)" distclean
