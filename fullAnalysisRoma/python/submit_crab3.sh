#!/bin/bash

declare -a DATASETS=(#"/DoubleEG/musella-EXOSpring15_v8-Spring15BV2_PM_patch3-v0-Run2015B-PromptReco-v1-05a5d564021414fa3556ebaec3c8e16b/USER",
                     "/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/musella-EXOSpring15_v8-Spring15BV2_PM_patch3-v0-RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2-2dae4b40c52b5ed4955a59a04b7390a5/USER",
                     # "/WJetsToLNu_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/musella-EXOSpring15_v2-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1-a7f089b9a439c245189cf8643d38aeca/USER",
                     # "/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/musella-EXOSpring15_v3-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1-a7f089b9a439c245189cf8643d38aeca/USER",
                     # "/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/musella-EXOSpring15_v3-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3-a7f089b9a439c245189cf8643d38aeca/USER",
                     # "/WJetsToLNu_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/musella-EXOSpring15_v3-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1-a7f089b9a439c245189cf8643d38aeca/USER",
                     # "/WZ_TuneCUETP8M1_13TeV-pythia8/musella-EXOSpring15_v2-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1-a7f089b9a439c245189cf8643d38aeca/USER",
                     # "/ZZTo4L_13TeV_powheg_pythia8/musella-EXOSpring15_v2-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1-a7f089b9a439c245189cf8643d38aeca/USER",
                     # "/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/musella-EXOSpring15_v3-Spring15BetaV2-2-gfceadad-v0-RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2-a7f089b9a439c245189cf8643d38aeca/USER"
                    )


for DATASET in "${DATASETS[@]}":
do
    NAME=$(echo $DATASET | tr "/" " " | awk '{print $1}')
    DATASET=$(echo $DATASET | sed 's/[,:]//g')
    echo $NAME
    if [ "$NAME" == "DoubleEG" ]; then
        crab submit --proxy=/tmp/x509up_u68758 -c run_TaP_crab3.py General.requestName=$NAME Data.inputDataset=$DATASET JobType.psetName='tnp_Data.py'
    else
        crab submit --proxy=/tmp/x509up_u68758 -c run_TaP_crab3.py General.requestName=$NAME Data.inputDataset=$DATASET
    fi
done              

