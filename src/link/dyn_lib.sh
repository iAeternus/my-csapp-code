#!/bin/bash
mkdir -p dyn_lib

gcc -fPIC -shared -o dyn_lib/libvector.so addvec.c multvec.c
gcc -o dyn_lib/prog main2.c -ldl