export DISPLAY=""
mydir="/afs/cern.ch/work/k/khoumani/CMSSW_7_1_5/src/diphotons/Analysis/macros"

set -x
cd $mydir

eval `scramv1 runtime -sh`

lum=$1
kappa=$2
mass=$3

#for i in `seq 1 9`;
#do
#	combine -M Asymptotic --run expected datacard_parametricSignal.txt -n _grav_${kappa}_${mass}_lum_${lum}_ -m $mass --setPhysicsModelParameters kpl=${kappa},lumi=${lum} -v 3
#	 let "mass+=500"
#done


#for i in `seq 1 41`;
#do

	combine -M Asymptotic --run expected datacards_dijet_cic2_lum_${lum}/datacard_bkg_bias_300_grav_${kappa}_${mass}.txt -n _grav_${kappa}_${mass}_lum_${lum}_Parametric_Bias_new -m $mass -v 3 --rMin $4 --rMax $5
#	let "mass+=100"
#done

#hadd -f added_higgsCombine_grav_${kappa}_lum_${lum}_Parametric_Bias_new.Asymptotic.root higgsCombine_grav_${kappa}_*_lum_${lum}_Parametric_Bias_new.Asymptotic.mH*.root

#rm higgsCombine_grav_01_*_lum_10_Parametric.Asymptotic.mH*.root 
#
#combine -M Asymptotic --run expected datacards_cic2_lum_5/datacard_bkg_bias_300_grav_02_1500.txt -n _grav_02_1500_lum_5_ -m 1500 -v 3
#
##combine -M Asymptotic --run expected datacards_cic2_lum_10/datacard_bkg_bias_300_grav_02_5000.txt -n _grav_02_5000_lum_10_ -m 5000 -v 3
#combine -M Asymptotic --run expected datacards_cic2_lum_5/datacard_bkg_bias_300_grav_02_5000.txt -n _grav_02_5000_lum_5_ -m 5000 -v 3
#
#combine -M Asymptotic --run expected datacards_cic2_lum_10/datacard_bkg_bias_300_grav_02_3000.txt -n _grav_02_3000_lum_10_ -m 3000 -v 3
#combine -M Asymptotic --run expected datacards_cic2_lum_5/datacard_bkg_bias_300_grav_02_3000.txt -n _grav_02_3000_lum_5_ -m 3000 -v 3
#
##combine -M Asymptotic --run expected datacards_cic2_lum_10/datacard_bkg_bias_300_grav_001_750.txt -n _grav_001_750_lum_10_ -m 750 -v 3
#combine -M Asymptotic --run expected datacards_cic2_lum_5/datacard_bkg_bias_300_grav_001_750.txt -n _grav_001_750_lum_5_ -m 750 -v 3
#
##combine -M Asymptotic --run expected datacards_cic2_lum_10/datacard_bkg_bias_300_grav_001_1500.txt -n _grav_001_1500_lum_10_ -m 1500 -v 3
#combine -M Asymptotic --run expected datacards_cic2_lum_5/datacard_bkg_bias_300_grav_001_1500.txt -n _grav_001_1500_lum_5_ -m 1500 -v 3
#
######combine -M Asymptotic --run expected datacards_cic2_lum_10/datacard_bkg_bias_300_grav_001_5000.txt -n _grav_001_5000_lum_10_ -m 5000 -v 3
#combine -M Asymptotic --run expected datacards_cic2_lum_5/datacard_bkg_bias_300_grav_001_5000.txt -n _grav_001_5000_lum_5_ -m 5000 -v 3