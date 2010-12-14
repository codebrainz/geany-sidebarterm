ACLOCAL_AMFLAGS = -I m4

pkglibdir = $(libdir)/geany

pkglib_LTLIBRARIES = sidebarterm.la
sidebarterm_la_CFLAGS = $(GEANY_CFLAGS) -DG_LOG_DOMAIN=\""Sidebar Terminal"\"
sidebarterm_la_SOURCES = sidebarterm.c
sidebarterm_la_LDFLAGS = -module -avoid-version $(GEANY_LIBS)
