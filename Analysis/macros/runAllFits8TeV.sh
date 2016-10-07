## parallel --ungroup -j 1 './combine_maker.sh full_analysis_spring15_7415v2_sync_v6 --data-file ~/public/workspace/exo/full_analysis_spring15_7415v2_sync_v5_data_ecorr/output.root --lumi 2.4 --fit-name cic2 --plot-fit-bands --rescale-signal-to 1e-3  --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m1000to4900.root --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m500to998.root  --parametric-signal-xsection xsections.json --parametric-signal-acceptance  acceptance_pu.json --load lumi.json  --compute-fwhm --generate-ws-bkgnbias --only-coups {} --label approval --minos-bands' ::: 001 005 007 01 015 02

coups="001 01 02"
## coups="001"

common_opts="--fit-name cic1 --plot-fit-bands  --minos-bands  --rescale-signal-to 1e-3  --load combine_maker_8TeV.json  --lumi 19.6  --load lumi_8TeV.json  --compute-fwhm --generate-ws-bkgnbias --saveas pdf,root,convert_png"
dirSig="~/eos/cms//store/user/crovelli/WSdiphoton76x_8TeV"

common_opts="--fit-name cic1 --plot-fit-bands  --minos-bands  --rescale-signal-to 1e-3  --load combine_maker_8TeV.json --lumi 19.6  --load lumi_8TeV.json  --saveas pdf,root,convert_png"
./combine_maker_8TeV.sh input_8TeV $common_opts --label bkg_fit --fit-background

### /store/user/crovelli/WSdiphoton76x_8TeV/signalModel76x_001_1000to4000__extendedRange__8TeV.root
### /store/user/crovelli/WSdiphoton76x_8TeV/signalModel76x_01_1000to4000__extendedRange__8TeV.root


##parallel -j 1 "./combine_maker_8TeV.sh input_8TeV $common_opts --parametric-signal ${dirSig}/signalModel76x_{}_500to998__8TeV.root --parametric-signal ${dirSig}/signalModel76x_{}_1000to4000__extendedRange__8TeV.root --parametric-signal-acceptance acceptance_8TeV.json --only-coups {} --label ext_spin2 --rescale-signal-ratio '((0.375128+(-0.000203595*x))+(3.69901e-08*(x*x)))+(-2.20622e-12*(x*(x*x)))'" ::: $coups
##
##parallel -j 1 "./combine_maker_8TeV.sh input_8TeV $common_opts --parametric-signal ${dirSig}/signalModel76x_{}_500to998__8TeV.root --parametric-signal ${dirSig}/signalModel76x_{}_1000to4000__extendedRange__8TeV.root --parametric-signal-acceptance acceptance_8TeV_spin0_fix.json --only-coups {} --label ext_spin0 --rescale-signal-ratio '0.408904+(-0.000304607*x)+(9.7501e-08*x*x)+(-1.70199e-11*x*x*x)+(1.37876e-15*x*x*x*x)'" ::: $coups

### parallel --ungroup './splitallcards.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_approval_lumi_2.4 {}' ::: EBEB EBEE

## ./combineall.sh input_8TeV_cic1_default_shapes_combination_pcorr_lumi_19.6 001 -M ProfileLikelihood --pvalue --significance  --hadd  &
## ./combineall.sh input_8TeV_cic1_default_shapes_combination_pcorr_lumi_19.6 001 -M Asymptotic --run both   --hadd  &

wait

