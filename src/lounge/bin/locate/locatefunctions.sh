#!/bin/bash

function prepareTerms() {
  echo "$@" | sed 's/%20/ /g' | sed 's/ /\*/g'
}

function makeLocateArgs() {
  terms="$1"
  shift;
  echo -en "-d '/lounge/movie.db:/lounge/image.db' -i ";
  echo -en " \"*$terms*\" "
}
