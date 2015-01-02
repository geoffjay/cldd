dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.64])
AC_INIT([cldd], m4_esyscmd([build-aux/git-version-gen .tarball-version]))

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/libcldd-core/cldd-client.vala])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_VALAC([0.22.0])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_HEADER_STDC

AC_SUBST(abs_top_builddir)
AC_SUBST(VERSION)

# Initialize automake
AM_INIT_AUTOMAKE([1.11.1 foreign no-dist-gzip dist-xz -Wno-portability])
AM_MAINTAINER_MODE([enable])

dnl Initialize libtool - disable generation of static libraries
LT_PREREQ([2.2.6])
LT_INIT([dlopen disable-static])

GOBJECT_INTROSPECTION_REQUIRE([1.33.4])

dnl Enable silent rules is available
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_SUBST(VERSION)

AC_SET_DEFAULT_PATHS_SYSTEM

PREFIX=$prefix
AC_SUBST(PREFIX)

SYSCONFDIR="$ac_default_prefix/etc"
AC_SUBST(SYSCONFDIR)

dnl Add setup for installation directories
AC_DEFINE_UNQUOTED(DATADIR, "$SYSCONFDIR/$PACKAGE",
                   [Define the architecture-independent data directory.])

dnl Library checks
VALAFLAGS="--enable-deprecated --target-glib=2.32 $VALAFLAGS"

CLDD_BASE_MODULES="gee-0.8 libxml-2.0 cld-0.3 libzmq"
CLDD_BASE_MODULES_VALAFLAGS="--pkg gee-0.8 --pkg libxml-2.0 --pkg cld-0.3 --pkg libzmq"

PKG_CHECK_MODULES([LIBCLDD_CORE_DEPS], [$CLDD_BASE_MODULES gio-2.0 gmodule-2.0])
LIBCLDD_CORE_DEPS_VALAFLAGS="$CLDD_BASE_MODULES_VALAFLAGS --pkg gio-2.0 --pkg gmodule-2.0"
AC_SUBST([LIBCLDD_CORE_DEPS_VALAFLAGS])

CLDD_PLUGIN_COMMON_VALAFLAGS="$LIBCLDD_CORE_DEPS_VALAFLAGS"
AC_SUBST([CLDD_PLUGIN_COMMON_VALAFLAGS])

CLDD_COMMON_MODULES="$CLDD_BASE_MODULES"
CLDD_COMMON_MODULES_VALAFLAGS="$CLDD_BASE_MODULES_VALAFLAGS"
AC_SUBST([CLDD_COMMON_MODULES_VALAFLAGS])

PKG_CHECK_MODULES([CLDD_DEPS], [$CLDD_COMMON_MODULES gio-2.0 gmodule-2.0])
CLDD_DEPS_VALAFLAGS="$CLDD_COMMON_MODULES_VALAFLAGS --pkg gio-2.0 --pkg gmodule-2.0"
AC_SUBST([CLDD_DEPS_VALAFLAGS])

dnl Header checks
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([valgrind/memcheck.h])

m4_include([m4/features.m4])
m4_include([m4/systemd.m4])

AX_AVAILABLE_SYSTEMD()

dnl Check if Comedi support was requested
dnl XXX This shouldn't be required, libcld should be responsible for including it.
AC_ARG_WITH(comedi,
    AS_HELP_STRING([--without-comedi], [Do not build with Comedi support]),
    [try_comedi=$withval], [try_comedi=yes])

HAVE_COMEDI=no
AS_IF([test "x$try_comedi" = "xyes"],
      [PKG_CHECK_MODULES([CLDD_COMEDI_DEPS], [comedi],
        [
           CLDD_CHECK_PACKAGES([comedi], [HAVE_COMEDI=yes])
           CLDD_COMEDI_DEPS_VALAFLAGS="--pkg comedi"
           CLDD_PLUGIN_COMMON_VALAFLAGS="$CLDD_PLUGIN_COMMON_VALAFLAGS $CLDD_COMEDI_DEPS_VALAFLAGS"
           AC_SUBST([CLDD_COMEDI_DEPS_VALAFLAGS])
        ],
        [
          AC_MSG_WARN([Comedi dependencies not found.])
          AC_MSG_WARN([Comedi support will not be built.])
        ])])

dnl Build plugin support
CLDD_ADD_PLUGIN([arduino],[Arduino plugin],[yes])
AS_IF([test "x$enable_arduino_plugin" = "xyes"],
  [
    PKG_CHECK_MODULES([CLDD_PLUGIN_ARDUINO_DEPS],
                      [$CLDD_COMMON_MODULES])
  ])

dnl Build options
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[Enable debugging (default: disabled)]),,
        [enable_debug=no])
AS_IF([test "x$enable_debug" != "xno"],
      [AC_DEFINE(DEBUG, [1], ["Enable debugging support"])]
      [CFLAGS="$CFLAGS -O0 -g"]
      [CLDD_DEBUG_VALAFLAGS="-g -D DEBUG"]
      [AS_IF([test "x$GCC" = xyes],
             [CC_CHECK_FLAG([-g])])])

AC_SUBST(CLDD_DEBUG_VALAFLAGS)

dnl Additional build flags
AS_IF([test "x$GCC" != "xno"],
      [CC_CHECK_FLAG([-Wall])]
      [CC_CHECK_FLAG([-Wextra])]
      [CC_CHECK_FLAG([-Wno-deprecated-declarations])]
      [CC_CHECK_FLAG([-Wmissing-prototypes])]
      [CC_CHECK_FLAG([-Wshadow])]
      [CC_CHECK_FLAG([-Wpointer-arith])]
      [CC_CHECK_FLAG([-Wstrict-prototypes])]
      [CC_CHECK_FLAG([-Wcast-qual])]
      [CC_CHECK_FLAG([-Wwrite-strings])]
      [CC_CHECK_FLAG([-pedantic])])

AC_SUBST([CLDD_PLUGIN_LINKER_FLAGS],["-module -avoid-version -Wl,-z,defs"])
AC_SUBST([CLDD_PLUGIN_DIR],[${libdir}/cldd-0.1/plugins])

dnl Configuration
AC_SUBST([pkgconfigdir])

dnl Generate files
AC_CONFIG_FILES([
	Makefile
	cldd-core-0.1.pc
    data/Makefile
    data/conf/Makefile
    data/units/Makefile
	data/units/cldd.service
	data/units/cldd_ro.socket
	data/units/cldd_rw.socket
    docs/man/Makefile
	src/Makefile
	src/cldd/Makefile
	src/libcldd-core/Makefile
	src/plugins/Makefile
])
AC_OUTPUT

dnl Build report
AC_MSG_NOTICE([

    $PACKAGE $VERSION
    =============

    Prefix:                         ${ac_default_prefix}
    System configuration:           ${sysconfdir}
    Data directory:                 ${datadir}
    Package configuration:          ${pkgconfigdir}
    Compiler:                       ${CC}
    CFLAGS:                         ${CFLAGS}
    VALAFLAGS:                      ${VALAFLAGS}

    Debugging enabled:              $enable_debug

    Comedi support:                 $with_comedi

    Plugins:

    Arduino:                        $enable_arduino_plugin

    $PACKAGE will be installed in $bindir

    configure is complete, type "make" to begin.
])
