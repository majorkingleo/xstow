#!/usr/bin/env bash

source functions.sh
export XSTOW_DONT_READ_SYSCONF_INI=0

for i in t* ; do 
    if test -d $i ; then 
        echo 
        echo "testing $i" 
        ( cd work && rm -rf * )
        cp -a $i/* work
        ( cd work && ./test.sh $i )
    fi
done