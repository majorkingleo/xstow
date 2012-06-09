#!/bin/bash
#
# The script tries guessing if an application can be installed by using
# 	make install prefix=/usr/local/stow/package-1.0
# or
#	make install DESTDIR=/stow/package-1.0
#
# This script belongs to the xstow package
#
# (C) 2004 by Martin Oberzalek <kingleo@gmx.at> under the terms of the GPL
#

index_p=0;
index_m=0;
index_dm=0;
index_dp=0;
index_ap=0;
index_am=0;

function msg() {
  echo "$1 => $2"
  if test $2 = '+' ; then
    index_p=$((index_p+1))
  else
    index_m=$((index_m+1))
  fi
}

function amsg() {
   echo "$1 => $2"

  if test $2 = '+' ; then
    index_ap=$((index_ap+1))
  else
    index_am=$((index_am+1))
  fi
}

function dmsg() {
  echo "$1 => $2"

  if test $# -gt 2 ; then
  if test $3 = '+' ; then
    index_dp=$((index_dp+1))
  else
    index_dm=$((index_dm+1))
  fi  
  fi 

  if test $2 = '+' ; then
    index_p=$((index_p+1))
  elif test $2 = '0' ; then
    return 0;
  else
    index_m=$((index_m+1))
  fi
}

function error() {
  echo "*ERROR*:" $1
  exit 1
}

function report_result(){
  hints_all=$((index_p+index_m))
  hints_dall=$((index_dp+index_dm))

  if test $hints_all = '0' ; then
     hints_all=1
  fi

  if test $hints_dall = '0' ; then
     hints_dall=1
  fi 

  inst_dest=$(( (index_dp-index_dm) / hints_dall ))
  inst_pref=$(( (index_p-index_m) / hints_all ))

  inst_dest_vote="bad"
  inst_pref_vote="bad"

  if test $inst_dest -gt 0 ; then 
    inst_dest_vote="good"
  fi

  if test $inst_pref -gt 0 ; then 
    inst_pref_vote="good"
  fi

  echo "positive Hints: $index_ap"
  echo "negative Hints: $index_am"
  echo "positive prefix Hints: $index_p"
  echo "negative prefix Hints: $index_m"
  echo "positive DESTDIR hints: $index_dp"
  echo "negative DESTDIR hints: $index_dm"
  echo "installable by using DESTDIR: $inst_dest => $inst_dest_vote" 
  echo "installable by using prefix: $inst_pref => $inst_pref_vote" 
  echo 
  echo "!!! WARNING !!! This script guesses if the package is installable by using"
  echo "   'make install prefix=/usr/local/stow/package-1.0'"
  echo "or"
  echo "   'make install DESTDIR=/stow/package-1.0'"
  echo "There is no warranty that the installation will work as expected"
}

function check_autoconf_script() {
    ERG=$(head $1 | grep -i Autoconf)
    if ! test -z "$ERG"; then
       return 0
    fi
    return 1
}

function check_makefile_in() {
  echo "checking $1"

  ERG=$( grep "^install" "$1")
  if test -z "$ERG" ; then
    return 0
  fi

  prefix="false"

  ERG=$( grep "prefix" "$1" )
  if ! test -z "$ERG" ; then
    prefix="true"
  fi

  destdir="false"

  ERG=$( grep "DESTDIR" "$1" )
  if ! test -z "$ERG" ; then
    destdir="true"
  fi

  edestdir="false"

  if test $destdir = "true" ; then

    ERG=$( grep -E '\$\(DESTDIR\)\$\(' "$1" )
    if ! test -z "$ERG" ; then
      edestdir="true"
    fi

  fi

  if test $prefix = "true" && test $destdir = "true" && test $edestdir = "true" ; then
    msg '  prefix found' '+'
    dmsg '  $(DESTDIR)$( found' "+" "+"
    return 0
  fi

  if ! test $destdir = "true" || ! test $prefix = "true" ; then

    # does the directory contains installable files?
    DIR=$(echo $1 | sed 's/\.in$//' | sed 's/Makefile$//')
    FILES=$(\ls $DIR*.cpp $DIR*.CPP $DIR*.c $DIR*.cc $DIR*.C $DIR*.h $DIR*.cxx 2>/dev/null)
    if ! test -z "$FILES" ; then

      if ! test $prefix = "true" && ! test $destdir = "true" ; then
         msg '  file contains install code, but no prefix or DESTDIR' '-' '-'
      elif ! test $destdir = "true" ; then
        dmsg '  DESTDIR not found but required' '0' '-'
      elif ! test $prefix = "true" ; then
        msg '  prefix not found but required' '-'
      fi

    fi
  fi

  if test $destdir = "true" && ! test $edestdir = "true" ; then
    dmsg '  DESTDIR found but no $(DESTDIR)$(' '0' '-'
  elif test $destdir = "true" && test $edestdir = "true" ; then
    dmsg '  $(DESTDIR)$( found' '0' '+'
  fi

  if ! test $prefix = "true" && test $destdir = "true" ; then
    dmsg '  no prefix but DESTDIR found' '-' '+'
  fi

  if test $prefix = "true" ; then
   msg '  prefix found' '+'
  fi
  
  return 0
}

# has a configure script ?
if test -f configure ; then 
  amsg "configure script found" "+"
  if check_autoconf_script configure ; then
    amsg "created by autoconf" "+"
  else
    amsg "not created by autoconf" "-"
  fi
fi

if test -f Makefile.am ; then
  amsg "found automake Makefile" "+"
fi

if test -f Makefile.in ; then
  amsg "found Makfile.in" "+"

  for i in $(find -name "Makefile.in") ; do
     check_makefile_in "$i" 
  done
fi

if test -f Imakefile ; then
  amsg "found Imakefile" "+"

  for i in $(find -name "Makefile") ; do
     check_makefile_in "$i"
  done  
fi

report_result
