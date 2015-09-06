# In this script there are a list of instructions to be run for the different steps: 
# Generate signal data sets, compute the signal FWHM, generate background+bias workspace, create datacards

export DISPLAY=""
mydir="/afs/cern.ch/work/k/khoumani/CMSSW_7_1_5/src/diphotons/Analysis/macros"

set -x
cd $mydir

eval `scramv1 runtime -sh`
##equivalent to cmsenv

minMass=300
#mass=1000
kappa=$2

lum=_lum_$1

#workspace_bkg=./ws_cic2_dijet${lum}
workspace_bkg=./workspace_cic2_dijet${lum}
workspace_sig=./workspace_cic2_signals${lum}
workspace_bkgnbias=./ws_cic2_dijet_bkgnbias${lum}
datacards_folder=./datacards_dijet_cic2${lum}

mkdir -p $workspace_sig
mkdir -p $workspace_bkgnbias
mkdir -p $workspace_bkg
#mkdir -p $datacards_folder
#mkdir ./sigoutput_xsec_5_cic2_dijet/

comp_bkg=bkg
#comp=pp_

#for kappa in 001 005 01 015 02
#do
mass=$4
#for i in `seq 1 41`;

#do
    let "bins=6000-minMass"
	signame=grav_${kappa}_${mass}
	
##-----------------------------------------------------------------------------------------
## GENERATE SIGNALS AND/OR FWHM
##-----------------------------------------------------------------------------------------
	## FOR COMPUTING FWHM FROM ROOT FILES, One root file for each signal
	./combine_maker.py --compute-fwhm-for-input-signal --read-ws workspace_cic2_signals${lum}/${minMass}_${signame}.root --signal-name  ${signame} -w $3 --load templates_maker.json --fit-name cic2
    ## For generating signal + compute FWHM (non parametric case)
   # ./combine_maker.py --generate-signal-dataset --observable mgg[$bins,$minMass,6000] --read-ws ./workspace_cic2_final/full_analysis_anv1_v18_final_ws_$minMass.root --fit-name cic2 --load templates_maker.json -o ./workspace_cic2_signals${lum}/${minMass} --compute-fwhm --set-bins-fwhm fwhm_bins.json --fwhm-output-file fwhm_cic2${lum}.json --luminosity 10
    
    ## For only one signal ## fine (non parametric case)
    #./combine_maker.py --generate-signal-dataset --observable mgg[$bins,$minMass,6000] --read-ws ./workspace_cic2_final/full_analysis_anv1_v18_final_ws_$minMass.root --fit-name cic2 --load templates_maker.json -o ./workspace_cic2_signals/${minMass}_${signame}.root --signal-name $signame

    ## For looping over all signals (non parametric case)
    #./combine_maker.py --generate-signal-dataset --observable mgg[$bins,$minMass,6000] --read-ws ./workspace_cic2_final/full_analysis_anv1_v18_final_ws_$minMass.root --fit-name cic2 --load templates_maker.json -o ./workspace_cic2_signals${lum}/${minMass} --luminosity 10


##-----------------------------------------------------------------------------------------
## DEFINE THE WORKSPACES
##-----------------------------------------------------------------------------------------
ws_bkg_fit=${workspace_bkg}/full_analysis_anv1_v18_${comp_bkg}_ws_$minMass.root
ws_sig=${workspace_sig}/${minMass}_${signame}.root
##-----------------------------------------------------------------------------------------


##-----------------------------------------------------------------------------------------
## Generate Workspace of bkg+bias with nuisance = 0 or nuisance != 0 for component: bkg or pp only
##-----------------------------------------------------------------------------------------

   #### generate workspace with bias but nuisance == 0 For pp replace in names bkg by pp >>>>>>>>>>>>>>>>>>>>>>>>>>
 #  ./combine_maker.py --generate-ws-bkgnbias --fit-name cic2 --observable mgg[$bins,$minMass,6000] --signal-name $signame --default-model dijet --read-ws $ws_bkg_fit --read-ws $ws_sig -o ${workspace_bkgnbias}/no_nuisance_${comp_bkg}_${minMass}_${signame}.root #--bkg-shapes ./bkg_model/pponly_shapes.json --data-source mctruth_pp

   #### generate workspace with bias with nuisance != 0 by including --fwhm-file and --bias-func >>>>>>>>>>>>>>>>>>>>>>>>>>
 #   ./combine_maker.py --generate-ws-bkgnbias --fit-name cic2 --observable mgg[$bins,$minMass,6000] --signal-name $signame --default-model dijet --read-ws $ws_bkg_fit --read-ws $ws_sig -o ${workspace_bkgnbias}/bias_${comp_bkg}_${minMass}_${signame}.root --fwhm-input-file ${2}/fwhm_cic2.json --bias-func ./bkg_model/bias_func_cic2.json --luminosity $1 #--bkg-shapes ./bkg_model/pponly_shapes.json --data-source mctruth_pp


