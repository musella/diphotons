## parallel --ungroup -j 1 './combine_maker.sh full_analysis_spring15_7415v2_sync_v6 --data-file ~/public/workspace/exo/full_analysis_spring15_7415v2_sync_v5_data_ecorr/output.root --lumi 2.4 --fit-name cic2 --plot-fit-bands --rescale-signal-to 1e-3  --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m1000to4900.root --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m500to998.root  --parametric-signal-xsection xsections.json --parametric-signal-acceptance  acceptance_pu.json --load lumi.json  --compute-fwhm --generate-ws-bkgnbias --only-coups {} --label approval --minos-bands' ::: 001 005 007 01 015 02

./combine_maker_ZG.sh EventYields_v0_silver --luminosity 2.6 \
    --fit-name ${1:-fit_em} \
    --fit-background \
    --mc-data \
    --label combination_pcorr --load combine_maker_ZG.json \
    --redo-input \
    --generate-signal-dataset \
    --doubleCB-signal \
    --generate-datacard \
    --saveas png,pdf,eps \
    --signal-scalefactor-forpdf 1

#./combine_maker_ZG.sh EventYields_v0 --luminosity 2.3 \
    #--gaussian-signal \
    #--no-use-custom-pdfs \
    #--minos-bands \
    #--plot-fit-bands  \
    #--no-use-custom-pdfs \
    #--minos-bands \
    #--plot-fit-bands  \

### parallel --ungroup './splitallcards.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_approval_lumi_2.4 {}' ::: EBEB EBEE

## ./combineall.sh input_8TeV_cic1_default_shapes_combination_pcorr_lumi_19.6 001 -M ProfileLikelihood --pvalue --significance  --hadd  &
## ./combineall.sh input_8TeV_cic1_default_shapes_combination_pcorr_lumi_19.6 001 -M Asymptotic --run both   --hadd  &


wait

