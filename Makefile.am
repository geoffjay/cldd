ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign 1.9 dist-bzip2 subdir-objects

# add these in later (this needs additional checking for distro)
#init_d_dir = $(sysconfdir)/rc.d
#init_d_SCRIPTS = $(PACKAGE)

SUBDIRS = \
	data \
	src \
	vapi

EXTRA_DIST = \
	$(noinst_DATA) \
	autogen.sh
