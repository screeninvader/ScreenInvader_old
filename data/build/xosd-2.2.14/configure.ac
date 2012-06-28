dnl
dnl Configure.ac for xosd
dnl

dnl Process this file with autoconf to produce a configure script.
AC_INIT(xosd, 2.2.14)
AC_CONFIG_SRCDIR(src/testprog.c)
AM_INIT_AUTOMAKE([no-define])

dnl
dnl Require autoconf version 2.57
dnl
AC_PREREQ(2.57)

dnl The libtool version numbers (LT_*); Don't even think about faking this!
dnl
dnl immediately before every release do:
dnl ===================================
dnl if (the interface is totally unchanged from previous release)
dnl    LT_REVISION ++;
dnl else { /* interfaces have been added, removed or changed */
dnl    LT_REVISION = 0;
dnl    LT_CURRENT ++;
dnl    if (any interfaces have been _added_ since last release)
dnl       AGE ++;
dnl    if (any interfaces have been _removed_ or _incompatibly changed_)
dnl       AGE = 0;
dnl }
LT_CURRENT=4
LT_AGE=2
LT_REVISION=14

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_AGE)
AC_SUBST(LT_REVISION)

AM_MAINTAINER_MODE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AM_PROG_LIBTOOL
AC_PATH_XTRA([X11],[X11/Xlib.h],[XrmInitialize()])
# We need two libtools, one that builds both shared and static, and
# one that builds only static.  This is because the resulting libtool
# does not allow us to choose which to build at runtime.
sed -e 's/^build_old_libs=yes/build_old_libs=no/' libtool > libtool-disable-static
chmod +x libtool-disable-static

LIBS="$LIBS $X_LIBS $X_EXTRA_LIBS"
CFLAGS="$CFLAGS $X_CFLAGS -Wall"
M4DATADIR="$datadir/aclocal"
AC_SUBST(M4DATADIR)

dnl Checks for libraries.
AM_PATH_GTK(1.2.2,,
	    AC_MSG_WARN([*** GTK+ >= 1.2.2 not found ***]))
AC_CHECK_LIB(X11, XInitThreads,,
	     AC_MSG_ERROR([*** X11 not threadsafe ***]))
AC_CHECK_LIB(Xext, XShapeQueryExtension,,
	     AC_MSG_ERROR([*** X11 Shape extension not found ***]))
AC_CHECK_LIB(pthread, pthread_create,,
	     AC_MSG_ERROR([*** POSIX thread support not found ***]))

dnl Check for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADER(pthread.h,,
		AC_MSG_ERROR([*** POSIX thread support not installed ***]))

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_FUNC_VPRINTF

# nicked from the irmix project...
AC_ARG_ENABLE([xinerama],
              AC_HELP_STRING([--disable-xinerama],
			     [disable use of Xinerama extension]),
              [disable_xinerama="yes"],
	      [disable_xinerama="no"])

if test x$disable_xinerama = "xno"
then
        AC_CHECK_LIB(Xinerama_pic,
                     XineramaQueryExtension,
                     [X_LIBS="$X_LIBS -lXinerama_pic"
                      AC_DEFINE(HAVE_XINERAMA,1,[Define this if you have libXinerama installed])
                      ac_have_xinerama="yes"],
                      AC_CHECK_LIB(Xinerama,
                                   XineramaQueryExtension,
                                   [X_LIBS="$X_LIBS -lXinerama"
                                    AC_DEFINE(HAVE_XINERAMA,1,[Define this if you have libXinerama
installed])
                                    ac_have_xinerama="yes"],,
                                    [$X_LIBS -lXext $X_EXTRA_LIBS]),
                     [$X_LIBS -lXext $X_EXTRA_LIBS])
fi

if pkg-config --exists bmp
then
	PKG_CHECK_MODULES(BMP, bmp)
	BMP_GENERAL_PLUGIN_DIR=$(pkg-config --variable general_plugin_dir bmp)
	AC_SUBST(BMP_GENERAL_PLUGIN_DIR)
	no_bmp="yes"
else
	no_bmp="no"	
	AC_MSG_WARN("beep media player not found")
fi




dnl Check for xmms
AM_PATH_XMMS(1.2.7,,
	     AC_MSG_WARN("xmms plugins can not be built"))
dnl Override where to place libxmms_xosd.so
AC_ARG_WITH([plugindir],
            AC_HELP_STRING([--with-plugindir=DIR],
			   [Set the xmms plugin directory]),
            [XMMS_PLUGIN_DIR="$withval"],
            [XMMS_PLUGIN_DIR="$XMMS_GENERAL_PLUGIN_DIR"])

dnl Check for gdk-pixbuf
AM_PATH_GDK_PIXBUF(0.22.0,,
		   AC_MSG_WARN("new xmms plugin can not be built"))
dnl Override where to place pixmaps for libxmms_xosd.so
AC_ARG_WITH([pixmapdir],
	    AC_HELP_STRING([--with-pixmapdir=DIR],
			   [Set the directory for xmms plugin pixmaps]),
            [XMMS_PIXMAPDIR="$withval"],
            [XMMS_PIXMAPDIR="${datadir}/xosd"])
AC_SUBST(XMMS_PIXMAPDIR)

dnl NEW plugin
AC_ARG_ENABLE([new-plugin],
	      AC_HELP_STRING([--disable-new-plugin],
			     [Disable new xmms plugin (enabled by default)]),
              [enable_new_plugin="$enableval"],
	      [enable_new_plugin="yes"])
AC_MSG_CHECKING([whether new xmms plugin was requested])
AC_MSG_RESULT($enable_new_plugin)
AM_CONDITIONAL([BUILD_NEW_PLUGIN],
	       [test x"$enable_new_plugin" = "xyes" -a x"$no_xmms" != "xyes" -a x"$no_gdk_pixbuf" != "xyes"])


dnl Check for Beep Media player
AC_ARG_ENABLE([beep_media_player_plugin],
		AC_HELP_STRING([--enable-beep_media_player_plugin],
			     [Enable beep media plugin (enabled by default)]),
              [beep_media_player_plugin="$enableval"],
              [beep_media_player_plugin="yes"])
AC_MSG_CHECKING([whether beep media plugin was requested])
AC_MSG_RESULT($beep_media_player_plugin)

AM_CONDITIONAL([BUILD_BEEP_MEDIA_PLUGIN],
	       [test x"$beep_media_player_plugin" = "xyes" -a x"$no_bmp" == "xyes"])

		


dnl OLD plugin
AC_ARG_ENABLE([old-plugin],
	      AC_HELP_STRING([--enable-old-plugin],
			     [Enable old xmms plugin (disabled by default)]),
              [enable_old_plugin="$enableval"],
              [enable_old_plugin="no"])
AC_MSG_CHECKING([whether old xmms plugin was requested])
AC_MSG_RESULT($enable_old_plugin)
AM_CONDITIONAL([BUILD_OLD_PLUGIN],
	       [test x"$enable_old_plugin" = "xyes" -a x"$no_xmms" != "xyes"])


dnl Define XOSD_VERSION
AC_DEFINE_UNQUOTED(XOSD_VERSION, "${VERSION}")

EXTRA_DIST=xosd.spec

dnl Produce Output files
AC_CONFIG_FILES([
Makefile
src/Makefile
src/libxosd/Makefile
src/xmms_plugin/Makefile
src/bmp_plugin/Makefile
macros/Makefile
man/Makefile
pixmaps/Makefile
script/Makefile
script/xosd-config
xosd.spec
])
AC_CONFIG_COMMANDS([default],[[chmod +x script/xosd-config]],[[]])
AC_OUTPUT
