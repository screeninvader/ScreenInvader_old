# XMMS library

AM_CFLAGS = @XMMS_CFLAGS@ @GTK_CFLAGS@ @GDK_PIXBUF_CFLAGS@ \
	-I$(top_srcdir)/src -DXMMS_PIXMAPDIR=\"$(XMMS_PIXMAPDIR)\"

xmmsplugindir = @XMMS_PLUGIN_DIR@

# Don't build a static library
LIBTOOL = $(top_builddir)/libtool-disable-static
 
if BUILD_NEW_PLUGIN
NEW_xmmsplugin = libxmms_osd.la
endif
if BUILD_OLD_PLUGIN
OLD_xmmsplugin = libxmms_osd_old.la
endif
xmmsplugin_LTLIBRARIES = $(NEW_xmmsplugin) $(OLD_xmmsplugin)

libxmms_osd_la_SOURCES = xmms_osd.c dlg_config.c dlg_font.c dlg_colour.c xmms_osd.h
libxmms_osd_la_LIBADD  = $(top_builddir)/src/libxosd/libxosd.la
libxmms_osd_la_LDFLAGS = -module -avoid-version @GDK_PIXBUF_LIBS@
 
libxmms_osd_old_la_SOURCES = xmms_osd.c dlg_config_old.c dlg_font.c dlg_colour.c xmms_osd.h
libxmms_osd_old_la_LIBADD  = $(top_builddir)/src/libxosd/libxosd.la
libxmms_osd_old_la_LDFLAGS = -module -avoid-version
