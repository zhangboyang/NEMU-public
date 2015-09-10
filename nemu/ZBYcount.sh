#!/bin/bash
target=.
date=`date '+%F %T %s'`


all=0
for i in `find $target -name '*.[ch]'`; do
#    cur=`cat "$i" | wc -l`
    cur=`cat "$i" | sed '/^\s*$/d' | wc -l`
#    echo "$i -> $cur"
    all=$[$all+$cur]
done

echo -ne '\x1b[1m'
echo "line count: $all"
echo -ne '\x1b[0m'

echo "$date $all" >> ZBYlclog.txt

