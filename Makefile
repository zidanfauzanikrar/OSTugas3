CC = gcc
CFLAGS ?= -std=gnu11 -O0 -Wall -Wextra

SOURCES := main.c banner.c memstat.c malloc_test.c mmap_test.c varian8.c logger.c
PREVIEW_SOURCES := $(filter-out logger.c,$(SOURCES)) logger_stub.c
HEADERS := $(wildcard *.h)

STATIC_BIN := mem_static
DYNAMIC_BIN := mem_dynamic
PREVIEW_BIN := mem_preview

.PHONY: all static dynamic preview clean

all: static dynamic

static: $(STATIC_BIN)

dynamic: $(DYNAMIC_BIN)

# Untuk mencoba menu sebelum logger.c asli tersedia. Target ini tidak
# menghasilkan berkas log dan bukan pengganti build pengumpulan.
preview: $(PREVIEW_BIN)

$(STATIC_BIN): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -static $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

$(DYNAMIC_BIN): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

$(PREVIEW_BIN): $(PREVIEW_SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PREVIEW_SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(STATIC_BIN) $(DYNAMIC_BIN) $(PREVIEW_BIN)
