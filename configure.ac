dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.64])
AC_INIT([cldd], m4_esyscmd([build-aux/git-version-gen .tarball-version]))

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/libcldd/cldd-client.vala])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

dnl Programs
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

AC_SET_DEFAULT_PATHS_SYSTEM

PREFIX=$prefix
AC_SUBST(PREFIX)

SYSCONFDIR="$ac_default_prefix/etc"
AC_SUBST(SYSCONFDIR)

dnl Add setup for installation directories
AC_DEFINE_UNQUOTED(DATADIR, "$SYSCONFDIR/$PACKAGE",
                   [Define the architecture-independent data directory.])

dnl Library checks
VALA_PACKAGES=""

dnl Headers/Libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0],,
    [AC_MSG_ERROR([glib2 is required])])

AC_ARG_WITH(gee08, AS_HELP_STRING([--with-gee08], [Use libgee-0.8]), [with_gee08=yes], [with_gee08=no])
AS_IF([test "x$with_gee08" != "xno"],
      [AC_DEFINE(WITH_GEE_08, [1], ["Using libgee-0.8"])]
      [PKG_CHECK_MODULES([GEE], [gee-0.8], [VALA_PACKAGES="$VALA_PACKAGES --pkg gee-0.8"],
                         [AC_MSG_ERROR([libgee-0.8 is required])])])

dnl This may be more aptly named gee06
AC_ARG_WITH(gee10, AS_HELP_STRING([--with-gee10], [Use libgee-1.0]), [with_gee10=yes], [with_gee10=no])
AS_IF([test "x$with_gee10" != "xno"],
      [AC_DEFINE(WITH_GEE_10, [1], ["Using libgee-1.0"])]
      [PKG_CHECK_MODULES([GEE], [gee-1.0], [VALA_PACKAGES="$VALA_PACKAGES --pkg gee-1.0"],
                         [AC_MSG_ERROR([libgee-1.0 is required])])])

PKG_CHECK_MODULES([GIO], [gio-2.0],
    [VALA_PACKAGES="$VALA_PACKAGES --pkg gio-2.0"],
    [AC_MSG_ERROR([gio-2.0 is required])])

PKG_CHECK_MODULES([CLD], [cld-0.2],
    [VALA_PACKAGES="$VALA_PACKAGES --pkg cld-0.2"],
    [AC_MSG_ERROR([cld-0.2 is required])])

PKG_CHECK_MODULES([XML], [libxml-2.0],
    [VALA_PACKAGES="$VALA_PACKAGES --pkg libxml-2.0"],
    [AC_MSG_ERROR([libxml-2.0 is required])])

PKG_CHECK_MODULES([ZMQ], [libzmq],
    [VALA_PACKAGES="$VALA_PACKAGES --pkg libzmq"],
    [AC_MSG_ERROR([libzmq is required])])

dnl Comedi
AC_ARG_WITH(comedi, AS_HELP_STRING([--with-comedi], [Use Comedi drivers]), [with_comedi=yes], [with_comedi=no])
AS_IF([test "x$with_comedi" != "xno"],
      [AC_DEFINE(USE_COMEDI, [1], ["Comedi drivers"])]
      [PKG_CHECK_MODULES([COMEDI], [comedi],
                         [VALA_PACKAGES="$VALA_PACKAGES --pkg comedi"],
                         [AC_MSG_ERROR([Comedi is required])])])

dnl Package setup for Vala files
AC_SUBST(VALA_PACKAGES)

dnl Header checks
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([valgrind/memcheck.h])

dnl Build options.
AC_ARG_WITH([distro],
	[AS_HELP_STRING([--with-distro],
					[Configure for a specific distribution (supported values: generic, redhat, suse, debian, gentoo, slackware; default is to autodetect)])],
	[DISTRO=$withval])
AC_ARG_WITH([initdir],
	[AS_HELP_STRING([--with-initdir],
					[Override directory for init scripts (default is distribution-specific)])],
	[INITDIR=$withval])

dnl INITDIR may be set with --with-initdir, or set in the distro
dnl detection magic above. If unset down to here, use a sensible
dnl default.
test -z $INITDIR && INITDIR="$sysconfdir/init.d"

AC_SUBST(DISTRO)
AC_SUBST(INITDIR)

dnl check which distribution we're running on in order to correctly handle
dnl installation of the init.d/rc.d script
if test -z $DISTRO; then
	AC_CHECK_FILE(/etc/arch-release,   [DISTRO="arch"])
	AC_CHECK_FILE(/etc/gentoo-release, [DISTRO="gentoo"])
	AC_CHECK_FILE(/etc/redhat-release, [DISTRO="redhat"])
	AC_CHECK_FILE(/etc/debian_version, [DISTRO="debian"])
fi
case "$DISTRO" in
	arch)
		AC_MSG_NOTICE([configured for Arch.])
		;;
	gentoo)
		AC_MSG_NOTICE([configured for Gentoo.])
		;;
	redhat)
		test -z $INITDIR && INITDIR="$sysconfdir/rc.d/init.d"
		AC_MSG_NOTICE([configured for Red Hat (includes Fedora, RHEL, CentOS).])
		;;
	debian)
		AC_MSG_NOTICE([configured for Debian (includes Ubuntu).])
		;;
	"")
		AC_MSG_WARN([Unable to determine what distribution we are running on.
					 Distribution-specific features will be disabled.])
		;;
esac

dnl Allow tools to be specifically built
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],
		[enable debugging (default: disabled)]),,
	[enable_debug=no])

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

dnl Generate files
AC_CONFIG_FILES([
	Makefile
    data/Makefile
    data/conf/Makefile
    data/rc.d/Makefile
    man/Makefile
	src/Makefile
	src/cldd/Makefile
	src/libcldd/Makefile
	src/plugins/Makefile
	vapi/Makefile
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

    $PACKAGE will be installed in $bindir

    configure is complete, type "make" to begin.
])
