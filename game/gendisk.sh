#!/bin/bash

echo "==== FILE LIST BEGIN ====" >&2
ls -l $@ >&2
echo "==== FILE LIST END ====" >&2


# make filelist
for i in $@; do
    basename $i | awk '{printf("%-16s", $1)}'
    ls -l $i | awk '{printf("%-16s", $5)}'
done
echo 'END_OF_LIST' | awk '{printf("%-16s", $1)}'

# concat data
cat $@
