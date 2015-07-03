#!/bin/bash

bad=$(find $1 -size -2k)
if [[ -n "$bad" ]]; then
    echo $bad | xargs rm -v | wc -l
fi

hadd -f $1/toys.root $1/toy_*.root
