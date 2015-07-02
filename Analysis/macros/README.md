# Wild notes to keep track of command line options needed for the different analysis tasks

## Control plots

### Event selection monitoring
`./control_plots.sh full_analysis_anv1_v19 ~/www/exo/phys_14_anv1`

## Templates generation

## Merge trees and fill template variables
`./templates_maker.py --load templates_maker.json,templates_maker_fits.json --input-dir full_analysis_anv1_v19 -o full_analysis_anv1_v19/templates.root`

## Event mixing
`./templates_maker.py --load templates_maker_fits.json --read-ws full_analysis_anv1_v19/templates.root --mix-templates`

 --store-new-only possible, then two times --read-ws afterwards for 2 files
- also possible to load different json files after another
## Comparison plots


## Bkg bias study

### Preparing input
`./templates_make.py --load templates_maker.json,templates_maker_fits.json --selection cic -o full_analysis_anv1_v19/bias_study_input.root --input-dir full_analysis_anv1_v19 --only-subset 2D`

###Comparison plots for templates
./templates_maker.py --load templates_maker_fits.json  --read-ws mix016_v19.root -O /afs/cern.ch/user/m/mquittna/www/diphoton/Phys14/compare_temp056_v19_2comp -o fit056_v19_mb5_w5.root  --store-new-only --compare-templates --fit-massbins 5,5,0

### 2d fit with unrolled histograms
- `./templates_maker.py --load templates_maker_fits.json  --read-ws fit056_v19_mb5_w5_template.root -O /afs/cern.ch/user/m/mquittna/www/diphoton/Phys14/compare_temp056_v19_3comp -o fit056_v19_mb5_3comp_all.root --nominal-fit --fit-massbins 5,5,0 --fit-template unrolled_mctruth --fit-categories EBEB`
-fit-template can be unrolled_mctruth or unrolled_tempalte
-massbins: overall number of bins, how many bins we want to run over, startbin
-fit-categories: EBEB or EBEE

### plots for purity vs massbins and pull function
- ./templates_maker.py --load templates_maker_fits.json  --read-ws fit056_v19_mb5_w5_all.root -O /afs/cern.ch/user/m/mquittna/www/diphoton/Phys14/test -o purity076_test.root --plot-purity --plot-closure mctruth --plot-purityvalue fraction
-plot-closure for mctruth or tempalte
---plot-purityvalue either fraction or number of events

### Throwing toys
- `./bkg_bias.py --throw-toys --throw-from-model --lumi-factor=10. --n-toys=1000 --components pp --models dijet --fit-name 2D --store-new-only --read-ws full_analysis_anv1_v19/bias_study_input.root -o full_analysis_anv1_v19/bias_study_toys_from_fit_unbinned_10fb.root  -O ~/www/exo/phys_14_anv1/full_analysis_v19/bkg_model_v0/ --observable mass[1140,300,6000]`
- `./bkg_bias.py --throw-toys --lumi-factor=10. --n-toys=1000 --components pp --models dijet --fit-name 2D --store-new-only --read-ws full_analysis_anv1_v19/bias_study_input.root -o full_analysis_anv1_v19/bias_study_toys_from_mc_unbinned_10fb.root  -O ~/www/exo/phys_14_anv1/full_analysis_v19/bkg_model_v0/ --observable mass[1140,300,6000]`

### Fitting toys
- `./submit_toys.sh 8nm full_analysis_anv1_v19/bias_study_toys_from_fit_unbinned_10fb.root bias_study_toys_from_fit_cmp_to_gen_10fb_1000 1000 2`
- `./submit_toys.sh 8nm full_analysis_anv1_v19/bias_study_toys_from_mc_unbinned_10fb.root  bias_study_toys_from_mc_cmp_to_gen_10fb_1000 1000 2`

### Analyze results
- `./hadd_toys.sh bias_study_toys_from_mc_cmp_to_gen_10fb_1000/`
- `./hadd_toys.sh bias_study_toys_from_fit_cmp_to_gen_10fb_1000/`
- `./bkg_bias.py --analyze-bias --bias-files bias_study_toys_from_mc_cmp_to_gen_10fb_1000/toys.root --bias-labels mc --bias-files bias_study_toys_from_fit_cmp_to_gen_5fb_1000/toys.root --bias-labels fit -O ~/www/exo/phys_14_anv1/bkg_model_v0/5fb`


