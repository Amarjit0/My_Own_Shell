# KaliShell Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -Iinclude
LDFLAGS = -lsqlite3 -ldl -lm

# Optional libraries
LUA_CFLAGS = $(shell pkg-config --cflags lua5.4 2>/dev/null)
LUA_LIBS = $(shell pkg-config --libs lua5.4 2>/dev/null)

CURSES_CFLAGS = $(shell pkg-config --cflags ncurses 2>/dev/null)
CURSES_LIBS = $(shell pkg-config --libs ncurses 2>/dev/null)

CFLAGS += $(LUA_CFLAGS) $(CURSES_CFLAGS)
LDFLAGS += $(LUA_LIBS) $(CURSES_LIBS)

# Source directories
SRCDIR = src
BUILDDIR = build

# Source files
SOURCES = $(wildcard $(SRCDIR)/**/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

# Target
TARGET = $(BUILDDIR)/kalishell

# Default target
all: $(TARGET)

# Create build directories
$(shell mkdir -p $(BUILDDIR)/core $(BUILDDIR)/parser $(BUILDDIR)/completion \
    $(BUILDDIR)/workspace $(BUILDDIR)/tools $(BUILDDIR)/graph \
    $(BUILDDIR)/scripting $(BUILDDIR)/tui $(BUILDDIR)/findings \
    $(BUILDDIR)/reports $(BUILDDIR)/plugins $(BUILDDIR)/ai \
    $(BUILDDIR)/utils)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Clean
clean:
	rm -rf $(BUILDDIR)

# Install
install: $(TARGET)
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: clean all

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: clean all

.PHONY: all clean install debug release
