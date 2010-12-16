#
# Makefile for Geany Vertical Terminal
#

ifndef PREFIX
PREFIX = /usr/local
endif

all: sidebarterm.so

sidebarterm.so: sidebarterm.o
	gcc sidebarterm.o -o sidebarterm.so -shared `pkg-config --libs geany vte`

sidebarterm.o: sidebarterm.c
	gcc -c sidebarterm.c -fPIC `pkg-config --cflags geany vte`

install:
	install -m 0644 sidebarterm.so $(PREFIX)/lib/geany

uninstall:
	rm -f $(PREFIX)/lib/geany/sidebarterm.so

clean:
	rm -f sidebarterm.o sidebarterm.so
