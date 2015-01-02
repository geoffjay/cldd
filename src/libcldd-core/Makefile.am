include $(top_srcdir)/common.am
include $(srcdir)/filelist.am

libclddincdir = $(includedir)/cldd-0.1

libclddinc_HEADERS = cldd-core.h

lib_LTLIBRARIES = libcldd-core-0.1.la

libcldd_core_0_1_la_SOURCES = \
	$(LIBCLDD_CORE_VAPI_SOURCE_FILES) \
	$(LIBCLDD_CORE_NONVAPI_SOURCE_FILES)

libcldd_core_0_1_la_VALAFLAGS = \
	-H cldd-core.h -C --library=cldd-core-0.1 \
	--gir=Cldd-0.1.gir \
	--vapidir=$(srcdir) \
	--pkg posix \
	$(LIBCLDD_CORE_DEPS_VALAFLAGS) \
	$(CLDD_COMMON_VALAFLAGS)

libcldd_core_0_1_la_CFLAGS = \
	$(CLDD_COMMON_DEFINES_CFLAGS) \
	$(LIBCLDD_CORE_DEPS_CFLAGS) \
	-DPLUGIN_DIR='"$(plugindir)"' \
	-include config.h \
	-DG_LOG_DOMAIN='"Cldd"'

libcldd_core_0_1_la_LIBADD = \
	$(LIBCLDD_CORE_DEPS_LIBS)

CLDD_CORE_VERSION=0:1:0
libcldd_core_0_1_la_LDFLAGS = \
	$(CLDD_DATA_DIR_DEFINES) \
	-version-info $(CLDD_CORE_VERSION)

BUILT_SOURCES = \
	cldd-core-0.1.vapi \
	cldd-core.h

VAPI_FILES = cldd-core-0.1.vapi
DEPS_FILES = cldd-core-0.1.deps

vapidir = $(VAPIDIR)
vapi_DATA = $(VAPI_FILES) $(DEPS_FILES)
cldd-core-0.1.vapi cldd-core.h Cldd-0.1.gir: libcldd_core_0_1_la_vala.stamp

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS=
INTROSPECTION_SCANNER_ARGS=
INTROSPECTION_COMPILER_ARGS= -l libcldd-core-0.1.so.2
$(builddir)/ClddCore-0.1.gir: $(srcdir)/Cldd-0.1.gir
	$(AM_V_GEN)$(SED) -e 's,Cldd\.,ClddCore.,g' -e 's,namespace name="Cldd",namespace name="ClddCore",g' $< >$@
GIRS = ClddCore-0.1.gir

girdir=$(datadir)/gir-1.0
dist_gir_DATA = $(GIRS)

typelibdir = $(libdir)/girepository-1.0
nodist_typelib_DATA = $(GIRS:.gir=.typelib)

DISTCLEANFILES = ClddCore-0.1.typelib

MAINTAINERCLEANFILES += $(BUILT_SOURCES)

EXTRA_DIST = \
	cldd-config.vapi \
	$(VAPI_FILES) \
	$(DEPS_FILES) \
	$(BUILT_SOURCES) \
	ClddCore-0.1.gir \
	Cldd-0.1.gir
