#!/bin/sh
init_compile () {
  if [ ! -f ~/dockcross-windows-shared-x86 ]
  then
    docker run --rm dockcross/windows-shared-x86 > ~/dockcross-windows-shared-x86
    chmod +x ~/dockcross-windows-shared-x86
  fi
}

compile () {
  ~/dockcross-windows-shared-x86 bash -c 'rm -rf build && mkdir build && $CC -Isrc/ -Wall -shared -s -Wl,--kill-at -o build/j2534-tcp.dll src/j2534.c -lws2_32'
}

init_compile
compile