##------------------------------------------------------------------------------------------
## GENERATING DATACARDS
##------------------------------------------------------------------------------------------

#>>>>>>>>>>>>>>>>>>>>>>>>>>
## generate datacard for signal+background 
#>>>>>>>>>>>>>>>>>>>>>>>>>>
# ./combine_maker.py --generate-datacard --read-ws $ws_bkg_fit --fit-name cic2 --load templates_maker.json --signal-name $signame --signal-root-file $ws_sig --cardname $datacards_folder/datacard_${comp_bkg}_${minMass}_${signame}.txt

#>>>>>>>>>>>>>>>>>>>>>>>>>>
## generate datacard for signal+(bkg+bias) with nuisances
###>>>>>>>>>>>>>>>>>>>>>>>>>>
# ./combine_maker.py --generate-datacard --read-ws ${workspace_bkgnbias}/bias_${comp_bkg}_${minMass}_${signame}.root --fit-name cic2 --load templates_maker.json --signal-name $signame --signal-root-file $ws_sig --cardname ${datacards_folder}/datacard_${comp_bkg}_bias_${minMass}_${signame}.txt


###########################################################################################################
##------------------------------------------------------------------------------------------
## Combine tool - compute significance by injecting a signal
##------------------------------------------------------------------------------------------
   # echo $signame >> ./sigoutput_xsec_5_cic2_dijet/sig3_$minMass.txt
    #combine -M ProfileLikelihood --expectSignal 0.0299 --significance -t -1 ./datacards_cic2/datacard_xsec_5_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_5_cic2_dijet/sig3_$minMass.txt
   # combine -M ProfileLikelihood --expectSignal 31 --rMin -10 --rMax 100 --significance -t -1 ./datacards_cic2/datacard_xsec_5_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_5_cic2_dijet/sig3_$minMass.txt
    
# ------------------------------------------------------------------------------------------------------------------------
# sig=sig3
# xsec=5_
 #  dir_sigoutput=./sigoutputFrozen_xsec_${xsec}cic2_dijet_${comp}asimov_kappa02
 #  mkdir -p $dir_sigoutput
 #  sigrate=0.662
 #
 #  echo $signame >> ${dir_sigoutput}/${sig}_kappa02_xsec_5.txt   #${sig}_$minMass.txt
 #  combine -M ProfileLikelihood  --significance -t -1  ./datacards_cic2/datacard_${comp}xsec_${xsec}bias_${minMass}_${signame}.txt  --expectSignal $sigrate --freezeNuisances dijet_model_bkg_EBEB_lin,dijet_model_bkg_EBEB_log,dijet_model_bkg_EBEE_lin,dijet_model_bkg_EBEE_log # | grep ^Significance | cut -d : -f2 >>  ${dir_sigoutput}/${sig}_kappa02_xsec_5.txt
    #
   #echo $signame >> ${dir_sigoutput}/${sig}_no_nuisance_$minMass.txt
   # combine -M ProfileLikelihood --expectSignal $sigrate --significance -t -1 --rMax 9000 --rMin -10 ./datacards_cic2/datacard_${comp}xsec_${xsec}no_nuisance_${minMass}_${signame}.txt  | grep ^Significance | cut -d : -f2 >> ${dir_sigoutput}/${sig}_no_nuisance_$minMass.txt
  ## 
   #echo $signame >> ${dir_sigoutput}/${sig}_bias_$minMass.txt
   #combine -M ProfileLikelihood --significance -t -1 ./datacards_cic2/datacard_${comp}xsec_${xsec}bias_${minMass}_${signame}.txt --expectSignal $sigrate | grep ^Significance | cut -d : -f2 >> ${dir_sigoutput}/${sig}_bias_$minMass.txt
#
   ##-----------------------------------------------------------------------------------------------------------------------
