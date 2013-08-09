#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
HOST=$( ${DIR}/mk/config.guess )

# assume we're running in a directory with lots of different sub-directories.
case $HOST in
  *darwin*)
    for dir in *-apple-darwin*; do
      echo "Building in $dir; $PWD"
      (cd $dir && make $*)
      if [ $? -ne 0 ]; then
        exit -1;
      fi
    done
  ;;
  *linux*)
    for dir in *-linux-*; do
      (cd $dir && make $*)
      if [ $? -ne 0 ]; then
        exit -1;
      fi
    done
    for dir in *-mingw-*; do
      (cd $dir && make $*)
      if [ $? -ne 0 ]; then
        exit -1;
      fi
    done
  ;;
esac
