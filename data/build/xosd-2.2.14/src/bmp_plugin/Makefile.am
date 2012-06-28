# XMMS library

AM_CFLAGS = @BMP_CFLAGS@ -I$(top_srcdir)/src -DXMMS_PIXMAPDIR=\"$(XMMS_PIXMAPDIR)\"

bmpplugindir = @BMP_GENERAL_PLUGIN_DIR@

# Don't build a static library
LIBTOOL = $(top_builddir)/libtool-disable-static

if BUILD_BEEP_MEDIA_PLUGIN
if BUILD_NEW_PLUGIN
NEW_bmpplugin = libbmp_osd.la
endif
if BUILD_OLD_PLUGIN
OLD_bmpplugin = libbmp_osd_old.la
endif
endif

bmpplugin_LTLIBRARIES = $(NEW_bmpplugin) $(OLD_bmpplugin)

libbmp_osd_la_SOURCES = bmp_osd.c dlg_config.c dlg_font.c dlg_colour.c bmp_osd.h
libbmp_osd_la_LIBADD  = $(top_builddir)/src/libxosd/libxosd.la
libbmp_osd_la_LDFLAGS = -module -avoid-version @GDK_PIXBUF_LIBS@

libbmp_osd_old_la_SOURCES = bmp_osd.c dlg_config_old.c dlg_font.c dlg_colour.c bmp_osd.h
libbmp_osd_old_la_LIBADD  = $(top_builddir)/src/libxosd/libxosd.la
libbmp_osd_old_la_LDFLAGS = -module -avoid-version
