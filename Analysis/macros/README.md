# Wild notes to keep track of command line options needed for the different analysis tasks

## Control plots

### Event selection monitoring
`./control_plots.sh full_analysis_anv1_v18 ~/www/exo/phys_14_anv1`

## Templates generation

## Merge trees and fill template variables
`./templates_maker.py --load templates_maker.json --input-dir full_analysis_anv1_v18 -o full_analysis_anv1_v18/templates.root`

## Event mixing
`./templates_maker.py --load templates_maker.json --read-ws full_analysis_anv1_v18/templates.root --mix-templates`

## Comparison plots


## Bkg bias study

### Preparing input
`./templates_maker.py --load templates_maker.json --selection cic -o full_analysis_anv1_v13/bias_study_input.root --input-dir full_analysis_anv1_v13 --only-subset 2D`

###Comparison plots for templates
./templates_maker.py --load templates_maker.json  --read-ws mix016_v18.root -O /afs/cern.ch/user/m/mquittna/www/diphoton/Phys14/compare_temp056_v18_2comp -o fit056_v18_mb5_w5.root  --store-new-only --compare-templates --fit-massbins 5,5,0

### 2d fit with unrolled histograms
- `./templates_maker.py --load templates_maker.json  --read-ws fit056_v18_mb5_w5_template.root -O /afs/cern.ch/user/m/mquittna/www/diphoton/Phys14/compare_temp056_v18_3comp -o fit056_v18_mb5_3comp_all.root --nominal-fit --fit-massbins 5,5,0 --fit-template unrolled_mctruth --fit-categories EBEB`
-fit-template can be unrolled_mctruth or unrolled_tempalte
-massbins: overall number of bins, how many bins we want to run over, startbin
-fit-categories: EBEB or EBEE

### plots for purity vs massbins and pull function
- ./templates_maker.py --load templates_maker.json  --read-ws fit056_v18_mb5_w5_all.root -O /afs/cern.ch/user/m/mquittna/www/diphoton/Phys14/test -o purity076_test.root --plot-purity --plot-closure mctruth --plot-purityvalue fraction
-plot-closure for mctruth or tempalte
---plot-purityvalue either fraction or number of events

### Throwing toys
- `./bkg_bias.py --throw-toys --throw-from-model --lumi-factor=5. --n-toys=1000 --components pp --models dijet --fit 2D --store-new-only --read-ws full_analysis_anv1_v13/bias_study_input.root -o full_analysis_anv1_v13/bias_study_toys_from_fit_unbinned_5fb.root  -O ~/www/exo/phys_14_anv1/bkg_model_v0/`
- `./bkg_bias.py --throw-toys --lumi-factor=5. --n-toys=1000 --components pp --models dijet --fit 2D --store-new-only --read-ws full_analysis_anv1_v13/bias_study_input.root -o full_analysis_anv1_v13/bias_study_toys_from_mc_unbinned_5fb.root  -O ~/www/exo/phys_14_anv1/bkg_model_v0/`

### Fitting toys
- `./submit_toys.sh 8nm full_analysis_anv1_v13/bias_study_toys_from_fit_unbinned_5fb.root bias_study_toys_from_fit_cmp_to_gen_5fb_1000 1000 2`
- `./submit_toys.sh 8nm full_analysis_anv1_v13/bias_study_toys_from_mc_unbinned_5fb.root  bias_study_toys_from_mc_cmp_to_gen_5fb_1000 1000 2`

### Analyze results
- `./hadd_toys.sh bias_study_toys_from_mc_cmp_to_gen_5fb_1000/`
- `./hadd_toys.sh bias_study_toys_from_fit_cmp_to_gen_5fb_1000/`
- `./bkg_bias.py --analyze-bias --bias-files bias_study_toys_from_mc_cmp_to_gen_5fb_1000/toys.root --bias-labels mc --bias-files bias_study_toys_from_fit_cmp_to_gen_5fb_1000/toys.root --bias-labels fit -O ~/www/exo/phys_14_anv1/bkg_model_v0/5fb`


## Preparing combine inputs

### Background model
- `./combine_maker.py --load templates_maker.json --fit-name cic --input-dir ~musella/public/workspace/exo/full_analysis_anv1_v14  -o full_analysis_anv1_v14_final_ws.root`
- `./combine_maker.py --fit-name cic  --fit-background --read-ws full_analysis_anv1_v14_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v14_bkg_ws.root`
- `./combine_maker.py --generate-signal-dataset --read-ws full_analysis_anv1_v14_final_ws.root --fit-name cic --load templates_maker.json --signal-name grav_001_1500 -o grav_001_1500.root`
-`./combine_maker.py --generate-datacard --read-ws full_analysis_anv1_v14_bkg_ws.root --fit-name cic --load templates_maker.json --signal-name grav_001_1500 --signal-root-file grav_001_1500.root --background-root-file full_analysis_anv1_v14_bkg_ws.root`


### Per-component background model
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5700,300,6000] --read-ws full_analysis_anv1_v18_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v18_bkg_ws.root  --bkg-shapes bkg_model/split_shapes.json`

### Per-component background model with constrained purities
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5700,300,6000] --read-ws full_analysis_anv1_v18_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v18_bkg_ws.root   --norm-as-fractions --nuisance-fractions-covariance bkg_model/split_covariance.json --bkg-shapes bkg_model/split_shapes.json`

### Running combine tool 

### Significance
- `combine -M ProfileLikelihood --expectSignal 1 --significance -t -1 dataCard_full_analysis_anv1_v14_bkg_ws.txt`
### Limits
- `combine -M Asymptotic -t -1 --run expected dataCard_full_analysis_anv1_v14_bkg_ws.txt`
