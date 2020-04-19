OBJ = main.o
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra
LIBS = -lncurses
H_FILES = ncurses_life.h
PREFIX = /usr/local

all: options clean ncurses-life

options:
	@echo "ncurses-life make options:"
	@printf "CC\t= $(CC)\n"
	@printf "CFLAGS\t= $(CFLAGS)\n"
	@printf "PREFIX\t= $(PREFIX)\n\n"

ncurses-life: $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(OBJ)

%.o: %.c $(H_FILES)
	$(CC) $(CFLAGS) -c $<

install: clean all
	mkdir -p $(PREFIX)/bin
	cp -f ncurses-life $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/ncurses-life

uninstall:
	rm -f $(PREFIX)/bin/ncurses-life

clean:
	rm -rf ncurses-life $(OBJ)

.PHONY: all options ncurses-life install uninstall clean
