#!/usr/bin/env bash

source ../functions.sh

XSTOW=../../../src/xstow

exec_or_die mkdir -p usr/local
exec_or_die cd stow   
exec_or_die $XSTOW a
exec_or_die $XSTOW b
exec_or_die $XSTOW --keep-dirs `pwd`/../usr/local -D a
exec_or_die $XSTOW --keep-dirs `pwd`/../usr/local -D b
exec_or_die cd ..
exec_or_die find usr > log

diff_and_log $1 log RESULT


