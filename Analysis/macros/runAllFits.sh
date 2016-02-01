# coups="001 005 007 01 015 02"
## coups="001 01 02"
coups=001

parallel --ungroup -j 1 './combine_maker.sh full_analysis_spring15_7415v2_sync_v6 --data-file `pwd`/full_analysis_spring15_7415v2_sync_v5_data_ecorr/output.root --lumi 2.56 --fit-name cic2 --plot-fit-bands --rescale-signal-to 1e-3  --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m1000to4900.root --parametric-signal ~/eos/cms/store/user/crovelli/WSdiphotonConSmearings/nominalWSwithSmear_k{}_m500to998.root  --parametric-signal-xsection xsections.json --parametric-signal-acceptance  acceptance_pu.json --load lumi.json  --compute-fwhm --generate-ws-bkgnbias --only-coups {} --label pas --minos-bands' ::: $coups

parallel --ungroup './splitallcards.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56 {}' ::: EBEB EBEE

parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56 {} -M ProfileLikelihood --pvalue --significance  --hadd --rMax 60 --cont' ::: $coups &

parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56/EBEE {} -M ProfileLikelihood --pvalue --significance  --hadd --rMax 60 -n EBEE --cont' ::: $coups &
parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56/EBEB {} -M ProfileLikelihood --pvalue --significance  --hadd --rMax 60 -n EBEB --cont' ::: $coups &

wait

parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56 {} -M Asymptotic --run both --hadd --cont --rMax 60' ::: $coups &
parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56/EBEB {} -M Asymptotic --run both --hadd --cont --rMax 60 -n EBEB' ::: $coups &
## parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56/EBEE {} -M Asymptotic --run both --hadd --cont --rMax 60 -n EBEE' ::: 001 005 007 01 &
## parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56/EBEE {} -M Asymptotic --run both --hadd --cont --rMax 100 -n EBEE' ::: 015 02 &
parallel --ungroup './combineall.sh full_analysis_spring15_7415v2_sync_v5_data_ecorr_cic2_default_shapes_pas_lumi_2.56/EBEE {} -M Asymptotic --run both --hadd --cont --rMax 100 -n EBEE' ::: $coups &

wait

