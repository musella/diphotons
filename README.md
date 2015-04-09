# Repository for high mass diphoton analysis.

## Checking out the code

This package depends on the [flashgg](https://github.com/cms-analysis/flashgg).

A quick set of commands to check out the code is below. 
They assume that you have set your user github name in your git config and that you have 
forked the flashgg and this repository. 

```
# change these to the values that you prefer
PROJECT_AREA=EXO_7_2_2_patch2 
CMSSW_VERSION=CMSSW_7_2_2_patch2
FLASHGG_TAG=diphotonsPhys14AnV1

# read github name from git config
MY_GITHUB_NAME=$(git config --get user.github)

scram project -n ${PROJECT_AREA} CMSSW ${CMSSW_VERSION}
cd ${PROJECT_AREA}/src

cmsenv 

cd ${CMSSW_BASE}/src
git cms-init

# clone flashgg 
cd ${CMSSW_BASE}/src
git clone https://github.com/cms-analysis/flashgg.git

cd flashgg
git remote add musella git@github.com:musella/flashgg.git
git fetch musella
git co -b topic_${FLASHGG_TAG} ${FLASHGG_TAG} 

bash ./setup.sh | tee setup.log

# add your own fork as a remote. Skip if you dont have one
cd flashgg 
git remote add ${MY_GITHUB_NAME} git@github.com:${MY_GITHUB_NAME}/flashgg.git
git fetch ${MY_GITHUB_NAME}

# clone this repository
cd ${CMSSW_BASE}/src
git clone https://github.com/musella/diphotons.git

# add your own fork as a remote. Skip if you dont have one
cd diphotons
git remote add ${MY_GITHUB_NAME} git@github.com:${MY_GITHUB_NAME}/diphotons.git
git fetch ${MY_GITHUB_NAME}

# now compile everything
cd ${CMSSW_BASE}/src
scram b -j 16

```

## Producing microAODs

The so-called microAOD format is a subset of the MINIAOD produced by the flashgg framework, adding specific photon-related informations.

Only high level objects such as photons, leptons, jets and MET are stpored in microAD by default. If additional informations are needed, 
they need to be added explicitely to the output.

The package flashgg/MetaData contains the scripts related to microAOD production.
To start with, go to `flashgg/MetaData/work`.

```
cmsenv
cd ${CMSSW_BASE}/src/flashgg/MetaData/work 
```

The folder contains the script `prepareCrabJobs.py`, which sets up the crab3 tasks needed to generate microAODs. 

First, you need to set up the crab3 and grid environment.

```
source /cvmfs/cms.cern.ch/crab3/crab.sh
voms-proxy-init --voms cms --valid 168:00
```

Then, prepare a json file with the list of datasets to be processed, as in the example below.

```
{
    "data" : ["/DoubleElectron/CMSSW_7_0_6_patch1-GR_70_V2_AN1_RelVal_zEl2012D-v1/MINIAOD"
              ],
    "sig"  : ["/GluGluToHToGG_M-125_13TeV-powheg-pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v2/MINIAODSIM",
              "/VBF_HToGG_M-125_13TeV-powheg-pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v1/MINIAODSIM",
              "/WH_ZH_HToGG_M-125_13TeV_pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v2/MINIAODSIM",
              "/TTbarH_HToGG_M-125_13TeV_pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v1/MINIAODSIM",
              "/TTbarH_HToGG_M-125_13TeV_amcatnlo-pythia8-tauola/Spring14miniaod-PU20bx25_POSTLS170_V5-v1/MINIAODSIM",
              "/GluGluToHHTo2B2G_M-125_13TeV-madgraph-pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v1/MINIAODSIM"
              ],
    "bkg"  : ["/GJet_Pt20to40_doubleEMEnriched_TuneZ2star_13TeV-pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v1/MINIAODSIM",
              "/GJet_Pt40_doubleEMEnriched_TuneZ2star_13TeV-pythia6/Spring14miniaod-PU20bx25_POSTLS170_V5-v2/MINIAODSIM",
              "/DYJetsToLL_M-50_13TeV-madgraph-pythia8-tauola_v2/Spring14miniaod-PU20bx25_POSTLS170_V5-v1/MINIAODSIM"
              ]
}
```

To generate the crab3 configuration run (see `./prepareCrabJobs.py --help` for the full list of options).
```
./prepareCrabJobs.py -p <parameterSet> \
		     -C <microAODCampaginName> \
		     -s <jsonFileWithDatasetList> \
		     --mkPilot
```

This will create a folder containing all the crab3 configuration files, the CMSSW parameterSet as well as a file called `config.json` containing the dump
of all the script options.
The latter can be used to reproduce the same configuration (using the `--load` option), eg when adding more dataset to the same campaign.
```
./prepareCrabJobs.py --load <previous_config.json> \
		     -s <jsonFileWithAdditionalDatasets>
```

The `--mkPilot` option will create an additional crab3 configuration that can be used to run on a single file, before submitting the whole list of tasks.

You can now lanch the microAOD production.

```
cd <microAODCampaginName>
echo crabConfig_*.py | xargs -n 1 crab sub
## or using GNU parallel 
## parallel 'crab sub {}' ::: crabConfig_*.py
```

### Configuration for Phys14 AN studies
```
cmsenv

source /cvmfs/cms.cern.ch/crab3/crab.sh
voms-proxy-init --voms cms --valid 168:00

cd ${CMSSW_BASE}/src/flashgg/MetaData/work

cp -p  ${CMSSW_BASE}/src/diphotons/MetaData/work/campaigns/AN_Phys14_samples.json campaigns/MyAN_Phys14_samples.json
ln -sf  ${CMSSW_BASE}/src/diphotons/MetaData/work/analysis_microAOD.py .

# edit list of samples to be actually submitted 
emacs -nw campaigns/MyANPhys14_samples.json
./prepareCrabJobs.py -C ExoPhys14AN -s campaigns/AN_MyPhys14_samples.json -p analysis_microAOD.py  --mkPilot

# submit pilot jobs
cd ExoPhys14AN
echo pilot* | xargs -n 1 crab sub

```

### Minitree dumper

#### Set up file catalog

See instructions in flashgg/MetaData/README.md

File catalog for ExoPhys14 is in git.
```
# Needs flashgg after mergin of https://github.com/cms-analysis/flashgg/pull/136
# can be taken from musella:topic_jobs_driver
fggManageSamples.py -m diphotons -C ExoPhys14 list
```

#### Run

```
cd Analysis/config
cp -p jobs_gamjets_gamgam.json myjobs.json
emacs -nw photonIdAnalyzer.py myjobs.json

# Needs flashgg after mergin of https://github.com/cms-analysis/flashgg/pull/136
# can be taken from musella:topic_jobs_driver
# alternative, can still use MetaData/test/runJobs.py but not 100% safe
fggRunJobs.py --load myjobs.json -d myjobs ./photonIdAnalyzer.py maxEvent=100

fggRunJobs.py --load myjobs.json -d myjobs -H -D -P -n 5 ./photonIdAnalyzer.py maxEvent=-1
```
