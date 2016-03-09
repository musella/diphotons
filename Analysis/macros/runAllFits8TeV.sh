## parallel --ungroup -j 1 './combine_maker.sh full_analysis_spring15_7415v2_sync_v6 --data-file ~/public/workspace/exo/full_analysis_spring15_7415v2_sync_v5_data_ecorr/output.root --lumi 2.4 --fit-name cic2 --plot-fit-bands --rescale-signal-to 1e-3  --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m1000to4900.root --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m500to998.root  --parametric-signal-xsection xsections.json --parametric-signal-acceptance  acceptance_pu.json --load lumi.json  --compute-fwhm --generate-ws-bkgnbias --only-coups {} --label approval --minos-bands' ::: 001 005 007 01 015 02

coups="001 01 02"
## coups="01 02"

common_opts="--fit-name cic1 --plot-fit-bands  --minos-bands  --rescale-signal-to 1e-3  --load combine_maker_8TeV.json  --lumi 19.6  --load lumi.json  --compute-fwhm --generate-ws-bkgnbias"
dirSig="~/eos/cms//store/user/crovelli/WSdiphoton76x_8TeV"

parallel -j 1 "./combine_maker_8TeV.sh input_8TeV $common_opts --parametric-signal ${dirSig}/signalModel76x_{}_500to998__8TeV.root --parametric-signal ${dirSig}/signalModel76x_{}_1000to4900__8TeV.root --parametric-signal-acceptance acceptance_8TeV.json --only-coups {} --label spin2 --rescale-signal-ratio '((0.375128+(-0.000203595*x))+(3.69901e-08*(x*x)))+(-2.20622e-12*(x*(x*x)))'" ::: $coups

parallel -j 1 "./combine_maker_8TeV.sh input_8TeV $common_opts --parametric-signal ${dirSig}/signalModel76x_{}_500to998__8TeV.root --parametric-signal ${dirSig}/signalModel76x_{}_1000to4900__8TeV.root --parametric-signal-acceptance acceptance_8TeV_spin0.json --only-coups {} --label spin0 --rescale-signal-ratio '(0.361542+(-0.000176012*x))+(2.22554e-08*(x*x))'" ::: $coups

### parallel --ungroup './splitallcards.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_approval_lumi_2.4 {}' ::: EBEB EBEE

## ./combineall.sh input_8TeV_cic1_default_shapes_combination_pcorr_lumi_19.6 001 -M ProfileLikelihood --pvalue --significance  --hadd  &
## ./combineall.sh input_8TeV_cic1_default_shapes_combination_pcorr_lumi_19.6 001 -M Asymptotic --run both   --hadd  &

wait

