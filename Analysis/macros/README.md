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
`./templates_make.py --load templates_maker.json --selection cic -o full_analysis_anv1_v18/bias_study_input.root --input-dir full_analysis_anv1_v18 --only-subset 2D`

### Throwing toys
- `./bkg_bias.py --throw-toys --throw-from-model --lumi-factor=10. --n-toys=1000 --components pp --models dijet --fit-name 2D --store-new-only --read-ws full_analysis_anv1_v18/bias_study_input.root -o full_analysis_anv1_v18/bias_study_toys_from_fit_unbinned_10fb.root  -O ~/www/exo/phys_14_anv1/full_analysis_v18/bkg_model_v0/ --observable mass[300,6000]`
- `./bkg_bias.py --throw-toys --lumi-factor=10. --n-toys=1000 --components pp --models dijet --fit-name 2D --store-new-only --read-ws full_analysis_anv1_v18/bias_study_input.root -o full_analysis_anv1_v18/bias_study_toys_from_mc_unbinned_10fb.root  -O ~/www/exo/phys_14_anv1/full_analysis_v18/bkg_model_v0/ --observable mass[300,6000]`

### Fitting toys
- `./submit_toys.sh 8nm full_analysis_anv1_v18/bias_study_toys_from_fit_unbinned_10fb.root bias_study_toys_from_fit_cmp_to_gen_10fb_1000 1000 2`
- `./submit_toys.sh 8nm full_analysis_anv1_v18/bias_study_toys_from_mc_unbinned_10fb.root  bias_study_toys_from_mc_cmp_to_gen_10fb_1000 1000 2`

### Analyze results
- `./hadd_toys.sh bias_study_toys_from_mc_cmp_to_gen_10fb_1000/`
- `./hadd_toys.sh bias_study_toys_from_fit_cmp_to_gen_10fb_1000/`
- `./bkg_bias.py --analyze-bias --bias-files bias_study_toys_from_mc_cmp_to_gen_10fb_1000/toys.root --bias-labels mc --bias-files bias_study_toys_from_fit_cmp_to_gen_5fb_1000/toys.root --bias-labels fit -O ~/www/exo/phys_14_anv1/bkg_model_v0/5fb`


## Preparing combine inputs

### Background model
- `./combine_maker.py --load templates_maker.json --fit-name cic --input-dir ~musella/public/workspace/exo/full_analysis_anv1_v14  -o full_analysis_anv1_v14_final_ws.root`
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5000,300,6000] --read-ws full_analysis_anv1_v14_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v14_bkg_ws.root`

### Per-component background model
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5700,300,6000] --read-ws full_analysis_anv1_v18_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v18_bkg_ws.root  --bkg-shapes bkg_model/split_shapes.json`

### Per-component background model with constrained purities
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5700,300,6000] --read-ws full_analysis_anv1_v18_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v18_bkg_ws.root   --norm-as-fractions --nuisance-fractions-covariance bkg_model/split_covariance.json --bkg-shapes bkg_model/split_shapes.json`

### Running combine tool 

### Significance
- `combine -M ProfileLikelihood --expectSignal 1 --significance -t -1 dataCard_full_analysis_anv1_v14_bkg_ws.txt`
### Limits
- `combine -M Asymptotic -t -1 --run expected dataCard_full_analysis_anv1_v14_bkg_ws.txt`
