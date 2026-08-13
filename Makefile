# SPDX-License-Identifier: GPL-3.0-or-later

CC ?= cc
AR ?= ar
BUILD_DIR ?= build
CPPFLAGS += -Iinclude
CFLAGS ?= -O2 -g
override CFLAGS += -std=c11 -fPIC -Wall -Wextra -Wpedantic -Werror \
	-Wshadow -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes
PORTABLE_OBJECTS := $(BUILD_DIR)/core.o $(BUILD_DIR)/format.o
OBJECTS := $(PORTABLE_OBJECTS) $(BUILD_DIR)/posix.o
PORTABLE_ARCHIVE := $(BUILD_DIR)/libinfiltratr-portable.a
ARCHIVE := $(BUILD_DIR)/libinfiltratr-common.a
SHARED := $(BUILD_DIR)/libinfiltratr-common.so.1.4.0

.PHONY: all check clean portable portable-check shared

all: $(ARCHIVE)

$(BUILD_DIR):
	mkdir -p "$@"

$(BUILD_DIR)/core.o: src/core.c include/infiltratr/core.h \
	include/infiltratr/compiler.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/format.o: src/format.c include/infiltratr/format.h \
	include/infiltratr/core.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/posix.o: src/posix.c include/infiltratr/core.h \
	include/infiltratr/posix.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(PORTABLE_ARCHIVE): $(PORTABLE_OBJECTS)
	$(AR) rcsD $@ $(PORTABLE_OBJECTS)

portable: $(PORTABLE_ARCHIVE)

$(ARCHIVE): $(OBJECTS)
	$(AR) rcsD $@ $(OBJECTS)

shared: $(SHARED)

$(SHARED): $(OBJECTS)
	$(CC) -shared -Wl,-soname,libinfiltratr-common.so.1 \
		-o $@ $(OBJECTS) -lm

$(BUILD_DIR)/core-smoke: tests/core_smoke.c $(ARCHIVE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(ARCHIVE) -lm -o $@

$(BUILD_DIR)/format-smoke: tests/format_smoke.c $(ARCHIVE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(ARCHIVE) -lm -o $@

$(BUILD_DIR)/portable-smoke: tests/portable_smoke.c $(PORTABLE_ARCHIVE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(PORTABLE_ARCHIVE) -lm -o $@

portable-check: $(BUILD_DIR)/portable-smoke
	./$(BUILD_DIR)/portable-smoke

check: $(BUILD_DIR)/core-smoke $(BUILD_DIR)/format-smoke $(BUILD_DIR)/portable-smoke
	./$(BUILD_DIR)/core-smoke
	./$(BUILD_DIR)/format-smoke
	./$(BUILD_DIR)/portable-smoke

clean:
	rm -rf "$(BUILD_DIR)"
