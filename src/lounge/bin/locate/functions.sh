#!/bin/bash

export PATH="$PATH:/lounge/bin/locate/"
function prepareTerms() {
  echo "$@" | sed 's/%20/ /g' | sed 's/ /\*/g'
}

function makeLocateArgs() {
  terms="$1"
  shift;
  echo -en " -i ";
  echo -en " \"*$terms*\" "
}
