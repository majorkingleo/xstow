#!/usr/bin/env bash

source ../functions.sh

XSTOW=../../../src/xstow

exec_or_die mkdir -p usr/local
exec_or_die cd stow   
echo "[keep-dirs]" > xstow.ini
echo "  dir=`pwd`/../usr/local" >> xstow.ini

exec_or_die $XSTOW a
exec_or_die $XSTOW b
exec_or_die $XSTOW -D a
exec_or_die $XSTOW -D b
exec_or_die cd ..
exec_or_die find usr > log

diff_and_log $1 log RESULT


