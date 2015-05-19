#!/bin/bash

find $1 -size -2k | xargs rm -v | wc -l
hadd -f $1/toys.root $1/toy_*.root