## Preparing combine inputs

### Background model
- `./combine_maker.py --load templates_maker.json,templates_maker_fits.json --fit-name cic --input-dir ~musella/public/workspace/exo/full_analysis_anv1_v14  -o full_analysis_anv1_v14_final_ws.root`
- `./combine_maker.py --fit-name cic  --fit-background --read-ws full_analysis_anv1_v14_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v14_bkg_ws.root`
- `./combine_maker.py --generate-signal-dataset --read-ws full_analysis_anv1_v14_final_ws.root --fit-name cic --load templates_maker.json --signal-name grav_001_1500 -o grav_001_1500.root`
-`./combine_maker.py --generate-datacard --read-ws full_analysis_anv1_v14_bkg_ws.root --fit-name cic --load templates_maker_fits.json --signal-name grav_001_1500 --signal-root-file grav_001_1500.root --background-root-file full_analysis_anv1_v14_bkg_ws.root`


### Per-component background model
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5700,300,6000] --read-ws full_analysis_anv1_v19_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v19_bkg_ws.root  --bkg-shapes bkg_model/split_shapes.json`

### Per-component background model with constrained purities
- `./combine_maker.py --fit-name cic  --fit-background   --observable mgg[5700,300,6000] --read-ws full_analysis_anv1_v19_final_ws.root -O ~/www/test_bkg_fit -o full_analysis_anv1_v19_bkg_ws.root   --norm-as-fractions --nuisance-fractions-covariance bkg_model/split_covariance.json --bkg-shapes bkg_model/split_shapes.json`

### Semi-parametric templates vs isolation
- `./templates_maker.py --load templates_maker.json,templates_maker_fits.json --mix-templates --input-dir  ~musella/public/workspace/exo/full_analysis_anv1_v19  -o full_analysis_anv1_v19_final_ws_semiparam.root --only-subset 2D,singlePho`

- `./combine_maker.py --fit-name 2D  --fit-background   --observable mgg[11460,270,6000] --read-ws full_analysis_anv1_v19_final_ws_semiparam.root -O ~/www/exo/full_analysis_anv1_v19/test_bkg_fit_semiparam_split_shapes -o full_analysis_anv1_v19_bkg_ws_semiparam_truth_shapes.root  --use-templates  --bkg-shapes bkg_model/truth_shapes.json --plot-norm-dataset --plot-binning '191,270,6000'`
- `./combine_maker.py --fit-name 2D  --fit-background   --observable mgg[11460,270,6000] --read-ws full_analysis_anv1_v19_final_ws_semiparam.root -O ~/www/exo/full_analysis_anv1_v19/test_bkg_fit_semiparam_truth_shapes -o full_analysis_anv1_v19_bkg_ws_semiparam_truth_shapes.root  --use-templates  --bkg-shapes bkg_model/truth_shapes.json --plot-norm-dataset --plot-binning '191,270,6000'`
- Notes
  - `bkg_model/truth_shapes.json` uses MC truth for mgg shape, but data-driven templates.
  - `bkg_model/split_shapes.json` takes mgg shape from control region and data-driven templates. 
     Control regions are then added to combine datacard.
  - adding `--norm-as-fractions` fits purties instead of absolute normaliztions.  
  - `--template-binnning <binning>` overwrites the default template binning
  - `--plot-fit-bands --fast-bands` adds uncertainties bands on the models

### Running combine tool 

### Using custom pdfs
We have to run text2workspace.py by hand since combine forgets to pass the list of libraries to be loaded. eg:
- `text2workspace.py -L libdiphotonsUtils  -m 1500  -o dataCard_grav_001_1500.root  dataCard_grav_001_1500.txt`
- `combine -M ProfileLikelihood --expectSignal 1 --pvalue --significance -t -1  -m 1500 dataCard_grav_001_1500.root -L libdiphotonsUtils`

### Significance
- `combine -M ProfileLikelihood --expectSignal 1 --significance -t -1 dataCard_full_analysis_anv1_v14_bkg_ws.txt`
### Limits
- `combine -M Asymptotic -t -1 --run expected dataCard_full_analysis_anv1_v14_bkg_ws.txt`
