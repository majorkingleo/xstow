#!/usr/bin/env bash

function exec_or_die() 
{
    if test "${DEBUG}xx" != "xx" ; then
        echo $*
    fi

    $*
    if test $? != 0 ; then 
        echo "exec of $* FAILED"
        exit 1
    fi
}

function diff_and_log()
{
    res=`diff -u $2 $3 2>&1`

    if ! test -z "$res" ; then
    echo "DIFFER in $1"
        echo "   $res"
    else
        echo "  check $1 passed"        
    fi
}