#!/bin/bash

. ./.functions.sh

HASH=
DEBUG=

function prepare() {
  [ -z "$HASH" ] && echo
  janosh truncate
}

function finalize() {
  [ -n "$VERBOSE" ] && janosh dump
  echo -n "$1:"
  janosh hash
}

function test_truncate() {
  true
}

function test_mkarray() { 
  janosh mkarr /array/.        || return 1
  janosh mkarr /array/.        && return 1 
  janosh set /bla blu          || return 1
  janosh mkarr /bla/.          && return 1 || return 0
}

function test_mkobject() {
  janosh mkobj /object/.       || return 1
  janosh mkobj /object/.       && return 1
  janosh set /bla blu          || return 1
  janosh mkobj /bla/.          && return 1 || return 0
}

function test_append() {
  janosh mkarr /array/.             || return 1
  janosh append /array/. 0 1 2 3	  || return 1
  [ `janosh size /array/.` -eq 4 ]	|| return 1
  janosh mkobj /object/.		        || return 1
  janosh append /object/. 0 1 2 3 	&& return 1
  [ `janosh size /object/.` -eq 0 ] || return 1  
}

function test_set() {
  janosh mkarr /array/. 		        || return 1
  janosh append /array/. 0 		      || return 1
  janosh set /array/#0 1 		        || return 1
  janosh set /array/#6 0			        && return 1
  [ `janosh size /array/.` -eq 1 ]  || return 1
  janosh mkobj /object/. 	 	        || return 1
  janosh set /object/bla 1		      || return 1
  [ `janosh size /object/.` -eq 1 ] || return 1
}

function test_add() {
  janosh mkarr /array/.             || return 1
  janosh add /array/#0 1             || return 1
  janosh add /array/#0 0             && return 1
  janosh add /array/#5 0							&& return 1
  [ `janosh size /array/.` -eq 1 ]  || return 1
  janosh mkobj /object/.						|| return 1
  janosh add /object/0 1						|| return 1
  janosh add /object/0 0						&& return 1
  janosh add /object/5 0						|| return 1
  [ `janosh size /object/.` -eq 2 ]	|| return 1
}

function test_remove() {
  janosh mkarr /array/.             || return 1
  janosh add /array/#0 1            || return 1
  janosh add /array/#0 0            && return 1
  janosh append /array/. 2 3 4      || return 1
  janosh remove /array/#1           || return 1
  [ `janosh -r get /array/#1` -eq 3 ]  || return 1
  janosh add /array/#5 0            && return 1
  return 0
  janosh remove /array/*            || return 1
  janosh mkarr /array/#1/.          || return 1
  [ `janosh size /array/.` -eq 0 ]  || return 1
  janosh mkobj /object/.            || return 1
  janosh add /object/0 1            || return 1
  janosh add /object/0 0            && return 1
  janosh add /object/5 0            || return 1
  janosh mkarr /object/subarr/.     || return 1
  janosh remove /object/.           || return 1
  janosh size /object/.             && return 1 || return 0
}

function test_replace() {
  janosh mkarr /array/.             || return 1
  janosh append /array/. 0 		      || return 1
  janosh replace /array/#0 2 		    || return 1
  janosh replace /array/#6 0		      && return 1
  [ `janosh size /array/.` -eq 1 ] 	|| return 1
  janosh mkobj /object/. 	 	        || return 1
  janosh set /object/bla 1		      || return 1
  janosh replace /object/bla 2		  || return 1
  janosh replace /object/blu 0		  && return 1
  [ `janosh size /object/.` -eq 1 ] || return 1
}


function test_copy() {
  janosh mkobj /object/.                      || return 1
  janosh mkobj /target/.                      || return 1
  janosh mkarr /object/array/.                || return 1
  janosh append /object/array/. 0 1 2 3       || return 1
  janosh copy /object/array/. /target/array/. || return 1
  janosh copy /object/array/. /target/.       && return 1  
  janosh copy /object/. /object/array/.       && return 1
  janosh copy /target/. /object/array/.       && return 1
  [ `janosh size /target/array/.` -eq 4 ] 	  || return 1
  [ `janosh size /object/array/.` -eq 4 ]     || return 1
  [ `janosh -r get /target/array/#0` -eq 0 ]  || return 1
  [ `janosh -r get /object/array/#0` -eq 0 ]  || return 1
}

function test_shift() {
  janosh mkarr /array/.                 || return 1
  janosh append /array/. 0 1 2 3        || return 1
  janosh shift /array/#0 /array/#3        || return 1
  [ `janosh size /array/.` -eq 4 ]      || return 1
  [ `janosh -r get /array/#0` -eq 1 ]    || return 1
  [ `janosh -r get /array/#3` -eq 0 ]    || return 1
  janosh mkarr /array/#4/.               || return 1
  janosh append /array/#4/. 3 2 1 0      || return 1
  janosh shift /array/#4/#3 /array/#4/#0    || return 1
  [ `janosh -r get /array/#4/#0` -eq 0  ] || return 1  
  [ `janosh -r get /array/#4/#3` -eq 1  ] || return 1
  janosh append /array/. 4 5 6 7        || return 1
  janosh mkarr /array/#9/.               || return 1
  janosh append /array/#9/. 7 6 5 4      || return 1
  janosh shift /array/#4/. /array/#9/.    || return 1
  [ `janosh size /array/#8/.` -eq 4 ]    || return 1
  [ `janosh size /array/#9/.` -eq 4 ]    || return 1
  [ `janosh -r get /array/#8/#0` -eq 7  ] || return 1
}

function run() {
  ( 
    prepare
    [ -n "$DEBUG" ] && set -x
    [ -n "$DEBUG" ] && test_$1 || (test_$1 &>/dev/null)
    err=$?
    [ -n "$DEBUG" ] && set +x
    [ -z "$HASH" ] && check "$1" "[ $err -eq 0 ]" || (check "$1" "[ $err -eq 0 ]" &> /dev/null)
  ) 2>&1 | fgrep -v "INFO:"

  finalize $1 2>&1 | fgrep -v "INFO:"
}


while getopts 'dhv' c
do
  case $c in
    d) DEBUG=YES;;
    v) VERBOSE=YES;;
    h) HASH=YES;;
    \?) echo "Unkown switch"; exit 1;;
  esac
done

shift

if [ -z "$1" ]; then
  run truncate
  run mkarray
  run mkobject
  run append
  run set
  run add
  run remove
  run replace
  run copy
  run shift
else
  run $1
fi


