include $(abs_top_srcdir)/mk/doxygen.am
include $(abs_top_builddir)/mk/Makefile.global
ACLOCAL_AMFLAGS=-I m4
SUBDIRS= src
EXTRA_DIST= \
  mk/Doxyfile \
  mk/Valgrind.suppressions \
  mk/memcheck \
  mk/lib

.PHONY : doc
doc: doxygen-doc

dist-hook:
	$(MKDIR_P) $(distdir)/doc

# Remove the swig directory when all else is clean.
clean-local:
	rm -rf $(SWIGOUTTMPDIR)/$(VS_CPP_PATH)
