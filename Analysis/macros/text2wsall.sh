#!/bin/bash

set -x 

folder=$1

cd $folder

sed 's%^# bk%bk%' -i datacard*.txt

echo datacard*.txt | xargs -n 1 text2workspace.py -L libdiphotonsRooUtils 
