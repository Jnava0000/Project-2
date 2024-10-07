SRC = $(shell pwd)
DEP = $(SRC)/dep_root
STRIP = strip
CC ?= cc
CFLAGS += -isystem $(DEP)/include -I$(SRC)/include -I$(SRC) -D_XOPEN_SOURCE=500
CFLAGS += -Wall -Wextra -Wno-unused-parameter -DPALERAIN_VERSION=\"2.0.2\" -DHAVE_LIBIMOBILEDEVICE
CFLAGS += -Wno-unused-variable -I$(SRC)/src -std=c99 -pedantic-errors -D_C99_SOURCE -D_POSIX_C_SOURCE=200112L
LIBS += $(DEP)/lib/libimobiledevice-1.0.a $(DEP)/lib/libirecovery-1.0.a $(DEP)/lib/libusbmuxd-2.0.a
LIBS += $(DEP)/lib/libimobiledevice-glue-1.0.a $(DEP)/lib/libplist-2.0.a -pthread -lm
ifeq ($(TARGET_OS),)
TARGET_OS = $(shell uname -s)
UNAME = $(TARGET_OS)
else
UNAME = $(shell uname -s)
endif
ifeq ($(TARGET_OS),Darwin)
CFLAGS += -Wno-nullability-extension
ifeq (,$(findstring version-min=, $(CFLAGS)))
CFLAGS += -mmacosx-version-min=10.8
endif
LDFLAGS += -Wl,-dead_strip
LIBS += -framework CoreFoundation -framework IOKit
else
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,--gc-sections
endif
LIBS += $(DEP)/lib/libmbedtls.a $(DEP)/lib/libmbedcrypto.a $(DEP)/lib/libmbedx509.a $(DEP)/lib/libreadline.a

ifeq ($(TUI),1)
ifeq ($(TARGET_OS),Linux)
LIBS += $(DEP)/lib/libgpm.a
endif
endif

ifeq ($(DEV_BUILD),1)
CFLAGS += -O0 -g -DDEV_BUILD -fno-omit-frame-pointer
ifeq ($(ASAN),1)
BUILD_STYLE=ASAN
CFLAGS += -fsanitize=address,undefined -fsanitize-address-use-after-return=runtime
else ifeq ($(TSAN),1)
BUILD_STYLE=TSAN
CFLAGS += -fsanitize=thread,undefined
else
BUILD_STYLE = DEVELOPMENT
endif
else
CFLAGS += -Os -g
BUILD_STYLE = RELEASE
endif
LIBS += -lc

ifeq ($(TARGET_OS),Linux)
ifneq ($(shell echo '$(BUILD_STYLE)' | grep -q '[A-Z]\+SAN' && echo 1),1)
LDFLAGS += -static -no-pie
endif
endif

ifneq ($(BAKERAIN_DEVELOPE_R),)
CFLAGS += -DBAKERAIN_DEVELOPE_R="\"$(BAKERAIN_DEVELOPE_R)\""
endif

BUILD_NUMBER := $(shell git rev-list --count HEAD)
BUILD_TAG := $(shell git describe --dirty --tags --abbrev=7)
BUILD_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
BUILD_COMMIT := $(shell git rev-parse HEAD)

CFLAGS += -DBUILD_STYLE="\"$(BUILD_STYLE)\"" -DBUILD_TAG="\"$(BUILD_TAG)\""
CFLAGS += -DBUILD_NUMBER="\"$(BUILD_NUMBER)\"" -DBUILD_BRANCH="\"$(BUILD_BRANCH)\""
CFLAGS += -DBUILD_COMMIT="\"$(BUILD_COMMIT)\""

CPATH =
LIBRARY_PATH =

export SRC DEP UNAME CC CFLAGS LDFLAGS LIBS SHELL TARGET_OS DEV_BUILD BUILD_DATE BUILD_TAG BUILD_WHOAMI BUILD_STYLE BUILD_NUMBER BUILD_BRANCH

all: palera1n

palera1n: download-deps
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean
	$(MAKE) -C docs clean

download-deps:
	$(MAKE) -C src $(patsubst %, resources/%, checkra1n-macos checkra1n-linux-arm64 checkra1n-linux-armel checkra1n-linux-x86 checkra1n-linux-x86_64 checkra1n-kpf-pongo ramdisk.dmg binpack.dmg Pongo.bin)

docs:
	$(MAKE) -C docs

distclean: clean
	$(MAKE) -C src distclean

.PHONY: all palera1n clean docs distclean

