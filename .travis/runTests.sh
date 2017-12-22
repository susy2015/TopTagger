#!/bin/bash

TRAVIS_BUILD_DIR=$1
echo $TRAVIS_BUILD_DIR
cd $TRAVIS_BUILD_DIR
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch                                        
export SCRAM_ARCH=slc6_amd64_gcc630
export CMSSW_GIT_REFERENCE=/cvmfs/cms.cern.ch/cmssw.git/                       
source $VO_CMS_SW_DIR/cmsset_default.sh 
scramv1 project CMSSW CMSSW_9_3_3
cd CMSSW_9_3_3/src/
eval `scramv1 runtime -sh`
cd $TRAVIS_BUILD_DIR/TopTagger/test
which g++
echo "int main() {}" > test.cpp
g++ test.cpp
./configure
make -j
source taggerSetup.sh
$TRAVIS_BUILD_DIR/TopTagger/Tools/getTaggerCfg.sh -t Intermediate_Example_v1.0.0
./topTaggerTest
