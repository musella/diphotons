#!/bin/bash

set -x 

# generate MC truth model for 2D fit
combineCards.py --xc .*EBE[BE]$ \
    full_analysis_anv1_v19_2D_split_shapes_semiparam_test_lumi_5/datacard_full_analysis_anv1_v19_2D_split_shapes_semiparam_test_lumi_5_grav_02_1500.txt \
    | grep -v groups > full_analysis_anv1_v19_2D_split_shapes_lumi_5_control.txt

combineCards.py \
    full_analysis_anv1_v19_2D_truth_shapes_truth_templates_semiparam_lumi_5/datacard_full_analysis_anv1_v19_2D_truth_shapes_truth_templates_semiparam_lumi_5_grav_02_1500.txt \
    full_analysis_anv1_v19_2D_split_shapes_lumi_5_control.txt \
    | grep -v groups | sed 's%ch[12]_%%g' > full_analysis_anv1_v19_2D_split_shapes_lumi_5_truth.txt

combine -M GenerateOnly full_analysis_anv1_v19_2D_split_shapes_lumi_5_truth.txt -n _truth -m 0 -t -1 --saveToys -L libdiphotonsUtils


