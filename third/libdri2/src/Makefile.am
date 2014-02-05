lib_LTLIBRARIES = libdri2.la

libdri2_la_SOURCES = \
         dri2.c

libdri2_la_LIBADD = @DRI2_LIBS@
AM_CFLAGS = $(CWARNFLAGS) @DRI2_CFLAGS@ -I$(top_srcdir)/include @MALLOC_ZERO_CFLAGS@

libdri2_la_LDFLAGS = -version-number 1:0:0 -no-undefined

if LINT
ALL_LINT_FLAGS=$(LINT_FLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		$(AM_CPPFLAGS) $(CPPFLAGS)

lint:
	$(LINT) $(ALL_LINT_FLAGS) $(libdri2_la_SOURCES) $(DRI2_LIBS)
endif LINT

if MAKE_LINT_LIB
lintlibdir = $(libdir)

lintlib_DATA = $(LINTLIB)

$(LINTLIB): $(libXau_la_SOURCES)
	$(LINT) -y -oDRI2 -x $(ALL_LINT_FLAGS) $(libdri2_la_SOURCES)

CLEANFILES = $(LINTLIB)
endif MAKE_LINT_LIB
