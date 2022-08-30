#!/usr/bin/env bash

source ../functions.sh

XSTOW=../../../src/xstow

exec_or_die mkdir -p usr/local
exec_or_die cd stow   
exec_or_die $XSTOW a --copy root
exec_or_die $XSTOW b --copy root
exec_or_die cd ..
exec_or_die find usr root > log

diff_and_log $1 log RESULT


