include $(abs_top_builddir)/mk/Makefile.global
ACLOCAL_AMFLAGS=-I m4
SUBDIRS= src
EXTRA_DIST= \
  mk/Doxyfile \
  mk/Valgrind.suppressions \
  mk/memcheck \
  mk/lib

DOC_DIR=$(abs_top_srcdir)/target/api

.PHONY : doc
if HAVE_DOXYGEN
doc:
	$(MKDIR_P) $(DOC_DIR)
	cd $(abs_top_srcdir) && $(DOXYGEN) $(abs_top_srcdir)/mk/Doxyfile
else
doc:
	@echo "You must have 'doxygen' installed on your system to generate"
	@echo "the documentation.  Try installing it, and running"
	@echo "'configure' again."
endif

dist-hook:
	$(MKDIR_P) $(distdir)/doc
