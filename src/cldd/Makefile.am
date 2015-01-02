include $(top_srcdir)/common.am

bin_PROGRAMS = cldd

cldd_VALAFLAGS = \
	--target-glib=2.32 \
	-H $(srcdir)/libcldd.h -C -c --thread \
	--vapidir=$(srcdir)/../vapi \
	--pkg posix --pkg linux \
	$(CLDD_DEPS_VALAFLAGS)

cldd_CFLAGS = \
	--include config.h \
	-DG_LOG_DOMAIN='"Cldd"' \
	-DLOCALEDIR=\""$(datadir)/locale"\" \
	$(AM_CFLAGS) \
	$(SYSTEMD_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GEE_CFLAGS) \
	$(CLD_CFLAGS) \
	$(XML_CFLAGS) \
	$(COMEDI_CFLAGS) \
	$(ZMQ_CFLAGS)

cldd_LDADD = \
	$(SYSTEMD_LIBS) \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GEE_LIBS) \
	$(CLD_LIBS) \
	$(XML_LIBS) \
	$(COMEDI_LIBS) \
	$(ZMQ_LIBS)

cldd_headers = \
	cldd.h \
	libcldd.h

cldd_SOURCES = \
	cldd-main.vala \
	cldd-configuration.vala \
	cldd-daemon.vala \
	cldd-settings.vala

CLEANFILES = \
	*.stamp \
	stamp-h1 \
	$(srcdir)/libcldd.h

# Not really sure whether or not this is useful yet

.PHONY: reload
reload:
	-systemctl daemon-reload

.PHONY: disable
disable:
	systemctl disable cldd.service
	systemctl disable cldd_rw.socket
	systemctl disable cldd_ro.socket

.PHONY: enable
enable:
	make disable
	systemctl daemon-reload
	systemctl enable cldd.service
	systemctl enable cldd_rw.socket
	systemctl enable cldd_ro.socket

install-sockets:
	cp units/*.socket @SYSTEMD_DIR@/

install-data-am:
	make install-sockets
	cp cldd_regular $(bindir)/
	cp cldd_dynamic $(bindir)/
	cp units/cldd.service @SYSTEMD_DIR@/
	make reload
	make enable

maintainer-clean:
	-make clean
	-rm -rf autom4te.cache configure config.status config.log \
		config.h config.h.in config.h.in~ stamp-h1 aclocal.m4 \
		build-aux Makefile.in Makefile Config.mk \
		units/*.service units/*.socket

dist-clean:
	-make maintainer-clean

.PHONY: status
status:
	-systemctl status cldd_rw.socket
	-systemctl status cldd_ro.socket
	-systemctl status cldd.service

.PHONY: start
start:
	-systemctl start cldd_rw.socket
	-systemctl start cldd_ro.socket
	-systemctl start cldd.service
	-make status

.PHONY: stop
stop:
	-systemctl stop cldd.service
	-systemctl stop cldd_rw.socket
	-systemctl stop cldd_ro.socket
	-make status

.PHONY: restart
restart:
	@-make stop
	@-make start
