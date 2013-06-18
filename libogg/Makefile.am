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
all-local: incarcerate-package
	@echo "Incarcerating package $(srcdir) to fake DESTDIR=$(vs_captive_prefix)"
	@cd csrc && \
	  ( $(MAKE) -q all || \
	    (echo "Out of date files; re-running make" && $(MAKE) all && \
	     DESTDIR="$(vs_captive_prefix)" $(MAKE) install) \
	)
	@echo "Incarceration complete: $(srcdir)"

dist-hook:
	cp -r $(srcdir)/csrc $(distdir)/csrc
	rm -rf `find $(distdir)/csrc -name '.svn'`

install-exec-hook: incarcerate-package
	@echo "make install-exec-hook; DESTDIR=$(DESTDIR)"
	cd csrc && $(MAKE) install-exec

install-data-hook: incarcerate-package
	@echo "make install-data-hook; DESTDIR=$(DESTDIR)"
	cd csrc && $(MAKE) install-data

uninstall-local: incarcerate-package
	cd csrc && $(MAKE) uninstall

check: incarcerate-package
	cd csrc && $(MAKE) check

clean-local: incarcerate-package
	cd csrc && $(MAKE) clean && DESTDIR="$(vs_captive_prefix)" $(MAKE) uninstall

distclean-local: incarcerate-package
	cd csrc && $(MAKE) distclean
