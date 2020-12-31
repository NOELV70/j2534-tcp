CC=clang
MAKE=make
CFLAGS=-Isrc/ -Wall

ifeq ($(OS),Windows_NT)
    uname_S := Windows
else
    uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
    target = j2534-tcp.dll
else
    target = j2534-tcp.so
endif

all:
	$(MAKE) clean
	$(MAKE) lint
	$(MAKE) build

clean:
	rm -rf build

lint:
	clang-format -i src/*.c src/*.h

build:
	mkdir build
	$(CC) $(CFLAGS) src/*.c -shared -o build/$(target)
