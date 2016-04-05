#!/bin/bash

function run_test()
{
    PROG=$1
    SFILE=`echo "$1" | grep 'S$' > /dev/null && echo 1 || echo 0`
    if [ "$SFILE" == "1" ]; then echo -n -e '\033[34m'; else echo -n -e '\033[33m'; fi
    echo -n -e "$PROG\033[0m - "
    if gcc -O0 -g -pipe -m32 -fno-builtin -Wall -D REALMACHINE -D "HAVEASM=$SFILE" -o testprog -I lib-common "$PROG" testcase/realtest.c; then
        echo -n -e '\033[32mCOMPILE OK\033[0m - '
        if ! ./testprog; then
            echo -e '\033[1;31mRUN FAILED\033[0m'
        else
            rm -f ./testprog
        fi
    else
        echo -e '\033[1;31mCOMPILE FAILED\033[0m'
    fi
}

if [ -z "$1" ]; then
    for i in `find testcase/src -name '*.[cS]' | sort`; do
        run_test $i
    done
else
    run_test testcase/src/$1
fi
