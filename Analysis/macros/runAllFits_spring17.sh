label=parametric_v3

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


##dir2016=full_analysis_spring16v1_sync_v5_cert_276097
## lumi2016=7.64

dir2017=full_analysis_spring17v1_sync_v1
lumi2017=35.9

eosdir2016=~/eos/cms/store/user/crovelli/WSdiphoton76x_v4
## coups="001"
coups="001 01 02"
## coups="01 02"


common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --prepare-data --no-parametric-signal-nuisances --saveas pdf,root,convert_png --plot-blind 500,13100"
 
./combine_maker.sh $dir2017 --lumi $lumi2017 --lumistr $lumi2017 --fit-name cic2016  $common_opts --fit-background --load lumi.json --label fit_paper --load-merge lumi_paper.json 

## ./combine_maker.sh $dir2016 --lumi $lumi2016 --lumistr $lumi2016 --fit-name cic2016  $common_opts --fit-background --load lumi.json --label pr_plot --no-plot-fit-bands --read-ws full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9/full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9_grav_001.root --read-ws full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9/full_analysis_spring16v2_sync_v6_ichep_cic2016_default_shapes_spin0_parametric_v2_lumi_12.9.root --make-pr-plot 

exit

### common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --prepare-data --no-parametric-signal-nuisances --saveas pdf,root,convert_png --plot-blind 500,13000"
### ./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016  $common_opts --fit-background --mag-field 3.8T --load lumi.json --label fit_blind
### 
### ## exit

## common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias --prepare-data --do-parametric-signal-nuisances --plot-blind 500,4000"

## common_opts="--plot-fit-bands --rescale-signal-to 1e-3 --minos-bands --compute-fwhm --prepare-data --no-parametric-signal-nuisances --saveas pdf,root,convert_png --plot-blind 500,13000"
## ./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016  $common_opts --fit-background --load lumi.json --label fit_blind

common_opts="--rescale-signal-to 1e-3 --compute-fwhm --generate-ws-bkgnbias-new --prepare-data --do-parametric-signal-nuisances --plot-blind 500,4000"

parallel --ungroup -j 1 "./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016 $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes80X_ws_kMpl{}.root   --parametric-signal-acceptance  acceptance_76.json --load lumi.json --only-coups {}   --label spin2_${label} --do-parametric-signal-nuisances --model-strip-coeff-names 016" ::: $coups &

parallel --ungroup -j 1 "./combine_maker.sh $dir2016 --lumi $lumi2016 --fit-name cic2016 $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes80X_ws_kMpl{}.root --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_${label} --do-parametric-signal-nuisances --model-strip-coeff-names 016" ::: $coups &


wait


## 2015 13TeV
dir38=full_analysis_moriond16v1_sync_v4_data
lumi2015=2.69
eosdir38=~/eos/cms/store/user/crovelli/WSdiphoton76x_v4


common_opts="--rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias-new --prepare-data --do-parametric-signal-nuisances"

## 38T workspaces
parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi $lumi2015 --fit-name cic2 $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes76X_38T_ws_kMpl{}.root --parametric-signal-acceptance acceptance_76.json --load lumi.json --only-coups {}   --label spin2_${label} --do-parametric-signal-nuisances" ::: $coups &

parallel --ungroup -j 1 "./combine_maker.sh $dir38 --lumi $lumi2015 --fit-name cic2 $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes76X_38T_ws_kMpl{}.root  --parametric-signal-acceptance  acceptance_76_spin0.json --load lumi.json --only-coups {}   --label spin0_${label} --do-parametric-signal-nuisances" ::: $coups &


wait


## 2015 13TeV
dir0=full_analysis_moriond16v1_0T_sync_v6_data
lumi2015=0.59
## coups=001

common_opts="--rescale-signal-to 1e-3 --minos-bands --compute-fwhm --generate-ws-bkgnbias-new --prepare-data --do-parametric-signal-nuisances --load '{\"data\":[\"DoubleEG_13TeV:__infile__/%(sel)s\"]}'"

## 38T workspaces
parallel --ungroup -j 1 "./combine_maker.sh $dir0 --lumi $lumi2015 --fit-name cic0T $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes76X_0T_ws_kMpl{}.root --parametric-signal-acceptance acceptance_0T.json --load lumi.json --only-coups {}   --label spin2_${label} --do-parametric-signal-nuisances" ::: $coups &

parallel --ungroup -j 1 "./combine_maker.sh $dir0 --lumi $lumi2015 --fit-name cic0T $common_opts --parametric-signal-new ~soffi/public/4Pasquale/ParametricSignalModels/SignalParametericShapes76X_0T_ws_kMpl{}.root  --parametric-signal-acceptance  acceptance_0T_spin0.json --load lumi.json --only-coups {}   --label spin0_${label} --do-parametric-signal-nuisances" ::: $coups &


wait

exit

## coups="001 01 02"
## coups="001"

common_opts="--fit-name cic1 --plot-fit-bands  --minos-bands  --rescale-signal-to 1e-3  --load combine_maker_8TeV.json  --lumi 19.6  --load lumi_8TeV.json  --compute-fwhm --generate-ws-bkgnbias-new --saveas pdf,root,convert_png"

### common_opts="--fit-name cic1 --plot-fit-bands  --minos-bands  --rescale-signal-to 1e-3  --load combine_maker_8TeV.json --lumi 19.6  --load lumi_8TeV.json  --saveas pdf,root,convert_png"
### ./combine_maker_8TeV.sh input_8TeV $common_opts --label bkg_fit --fit-background

parallel -j 1 "./combine_maker_8TeV.sh input_8TeV $common_opts --parametric-signal ${dirSig}/signalModel76x_{}_500to998__8TeV.root --parametric-signal-new ~musella/public/workspace/paramModels/SignalParametericShapes8TeVOtman_ws_kMpl{}.root --parametric-signal-acceptance acceptance_8TeV.json --only-coups {} --label spin2_${label} --rescale-signal-ratio '(x<4500.)*(((0.375128+(-0.000203595*x))+(3.69901e-08*(x*x)))+(-2.20622e-12*(x*(x*x))))'" ::: $coups

parallel -j 1 "./combine_maker_8TeV.sh input_8TeV $common_opts --parametric-signal ${dirSig}/signalModel76x_{}_500to998__8TeV.root --parametric-signal-new ~musella/public/workspace/paramModels/SignalParametericShapes8TeVOtman_ws_kMpl{}.root --parametric-signal-acceptance acceptance_8TeV_spin0_fix.json --only-coups {} --label spin0_${label} --rescale-signal-ratio '(x<4500.)*(0.408904+(-0.000304607*x)+(9.7501e-08*x*x)+(-1.70199e-11*x*x*x)+(1.37876e-15*x*x*x*x))'" ::: $coups
