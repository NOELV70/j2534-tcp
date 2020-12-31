CC=clang
MAKE=make
CFLAGS=-Isrc/ -Wall

ifeq ($(OS),Windows_NT)
	TARGET = j2534-tcp.dll
else
	TARGET = j2534-tcp.so
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
	$(CC) $(CFLAGS) src/*.c -shared -o build/$(TARGET)
