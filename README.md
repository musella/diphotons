# Repository for high mass diphoton analysis.

## Checking out the code

This package depends on the [flashgg](https://github.com/cms-analysis/flashgg).

A quick set of commands to check out the code is below. 
They assume that you have set your user github name in your git config and that you have 
forked the flashgg and this repository. 

```
# change these to the values that you prefer
PROJECT_AREA=EXO_7_4_0_pre9 
CMSSW_VERSION=CMSSW_7_4_0_pre9 
FLASHGG_TAG=diphtons_phys14

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

## make sure we use a consistent flashgg tag
cd flashgg
git remote add musella git@github.com:musella/flashgg.git
git fetch musella
git co -b topic_${FLASHGG_TAG} ${FLASHGG_TAG} 

cd ${CMSSW_BASE}/src
bash flashgg/setup.sh 2>&1 | tee flashgg/setup.log

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

## Running on microAODs

### Set up file catalog

See instructions in flashgg/MetaData/README.md

File catalog for ExoPhys14ANv1 is already in git (ie you can skip this part). To see the list of datasets run:
```
fggManageSamples.py -m diphotons -C ExoPhys14ANv1 list
```

### Jobs configuration

Full-framework and FWLite configurations are stored in `Analysis/config`. Example of how to run them:

```
cd Analysis/config
cp -p jobs_gamjets_gamgam.json myjobs.json
emacs -nw photonIdAnalyzer.py myjobs.json

# test with 100 events
fggRunJobs.py --load myjobs.json -d myjobs ./photonIdAnalyzer.py maxEvent=100

# submit all jobs. add -q <queue_name> to run in batch
fggRunJobs.py --load myjobs.json -d myjobs -H -D -P -n 5 ./photonIdAnalyzer.py maxEvent=-1
```

Configuration files
- `Analysis/config/high_mass_analysis.py`: full analysis with signal region and sidebands.
- `Analysis/config/regressionTreeAnalyzer.py`: regression training tree producer.
- `Analysis/config/photonIdAnalyzer.py`, `Analysis/config/photonIdDumper.py`: photon ID training tree producer.

JSON files containing list of processes.
- `Analysis/config/jobs_gamgam.json` : only GGJets sherpa
- `Analysis/config/jobs_gamjets.json` : only GJets madgraph
- `Analysis/config/jobs_gamjets_gamgam.json` : all processes

### Macros

Macros are stored in `Analysis/macros`.

They tipically come with associated JSON file for configuration. Python macros are run as:
`./macroName.py --load macroConfig.json [additional options, typilcally --input-dir <input dir> -O <output dir>]`

The `--help` option gives a list of supported options, while the `--dumpConfig` dumps all options in JSON format (useful to record a particular configuration).

- `Analysis/macros/idEvolution.py`: photon ID studies. Runs on the output of `Analysis/config/photonIdAnalyer.py` or `photonIdDumper.py`.
- `Analysis/macros/basic_plots.py`: basic di-photon plots. Runs on the output of `Analysis/config/high_mass_analysis.py`.
- `Analysis/macros/eff_plots.py`: efficiency for di-photon selection. Runs on the output of `Analysis/config/high_mass_analysis.py`.
- `Analysis/macros/templates_maker.py`: deals with templates creation.

##### Templates maker configuration. 

Note: JSON files do support comments, so remove the parts starting with `#` if you copy paste

```
{
    # ---------------------------------------------------------------------------------------------------------------------
    # Dataset definition
    # ---------------------------------------------------------------------------------------------------------------------
    #   %(sel)s is replaced with the appropriate string
    #   fromat is <sample-name>:[[input-file][/file-folder]]
    # data to be fit
    "data" : [ 
	       "GGJets_M_200To13000_sherpa_13TeV:output.root/%(sel)sGenIso",
	       "GGJets_M_200To13000_sherpa_13TeV:output.root/%(sel)sNonGenIso",
	       "QCD_HT_100toInf_13TeV:output.root/%(sel)s",
	       "GJets_HT_100toInf_13TeV:output.root/%(sel)s"		
	     ],
    # data used for template making
    "templates" : [
	       "GGJets_M_200To13000_sherpa_13TeV:output.root/%(sel)s",
	       "QCD_HT_100toInf_13TeV:output.root/%(sel)s",
	       "GJets_HT_100toInf_13TeV:output.root/%(sel)s"		
	     ],
     # data used for template making
     "mc" : [  # data used for template making
	     "GGJets_M_200To13000_sherpa_13TeV:output.root/%(sel)sGenIso",
	     "GGJets_M_200To13000_sherpa_13TeV:output.root/%(sel)sNonGenIso",
	       "QCD_HT_100toInf_13TeV:output.root/%(sel)s",
	     "GJets_HT_100toInf_13TeV:output.root/%(sel)s"
	    ],

    # file names for different datasets. Files folder can be specified by --input-dir on the command line
    "data_file"  : "output.root",
    "mc_file"    : "output.root",
    
    # categories for the input datasets
    "categories" : [],
    # groups of categories can be defined to resum some of the categories
    "groups": {
        "EBEB"       : ["EBHighR9","EBLowR9"],
        "notEBEB"    : ["EEHighR9","EELowR9"]
	}, 

    # how to compose the tree name starting from sub-pieces
    #  %(sample)s is replaced with the items in dataset definition
    #  %(cat)s is replaced with the category name
    "treeName": "trees/%(sample)s_%(cat)s",
    

    # ---------------------------------------------------------------------------------------------------------------------
    # Fit definition
    # ---------------------------------------------------------------------------------------------------------------------

    "dataset_variables" : ["mass","leadPt","subleadPt"],     # list of variables to be put in the dataset (other than the template ones)
    "weight_expression" : "weight",                          # event weight expression  

    "preselection" : "1",          # preselection applied to all trees
    "selection"    : "cicNoChIso", # analysis selection, coming out of high_mass_analysis.py
    
    # List of aliases. Used to define variables/simplify expressions selection.
    "aliases"  : [ 
    	       "leadIsEB    := abs(leadScEta) < 1.5",
	       "subleadIsEB := abs(subleadScEta) < 1.5",
	       "leadIsSB    := leadIsEB    && (leadSigmaIeIe    > 0.012) || !leadIsEB    && (leadSigmaIeIe>0.03   )",
	       "subleadIsSB := subleadIsEB && (subleadSigmaIeIe > 0.012) || !subleadIsEB && (subleadSigmaIeIe>0.03)",
	       "leadPrompt    := leadGenIso < 10. && leadMatchType == 1",
	       "leadFake      := ! leadPrompt",
	       "subleadPrompt := subleadGenIso < 10. && subleadMatchType == 1",
	       "subleadFake   := ! subleadPrompt"
    ],

    
    # List of fits.
    # General form is "<fit-name>" : { <fit-config> }
    "fits" : {
        # 2D fit
	"2D" : {
	    "ndim"       : 2,                              # number of dimensions for the template
	    "bins"       : [ "mass", [500.0,1300.0] ],     # kinematic bins in which the fit is run
            "template_binning" : [ -1.0, 0.0, 1.0, 15.0 ], # template varaible binning 
	    "components" : [ "pp", "pf", "ff" ],           # list of fit components

	    # categories in which to split the fit
	    # format is <fit-category-name>: { <config> }
	    "categories" : {                               
		"EBEB" : { 
		       	 "src"  : "EBEB",       # dataset category (or group) to use as input.
			 # instruction for how to fill the templates
			 #  format is <condition> : [ <list-of-expressions> ]. the lenght of <list-of-expressions> has to be exactly ndim
			 "fill" : {  
			 	# Pseudo-random-swap. 
				#  For even events order is lead,sublead.
				#  For odd events sublead,lead
			 	"TMath::Even(Entry$)" :[ "leadBlockChIso", "subleadBlockChIso" ],
			     	"! TMath::Even(Entry$)" :[ "subleadBlockChIso", "leadBlockChIso" ]
			     }
		}, 
                "EBEE" : { "src" : "notEBEB" ,
			"fill" : { 
			# Here first photon is barrel, second is endcap
			"leadIsEB && ! subleadIsEB" :[ "leadBlockChIso", "subleadBlockChIso" ],
                        "! leadIsEB && subleadIsEB" :[ "subleadBlockChIso", "leadBlockChIso" ]
		       }
		} 
	    },
            
            # MC truth selection.
            #   This is applied to all fit categories in the mc dataset
	    "truth_selection" : {
		"pp" : "leadPrompt  && subleadPrompt",
		    "pf" : "(leadPrompt && subleadFake) || (subleadPrompt && leadFake)",
		    "ff" : "leadFake    && subleadFake"
	    },
	    
	    # Template creation.
	    #   Format is <component> : { <settings> }
	    "templates" : {
		"pp" : { "sel"  : "cicNoChIso", # event selection
		         "dataset" : "data",    # input dataset
			# How to fill different categories. Logic is the same as for the 'fill' instructions above
			"fill_categories" : {
			        # same logic as above, but with random cones.
			        "EBEB" :{ "1" : [ "leadRndConeChIso", "subleadRndConeChIso" ] 
				},
				"EBEE" :{ "leadIsEB && ! subleadIsEB" : [ "leadRndConeChIso", "subleadRndConeChIso" ], 
				          "! leadIsEB && subleadIsEB" : [ "subleadRndConeChIso", "leadRndConeChIso" ]
				}
			    }
		},
		"pf" : { "sel"  : "cicNoChIsoSingleSB",	
			"fill_categories" : { 
			# pseudo-random swap, but taking care to take the random cone isolation for the signal-region photon
			#   and the photon isolation for the side-band one
			"EBEB" :{ "leadIsSB && TMath::Even(Entry$)" :    [ "leadBlockChIso", "subleadRndConeChIso" ],
				"leadIsSB && ! TMath::Even(Entry$)" :    [ "subleadRndConeChIso", "leadBlockChIso" ],
				"subleadIsSB && ! TMath::Even(Entry$)" : [ "subleadBlockChIso", "leadRndConeChIso" ],
				"subleadIsSB && ! TMath::Even(Entry$)" : [ "leadRndConeChIso", "subleadBlockChIso" ]
			},
			"EBEE" :{ "leadIsEB && ! subleadIsEB &&      leadIsSB" : [ "leadRndConeChIso", "subleadBlockChIso" ], 
				"leadIsEB && ! subleadIsEB &&    ! leadIsSB" : [ "leadBlockChIso", "subleadRndConeChIso" ], 
				"! leadIsEB && subleadIsEB &&   subleadIsSB" : [ "subleadBlockChIso", "leadRndConeChIso" ], 
				"! leadIsEB && subleadIsEB && ! subleadIsSB" : [ "subleadBlockChIso", "leadRndConeChIso" ]
				}
			}
		 },
		 "ff" : { "sel"  : "cicNoChIsoDoubleSB",	
			 "fill_categories" : {
			 "EBEB" :{ "TMath::Even(Entry$)" : [ "leadBlockChIso", "subleadBlockChIso" ],
				 "! TMath::Even(Entry$)" : [ "subleadBlockChIso", "leadBlockChIso" ]
				 },
			 "EBEE" :{ "leadIsEB && ! subleadIsEB" : [ "leadBlockChIso", "subleadBlockChIso" ],
				 "! leadIsEB && subleadIsEB" : [ "subleadBlockChIso", "leadBlockChIso" ]
				 }
			    }
			}
		}
	},
	# 1D fit: same as above, with (possible) additional loop over legs
	"1D" : {
	    "ndim"       : 1,
	    "bins"       : [ "mass", [500.0,1300.0] ],
            "template_binning" : [ -1.0, 0.0, 1.0, 15.0 ],
	    "components" : ["p", "f"],
	    # loop over legs defined here
	    #   %(leg)s replaced with these values
            "legs" : ["lead","sublead"],
	    "categories" : { 
		"EBEB" : { 
		    "src" : "EBEB",
			"fill" : { "1" : ["%(leg)sBlockChIso"] }
		}, 
		"EBEE" : {
    		    "src" : "notEBEB",
			"fill" : { "%(leg)sIsEB" : ["%(leg)sBlockChIso"] }

		},
		"EEEB" : {
    		    "src" : "notEBEB",
			"fill" : { "! %(leg)sIsEB" : ["%(leg)sBlockChIso"] }

		}
	    },
	    "truth_selection" : {
		"p" : "%(leg)sPrompt",
	        "f" : "%(leg)sFake"
	    },
	    "templates" : {
		"p" : {
		    "sel" : "cicNoChIso",
		    "dataset" : "data",
			"fill_categories" : { 
			    "EBEB" : { "1" : ["%(leg)sRndConeChIso"] },
			    "EBEE" : { "%(leg)sIsEB" : ["%(leg)sRndConeChIso"] },
			    "EEEB" : { "! %(leg)sIsEB" : ["%(leg)sRndConeChIso"] }
		    }
		},
		"f" : { 
		    "sel" : "cicNoChIsoSingleSB",
			"fill_categories" : { 
			"EBEB" : { "%(leg)sIsSB" : ["%(leg)sBlockChIso"] },
			"EBEE" : { "%(leg)sIsSB && %(leg)sIsEB" : ["%(leg)sBlockChIso"] },
			"EEEB" : { "%(leg)sIsSB && ! %(leg)sIsEB" : ["%(leg)sBlockChIso"] }
		    }
		}
	    }
	}
    }
    
}
```

