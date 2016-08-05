#!/bin/bash

set -x 

folder=$1 && shift

cd $folder

## sed 's%^# bk%bk%' -i datacard*.txt

libs="-L libdiphotonsUtils"
rootversion=$(root-config --version| tr '.' ' ' | awk '{print  $1}')
[[ $rootversion -gt 5 ]] && libs="-L libdiphotonsRooUtils"

## echo datacard*.txt | xargs -n 1 text2workspace.py $libs $@
for card in  datacard*.txt; do
    binary=$(echo $card | sed 's%txt%root%' )
    if [[ ! -f $binary ]]; then ## || [[ ! $binary -nt $card ]]; then
	text2workspace.py $libs $card
    fi
done
