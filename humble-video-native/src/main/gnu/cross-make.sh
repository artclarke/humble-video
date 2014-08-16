#*******************************************************************************
# Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
#   
# This file is part of Humble-Video.
#
# Humble-Video is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Humble-Video is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
#*******************************************************************************
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
    for dir in *mingw*; do
      (cd $dir && make $*)
      if [ $? -ne 0 ]; then
        exit -1;
      fi
    done
  ;;
esac
