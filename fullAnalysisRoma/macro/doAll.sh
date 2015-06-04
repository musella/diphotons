#! /bin/sh
# this script merge files per specie, add the xsec weight and then merge everything together
# also the directory for plots is made

./mergeTrees.sh
./weightTrees.sh 1
./mergeSpecies.sh
rm -r diphotPlots
mkdir diphotPlots
