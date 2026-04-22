#!/bin/bash
mkdir -p static_lib

gcc -c addvec.c -o static_lib/addvec.o
gcc -c multvec.c -o static_lib/multvec.o

ar rcs static_lib/libvector.a static_lib/addvec.o static_lib/multvec.o

gcc -c main.c -o static_lib/main.o

gcc -static -o static_lib/prog static_lib/main.o static_lib/libvector.a
