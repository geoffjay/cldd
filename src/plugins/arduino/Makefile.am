include $(top_srcdir)/common.am

plugin_LTLIBRARIES = libcldd-arduino.la
plugin_DATA = arduino.plugin

libcldd_arduino_la_SOURCES = \
	cldd-arduino-plugin.vala

libcldd_arduino_la_VALAFLAGS = \
	--target-glib=2.38 \
	--vapidir=$(top_srcdir)/vapi \
	$(CLDD_PLUGIN_COMMON_VALAFLAGS) \
	$(CLDD_COMMON_LIBCLDD_CORE_VALAFLAGS) \
	$(CLDD_COMMON_VALAFLAGS)

libcldd_arduino_la_CFLAGS = \
	-DG_LOG_DOMAIN='"Arduino"' \
	$(CLDD_PLUGIN_ARDUINO_DEPS_CFLAGS) \
	$(CLDD_COMMON_LIBCLDD_CORE_CFLAGS)

libcldd_arduino_la_LIBADD = \
	$(CLDD_PLUGIN_ARDUINO_DEPS_LIBS) \
	$(CLDD_COMMON_LIBCLDD_CORE_LIBS)

libcldd_arduino_la_LDFLAGS = \
	$(CLDD_PLUGIN_LINKER_FLAGS)

MAINTAINERCLEANFILES += $(libcldd_arduino_la_SOURCES:.vala=.c)

EXTRA_DIST = \
	arduino.plugin.in
