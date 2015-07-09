#!/bin/bash

set -x 

folder=$1

cd $folder

echo datacard*.txt | xargs -n 1 text2workspace.py -L libdiphotonsUtils 
