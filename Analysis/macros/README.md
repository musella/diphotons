## Bkg bias study

### Preparing input
`./templates_make.py --load templates_maker.json --selection cic -o full_analysis_anv1_v13/bias_study_input.root --input-dir full_analysis_anv1_v13 --only-subset 2D`

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
- `./combine_maker.py --fit-name cic  --fit-background  --read-ws full_analysis_anv1_v14_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v14_bkg_ws.root`

### Running combine tool 

### Significance
- `combine -M ProfileLikelihood --expectSignal 1 --significance -t -1 card_test1.txt`
### Limits
- `combine -M Asymptotic -t -1 --run expected card_test1.txt`
