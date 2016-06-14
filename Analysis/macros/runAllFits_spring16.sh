


## dir38=full_analysis_spring16v1_sync_v1_2705_cert
dir38=full_analysis_spring16v1_sync_v2_p34568_cert
lumi=2.07
## dir38=full_analysis_moriond16v1_sync_v4_vertex0_data

eosdir38=~/eos/cms/store/user/crovelli/WSdiphoton76x_v4
coups="001 01 02"


### common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --prepare-data --no-parametric-signal-nuisances --saveas pdf,root,convert_png --plot-blind 500,4000"
### 
### ./combine_maker.sh $dir38 --lumi $lumi --fit-name cic2  $common_opts --fit-background --mag-field 3.8T --load lumi.json --label bkg_fit


common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias --prepare-data --do-parametric-signal-nuisances --plot-blind 500,4000"

## 38T workspaces
parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi $lumi --fit-name cic2 $common_opts --parametric-signal $eosdir38/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir38/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76.json --load lumi.json --only-coups {}   --label spin2_preapp --do-parametric-signal-nuisances" ::: $coups &

parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi $lumi --fit-name cic2 $common_opts --parametric-signal $eosdir38/signalModel76x_{}_500to998__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_500to998__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_500to998__resolv4_smearDown.root --parametric-signal $eosdir38/signalModel76x_{}_1000to*__resolv4.root  --parametric-signal ${eosdir38}_smearUp/signalModel76x_{}_1000to*__resolv4_smearUp.root --parametric-signal ${eosdir38}_smearDown/signalModel76x_{}_1000to*__resolv4_smearDown.root  --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_preapp --do-parametric-signal-nuisances" ::: $coups &


wait
