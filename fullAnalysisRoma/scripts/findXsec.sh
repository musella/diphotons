#!/bin/bash


TEMP=`getopt -o d: --long dataset: -n 'findXsec.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Options are wrong..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

while true; do

  case "$1" in
    -d | --dataset ) datasetfile=$2; shift 2 ;;
    -- ) shift; break ;;
    * ) break ;;
  esac
done
[ -e ana.py ] || curl https://raw.githubusercontent.com/syuvivida/generator/master/cross_section/runJob/ana.py  -o ana.py

for dataset in $(cat datasets.txt); do
    echo "=====> $dataset"
    datatier=`echo $dataset | awk -F '/' '{print $4}'`
#    echo $datatier
    if [ "${datatier}" != "AODSIM" ]; then
	echo "You should run this on AODSIM"
	continue;
    fi
    aodfile=`das_client.py --limit=1 --query='file dataset='$dataset'' | sed '1,3d'`
    cmsRun ana.py inputFiles="${aodfile}" maxEvents=-1 2>&1 | grep "After" | awk -F '= ' '{print $2}'
#    cmsRun ana.py inputFiles="${aodfile}" maxEvents=-1 2>&1 
done
