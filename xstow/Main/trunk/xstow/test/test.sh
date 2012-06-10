#!/usr/bin/env bash

source functions.sh
export XSTOW_DONT_READ_SYSCONF_INI=0

if ! test -d work ; then
    exec_or_die mkdir work
fi

if test $# -gt 0 ; then
    TESTONLY=$1
    export DEBUG=1
fi

for i in t* ; do 
    if test -d $i ; then 

        if test "${TESTONLY}xx" != "xx" ; then
            if test $i != "${TESTONLY}" ; then
                continue;
            fi
        fi

        echo 
        echo "testing $i" 
        ( cd work && rm -rf * )
        cp -a $i/* work && find work -name ".svn" | xargs rm -rf
        ( cd work && ./test.sh $i )
    fi
done