#!/bin/bash

TRAVIS_BUILD_DIR=$1
echo $TRAVIS_BUILD_DIR
cd $TRAVIS_BUILD_DIR/..
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch                                        
export SCRAM_ARCH=slc6_amd64_gcc630
export CMSSW_GIT_REFERENCE=/cvmfs/cms.cern.ch/cmssw.git/                       
source $VO_CMS_SW_DIR/cmsset_default.sh 
scramv1 project CMSSW CMSSW_9_3_3
cd CMSSW_9_3_3/src/
pwd
eval `scramv1 runtime -sh`
mv $TRAVIS_BUILD_DIR .
cd TopTagger/test
scram tool info py2-numpy
scram tool tag py2-numpy PY2_NUMPY_BASE
./configure
make -j
source taggerSetup.sh
$TRAVIS_BUILD_DIR/TopTagger/Tools/getTaggerCfg.sh -t Intermediate_Example_v1.0.0
./topTaggerTest
cd ../..
scram b -j