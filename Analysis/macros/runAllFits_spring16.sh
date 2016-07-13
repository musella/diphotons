label=corrshape


### 2016 
### dir2016=full_analysis_spring16v1_sync_v4_cert_274443
### lumi2016=2.59

#### dir2016=full_analysis_spring16v1_sync_v4_topup_275125
#### lumi2016=1.4

## dir2016=full_analysis_spring16v1_sync_v5_topup_275125
## lumi2016=1.4

### dir2016=full_analysis_spring16v1_sync_v4_cert_275125
### lumi2016=3.99

## dir2016=full_analysis_spring16v1_sync_v5_cert_275125
## lumi2016=4.34

## dir2016=full_analysis_spring16v1_sync_v5_topup_276097
## lumi2016=3.3

dir2016=full_analysis_spring16v1_sync_v5_cert_276097
lumi2016=7.64

eosdir2016=~/eos/cms/store/user/crovelli/WSdiphoton76x_v4
coups="001"
## coups="001 01 02"
## coups="01 02"


### common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --prepare-data --no-parametric-signal-nuisances --saveas pdf,root,convert_png --plot-blind 13000,13100"
### 
### ./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016  $common_opts --fit-background --mag-field 3.8T --load lumi.json --label fit_unblind
### 
### exit 

### common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --prepare-data --no-parametric-signal-nuisances --saveas pdf,root,convert_png --plot-blind 500,13000"
### ./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016  $common_opts --fit-background --mag-field 3.8T --load lumi.json --label fit_blind
### 
### ## exit

## common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias --prepare-data --do-parametric-signal-nuisances --plot-blind 500,4000"

common_opts="--rescale-signal-to 1e-3 --compute-fwhm --generate-ws-bkgnbias --prepare-data --do-parametric-signal-nuisances --plot-blind 500,4000"

#### parallel --ungroup -j 1 "./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016 $common_opts --parametric-signal $eosdir2016/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir2016}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir2016}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir2016/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir2016}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir2016}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76.json --load lumi.json --only-coups {}   --label spin2_${label} --do-parametric-signal-nuisances --model-strip-coeff-names 016" ::: $coups &
#### ### 
#### parallel --ungroup -j 1 "./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016 $common_opts --parametric-signal $eosdir2016/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir2016}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir2016}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir2016/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir2016}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir2016}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_${label} --do-parametric-signal-nuisances --model-strip-coeff-names 016" ::: $coups &


parallel --ungroup -j 1 "./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016 $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes80X_ws_kMpl{}.root   --parametric-signal-acceptance  acceptance_76.json --load lumi.json --only-coups {}   --label spin2_${label} --do-parametric-signal-nuisances --model-strip-coeff-names 016" ::: $coups &

wait
exit

parallel --ungroup -j 1 "./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016 $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes80X_ws_kMpl{}.root --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_${label} --do-parametric-signal-nuisances --model-strip-coeff-names 016" ::: $coups &


wait
exit

## 2015 13TeV
dir38=full_analysis_moriond16v1_sync_v4_data
lumi2015=2.69
eosdir38=~/eos/cms/store/user/crovelli/WSdiphoton76x_v4

common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias --prepare-data --do-parametric-signal-nuisances"

## 38T workspaces
parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi $lumi2015 --fit-name cic2 $common_opts --parametric-signal $eosdir38/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir38/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76.json --load lumi.json --only-coups {}   --label spin2_${label} --do-parametric-signal-nuisances" ::: $coups &

parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi $lumi2015 --fit-name cic2 $common_opts --parametric-signal $eosdir38/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir38/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_${label} --do-parametric-signal-nuisances" ::: $coups &


wait
