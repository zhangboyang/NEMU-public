#!/bin/bash

function echo_program()
{
    echo "char input_buffer[] = "
    ./file2cstr < "$INPUT"
    echo ";"
    echo
    echo "char answer_buffer[] = "
    ./file2cstr < "$OUTPUT"
    echo ";"
    echo
    cat testcase/naive-stdio-template.c
    cat $PROG
}

PROG="$1"
INPUT="$2"
OUTPUT="$INPUT.out"
OUTPUTPROG="$3"

if [ -z "$PROG" -o -z "$INPUT" -o -z "$OUTPUTPROG" ]; then
    echo "usage: $0 PROGRAM INPUTFILE OUTPUTPROGRAM"
    exit 1
fi

gcc -Wall -o testprog_original "$PROG"
gcc -Wall -o file2cstr testcase/file2cstr.c

./testprog_original < "$INPUT" > "$OUTPUT"

echo -e "\033[1;32mINPUT:\033[0m"
cat "$INPUT"
echo -e "\033[1;32mOUTPUT:\033[0m"
cat "$OUTPUT"

echo_program | sed '/stdio\.h/d' > "$OUTPUTPROG"

rm -f testprog_original file2cstr "$OUTPUT"


cp "$OUTPUTPROG" "testcase/src/$OUTPUTPROG"
./realtest.sh "$OUTPUTPROG"

