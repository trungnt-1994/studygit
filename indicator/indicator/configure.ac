#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([indicator], [2.0], [tam.tranvan@toshiba-tsdv.com])
AC_CONFIG_SRCDIR([src/indicator.c])
AC_CONFIG_HEADERS([include/config.h])
AM_INIT_AUTOMAKE([1.9.6 -Wall -Werror dist-bzip2])

# Remember externally set CFLAGS
EXTERNAL_CFLAGS="$CFLAGS"

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

#AM_PATH_GTK_2_0([2.10.0],,AC_MSG_ERROR([Gtk+ 2.10.0 or higher required.]))

# Reset the externally set CFLAGS after calling AC_PROG*
CFLAGS="$EXTERNAL_CFLAGS"

# Use the C language and compiler for the following checks
AC_LANG([C])

# Checks for libraries.
PKG_CHECK_MODULES([GLIB], [glib-2.0], [have_libglib=yes], [have_libglib=no])
AM_CONDITIONAL([GLIB], [test "$have_libglib" = "yes"])

AC_SUBST([CORE_CFLAGS])
AC_SUBST([CORE_LIBS])

PKG_CHECK_MODULES([GTK], [gtk+-2.0], [
    CORE_CFLAGS="$GTK_CFLAGS"
    CORE_LIBS="$GTK_LIBS"
    ], [AC_MSG_ERROR([Gtk+ 2.10.0 or higher required.])])

PKG_CHECK_MODULES([XRANDR], [xrandr], [
    CORE_CFLAGS="$CORE_CFLAGS $XRANDR_CFLAGS"
    CORE_LIBS="$CORE_LIBS $XRANDR_LIBS"
    AC_DEFINE([ENABLE_XRANDR],[1],[Define to enable XRANDR support.])
    CONFIG_XRANDR=yes], [AC_MSG_WARN([XRANDR is not supported])])

PKG_CHECK_MODULES([X11], [x11], [
    CORE_CFLAGS="$CORE_CFLAGS $X11_CFLAGS"
    CORE_LIBS="$CORE_LIBS $X11_CFLAGS"
    AC_DEFINE([ENABLE_X11],[1],[Define to enable X11 support.])
    CONFIG_X11=yes], [AC_MSG_WARN([X11 is not supported])])

PKG_CHECK_MODULES([UDEV], [libudev], [
    CORE_CFLAGS="$CORE_CFLAGS $UDEV_CFLAGS"
    CORE_LIBS="$CORE_LIBS $UDEV_LIBS"
    AC_DEFINE([ENABLE_UDEV],[1],[Define to enable UDEV support.])
    CONFIG_UDEV=yes], [AC_MSG_WARN([UDEV is not supported])])

PKG_CHECK_MODULES([PTHREAD], [pthread-stubs], [
    CORE_CFLAGS="$CORE_CFLAGS $PTHREAD_CFLAGS"
    CORE_LIBS="$CORE_LIBS $PTHREAD_LIBS"
    AC_DEFINE([ENABLE_PTHREAD],[1],[Define to enable PTHREAD support.])
    CONFIG_PTHREAD=yes], [AC_MSG_WARN([PTHREAD is not supported])])

CORE_LIBS="$CORE_LIBS -ltzcs -ltzcs-security"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset select setlocale strdup])

# Add debug support
AC_ARG_ENABLE(debug,
  AS_HELP_STRING(
    [--enable-debug],[enable debugging, default: no]),
    [case "${enableval}" in
      yes) debug=true ;;
      no)  debug=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],
    [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
AM_COND_IF(DEBUG,
    AC_DEFINE(DEBUG,[1],[Define debug if this is a debug build]),)

# Add control panel support
AC_ARG_ENABLE(ctrlpanel,
  AS_HELP_STRING(
    [--enable-ctrlpanel],[enable control panel, default: no]),
    [case "${enableval}" in
      yes) ctrlpanel=true ;;
      no)  ctrlpanel=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-ctrlpanel]) ;;
    esac],
    [ctrlpanel=false])
AM_CONDITIONAL(CTRLPANEL, test x"$ctrlpanel" = x"true")
AM_COND_IF(CTRLPANEL,
    AC_DEFINE(ENABLE_CTRLPANEL,[1],[Try to use ctrlpanel feature]),)

# Disable switch mode of monitor
AC_ARG_ENABLE(switchmode,
  AS_HELP_STRING(
    [--disable-switchmode],[disable switch mode of monitor, default: yes]),
    [case "${enableval}" in
      yes) switchmode=true ;;
      no)  switchmode=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --disable-switchmode]) ;;
    esac],
    [switchmode=true])
AM_CONDITIONAL(SWITCHMODE, test x"$switchmode" = x"true")
AM_COND_IF(SWITCHMODE,
    ,AC_DEFINE(DISABLE_SWITCHMODE,[1],[Disable monitor switch mode on Indicator]))

# Make VDI fullscreen feature mode
AC_ARG_ENABLE(vdifullscreen,
  AS_HELP_STRING(
    [--enable-vdifullscreen],[make VDI to fullscreen mode, default: no]),
    [case "${enableval}" in
      yes) vdifullscreen=true ;;
      no)  vdifullscreen=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-vdifullscreen]) ;;
    esac],
    [vdifullscreen=false])
AM_CONDITIONAL(VDIFULLSCREEN, test x"$vdifullscreen" = x"true")
AM_COND_IF(VDIFULLSCREEN,
    AC_DEFINE(MAKE_FULLSCREEN,[1],[Make VDI to fullscreen mode on Indicator]),)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
