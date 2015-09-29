# Repository for high mass diphoton analysis.

## Checking out the code

This package depends on the [flashgg](https://github.com/cms-analysis/flashgg).

A quick set of commands to check out the code is below. 
They assume that you have set your user github name in your git config and that you have 
forked the flashgg and this repository. 

# Spring15 settings
PROJECT_AREA=EXO_7_4_12
CMSSW_VERSION=CMSSW_7_4_12


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
git checkout -b topic_diphotons_7412

cd ${CMSSW_BASE}/src
bash flashgg/setup.sh 

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
