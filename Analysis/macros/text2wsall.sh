#!/bin/bash

set -x 

folder=$1

cd $folder

sed 's%^# bk%bk%' -i datacard*.txt

libs="-L libdiphotonsUtils"
rootversion=$(root-config --version| tr '.' ' ')
[[ $rootversion -gt 5 ]] && libs="-L libdiphotonsRooUtils"

echo datacard*.txt | xargs -n 1 text2workspace.py $libs
