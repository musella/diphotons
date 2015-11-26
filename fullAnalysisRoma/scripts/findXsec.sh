#!/bin/bash


TEMP=`getopt -o d: --long debug,dataset: -n 'findXsec.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Options are wrong..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

debug=0

while true; do

  case "$1" in
    -d | --dataset ) datasetfile=$2; shift 2 ;;
    --debug ) debug=1; shift 2 ;;
    -- ) shift; break ;;
    * ) break ;;
  esac
done

#Check grid proxy
voms-proxy-info -e 2>&1 > /dev/null
PROXYEXIST=$?

if [ ${PROXYEXIST} == 1 ]; then
    echo "You need a valid grid proxy. Please run voms-proxy-init"
    exit -1
fi

[ -e ana.py ] || curl https://raw.githubusercontent.com/syuvivida/generator/master/cross_section/runJob/ana.py  -o ana.py
[ ${debug} == 1 ] && echo "==> DEBUG MODE <=="

for dataset in $(cat datasets.txt); do
    echo "=====> $dataset"
    datatier=`echo $dataset | awk -F '/' '{print $4}'`
    friendly_name=$(whoami)/`echo $dataset | sed -e 's%/%_%g'`

    if [ "${datatier}" != "AODSIM" ] && [ "${datatier}" != "USER" ]; then
	echo "You should run this on AODSIM"
	continue;
    fi

    aodfile=`das_client.py --limit=1 --query='file dataset='$dataset' instance=prod/phys03' | sed '1,3d'`

    [ -e /tmp/${friendly_name}_xsec.log ] && rm -rf /tmp/${friendly_name}_xsec.log

    if [ ${debug} == 0 ]; then
	cmsRun ana.py inputFiles="${aodfile}" maxEvents=-1 > /tmp/${friendly_name}_xsec.log 2>&1 
    else
	cmsRun ana.py inputFiles="${aodfile}" maxEvents=-1 2>&1 | tee /tmp/${friendly_name}_xsec.log  
    fi

    [ ${debug} == 1 ] && echo "XSEC==> " 
    cat /tmp/${friendly_name}_xsec.log | grep "After" | awk -F '= ' '{print $2}' | uniq
done
