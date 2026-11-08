CC = gcc
CFLAGS ?= -std=gnu11 -O0 -Wall -Wextra

SOURCES := main.c banner.c memstat.c malloc_test.c mmap_test.c varian8.c logger.c
HEADERS := $(wildcard *.h)

STATIC_BIN := mem_static
DYNAMIC_BIN := mem_dynamic

.PHONY: all static dynamic clean

all: static dynamic

static: $(STATIC_BIN)

dynamic: $(DYNAMIC_BIN)

$(STATIC_BIN): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -static $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

$(DYNAMIC_BIN): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(STATIC_BIN) $(DYNAMIC_BIN)
