#!/bin/bash

common_opts="--include-flat-params-in-groups"

## # nominal selection
## ./combine_maker.sh full_analysis_anv1_v19 --fit-name cic2  $common_opts --luminosity 5

# semi-parametric fit
./combine_maker.sh full_analysis_anv1_v19 --fit-name 2D   $common_opts --luminosity 5 --norm-as-fractions --use-templates --bkg-shapes bkg_model/split_shapes.json

# variations of semi-parametric fit to study syst uncertainties
./combine_maker.sh full_analysis_anv1_v19 --fit-name 2D   $common_opts --luminosity 5 --norm-as-fractions --use-templates --bkg-shapes bkg_model/split_shapes_truth_templates.json
./combine_maker.sh full_analysis_anv1_v19 --fit-name 2D   $common_opts --luminosity 5 --norm-as-fractions --use-templates --bkg-shapes bkg_model/truth_shapes_truth_templates.json
./combine_maker.sh full_analysis_anv1_v19 --fit-name 2D   $common_opts --luminosity 5 --norm-as-fractions --use-templates --bkg-shapes bkg_model/truth_shapes.json
## ./combine_maker.sh full_analysis_anv1_v19 --fit-name 2D   $common_opts --luminosity 5 --bkg-shapes bkg_model/truth_shapes_truth_templates.json --select-components pp,pf

