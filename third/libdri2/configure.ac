#
#  Copyright 2005 Red Hat, Inc.
#
#  Permission to use, copy, modify, distribute, and sell this software and its
#  documentation for any purpose is hereby granted without fee, provided that
#  the above copyright notice appear in all copies and that both that
#  copyright notice and this permission notice appear in supporting
#  documentation, and that the name of Red Hat not be used in
#  advertising or publicity pertaining to distribution of the software without
#  specific, written prior permission.  Red Hat makes no
#  representations about the suitability of this software for any purpose.  It
#  is provided "as is" without express or implied warranty.
#
#  RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
#  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
#  EVENT SHALL RED HAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
#  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
#  DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
#  TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
#  PERFORMANCE OF THIS SOFTWARE.
#

# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([libdri2], [1.0.0],
        [https://bugs.freedesktop.org/enter_bug.cgi?product=xorg], [libdri2])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

# Initialize libtool
AC_PROG_LIBTOOL

# Require X.Org macros 1.8 or later for MAN_SUBSTS set by XORG_MANPAGE_SECTIONS
m4_ifndef([XORG_MACROS_VERSION],
          [m4_fatal([must install xorg-macros 1.8 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.8)
XORG_DEFAULT_OPTIONS
XORG_CHECK_MALLOC_ZERO

# Obtain compiler/linker options for depedencies
PKG_CHECK_MODULES(DRI2, x11 xext xextproto libdrm)

# Optionally enable simple test app...
AC_ARG_ENABLE(test,
              AS_HELP_STRING([--enable-test],
                             [build simple test app (default: disabled)]),
              [TEST=$enableval], [TEST=no])

if test "x$TEST" = xyes; then
    have_drm_driver=no

    # Check for individual driver implementations... not required for
    # libdri2, but is required for the test app to map the buffer..
# libdrm_nouveau API has changed..
#    PKG_CHECK_MODULES(NOUVEAU, libdrm_nouveau, [HAVE_NOUVEAU=yes], [HAVE_NOUVEAU=no])
#    if test "x$HAVE_NOUVEAU" = "xyes"; then
#        have_drm_driver=yes
#        AC_DEFINE(HAVE_NOUVEAU, 1, [Have nouveau support])
#    fi
    PKG_CHECK_MODULES(OMAP, libdrm_omap, [HAVE_OMAP=yes], [HAVE_OMAP=no])
    if test "x$HAVE_OMAP" = "xyes"; then
        have_drm_driver=yes
        AC_DEFINE(HAVE_OMAP, 1, [Have omap support])
    fi

    if test "x$have_drm_driver" = "xyes"; then
        AC_DEFINE(ENABLE_TEST, 1, [build simple test app])
    else
        AC_MSG_WARN([No known DRM driver, disable test app])
        TEST=no
    fi
fi
AM_CONDITIONAL(ENABLE_OMAP, [test "x$HAVE_OMAP" = xyes])
AM_CONDITIONAL(ENABLE_NOUVEAU, [test "x$HAVE_NOUVEAU" = xyes])
AM_CONDITIONAL(ENABLE_TEST, [test "x$TEST" = xyes])

# Allow checking code with lint, sparse, etc.
XORG_WITH_LINT
XORG_LINT_LIBRARY([dri2])

AC_CONFIG_FILES([Makefile
		include/Makefile
		include/X11/Makefile
		include/X11/extensions/Makefile
		src/Makefile
		test/Makefile
		dri2.pc])
AC_OUTPUT
