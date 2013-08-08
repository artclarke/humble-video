# This is needed to have the rule for csrc/Makefile correctly
# look for Configure with the right capitalization
export VS_CONFIGURE=$(srcdir)/csrc/Configure
include $(abs_top_builddir)/Makefile.global

# This file assume the container folder contains the following files:
# README : A file saying (at least) what revision we have captive
# csrc : The actual source we'll contingure and build
# incarcerate : A wrapper script that configures the captive package
#             : NOTE that captive packages must support VPATH builds
#             : or the captive_configure needs to force that.

DESTDIR=

# have to rely on the cross-compile prefix, in particular for windows
# we still allow flags to be overriden though
unexport CC
unexport LD
unexport CPP
unexport CXX
unexport NM
unexport AS
unexport STRIP
unexport RANLIB
unexport AR
unexport DLLTOOL

all-local: incarcerate-package
	@echo "Incarcerating package $(srcdir) to fake DESTDIR=$(DESTDIR)"
	@cd csrc && \
	  ( $(MAKE) -q all || \
	    (echo "Out of date files; re-running make" && $(MAKE) all && \
	     $(MAKE) INSTALL_PREFIX="$(DESTDIR)" install_sw && \
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
	cd csrc && $(MAKE) INSTALL_PREFIX="$(DESTDIR)" install_sw
	if test -f $(builddir)/install.sh; then \
	  DESTDIR="$(DESTDIR)" sh $(builddir)/install.sh; \
	fi

uninstall-local: incarcerate-package
	if test -f $(builddir)/uninstall.sh; then \
	  DESTDIR="$(DESTDIR)" sh $(builddir)/uninstall.sh; \
	fi
#	cd csrc && $(MAKE) INSTALL_PREFIX="$(DESTDIR)" uninstall

check: incarcerate-package
	@echo "make check; DESTDIR=$(DESTDIR)"
#	cd csrc && $(MAKE) check

clean-local: incarcerate-package
	if test -f $(builddir)/uninstall.sh; then \
	  DESTDIR="$(DESTDIR)" sh $(builddir)/uninstall.sh; \
	fi
	cd csrc && $(MAKE) clean && $(MAKE) INSTALL_PREFIX="$(DESTDIR)" uninstall

distclean-local: incarcerate-package
	cd csrc && $(MAKE) distclean
