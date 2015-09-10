#!/bin/sh
gcc -Wall -O0 -m32 -o datagen datagen.c \
&& gcc -Wall -O2 -o testprog test.c -I ../../nemu -I ../../nemu/include \
&& (./datagen | ./testprog) \
&& echo 'TEST OK' || echo 'TEST FAILED'

rm -f testprog datagen