#     echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_$minMass.txt
#      combine -M ProfileLikelihood --expectSignal 32.73 --rMin -10 --rMax 100 --significance -t -1 ./datacards_cic2/datacard_xsec_10_bias_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt
   #
   # combine -M ProfileLikelihood --expectSignal 0.0375 --significance -t -1 ./datacards_cic2/datacard_xsec_10_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_$minMass.txt
   #
   # echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_no_nuisance_$minMass.txt
   # combine -M ProfileLikelihood --expectSignal 0.0375 --significance -t -1 ./datacards_cic2/datacard_xsec_10_no_nuisance_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_no_nuisance_$minMass.txt
   #
   #  echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt
   # combine -M ProfileLikelihood --expectSignal 0.0375 --significance -t -1 ./datacards_cic2/datacard_xsec_10_bias_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt

#eho $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_$minMass.txt
# combine -M ProfileLikelihood --expectSignal 0.743 --significance -t -1 ./datacards_cic2/datacard_xsec_10_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_$minMass.txt
#
# echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_no_nuisance_$minMass.txt
# combine -M ProfileLikelihood --expectSignal 0.743 --significance -t -1 ./datacards_cic2/datacard_xsec_10_no_nuisance_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_no_nuisance_$minMass.txt
#
#  echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt
# combine -M ProfileLikelihood --expectSignal 0.743 --significance -t -1 ./datacards_cic2/datacard_xsec_10_bias_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt


 #echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_$minMass.txt
 #combine -M ProfileLikelihood --expectSignal 32.73 --rMin -10 --rMax 100  --significance -t -1 ./datacards_cic2/datacard_xsec_10_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_$minMass.txt
 #
 #echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_no_nuisance_$minMass.txt
 #combine -M ProfileLikelihood --expectSignal 32.73 --rMin -10 --rMax 100  --significance -t -1 ./datacards_cic2/datacard_xsec_10_no_nuisance_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_no_nuisance_$minMass.txt
 #
 # echo $signame >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt
 #combine -M ProfileLikelihood --expectSignal 32.73 --rMin -10 --rMax 100  --significance -t -1 ./datacards_cic2/datacard_xsec_10_bias_${minMass}_${signame}.txt | grep ^Significance | cut -d : -f2 >> ./sigoutput_xsec_10_cic2_dijet/sig3_bias_$minMass.txt 
###########################################################################################################

    #let "minMass+=100"
 #let "mass+=100"
#done
#done





##########################################################################
## \mu-VALUES FOR EVIDENCE AND DISCOVERY POTENTIAL 
##########################################################################

##-------------------------------------------------------------------------------
##         Values to get 3 sigma
##-------------------------------------------------------------------------------
    ## for kappa = 0.2
    ## xsec_5: 0.02975, 0.662, 31.09 (--rMax 100 --rMin -10)
    ## xsec_10: 0.037502, 0.74202, 32.73 (--rMax 100 --rMin -10)

    ## for kappa = 0.01
    ## xsec_5: 0.51, 5.63, 7650 (--rMax 9000 --rMin -10 --minimizerTolerance 0.1)
    ## xsec_10: 0.685, 6.96, 8000.5 (--rMax 9000 --rMin -10)
##-------------------------------------------------------------------------------

##-------------------------------------------------------------------------------
##         Values to get 3 sigma for bias
##-------------------------------------------------------------------------------
    ## for kappa = 0.2
    ## xsec_5: 0.03, 0.665, 31.4 (--rMax 100 --rMin -10)>>>>>>> 0.006, 0.133, 6.25
    ## xsec_10: 0.0378, 0.745, 33.5 >>>>>>>> 0.0038, 0.075, 3.4

    ## for kappa = 0.01
    ## xsec_5: 0.5225, 5.62, 7700 ( --rMin -10 --rMax 9000 ) >>>>>>>> 0.105, 1.13, 1550
    ## xsec_10: 0.715, 6.96, 8300 ------------ >>>>>>>>> 0.072 ,0.7 , 840
##-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
##         Values to get 5 sigma for bias
##-------------------------------------------------------------------------------
    ## for kappa = 0.2
    ## xsec_5: 0.616, 1.662, 85.95 ( --rMax 500 --rMin -10 )>>>>>>> 0.0124, 0.33, 17.5
    ## xsec_10: 0.0755, 1.775, 87 >>>>>>> 0.0076, 0.178, 8.7

    ## for kappa = 0.01
    ## xsec_5: 0.969, 11.9, 21000 ( --rMin -10 --rMax 50000 ) >>>>>>>> 0.195, 2.4, 4193
    ## xsec_10: 1.3 ,  14.25, 21200 ----------- >>>>>>>>> 0.13 ,1.43 , 2150
##-------------------------------------------------------------------------------