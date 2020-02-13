#!/bin/bash

CMSSW_VERSION=CMSSW_10_2_9

#stop upon failed command
set -ex

TRAVIS_BUILD_DIR=$1
echo "UID" $(id -u)
echo "GID" $(id -g)
pwd
ls -la
cd $TRAVIS_BUILD_DIR/..
pwd
ls -la
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch                                        
export SCRAM_ARCH=slc6_amd64_gcc630
export CMSSW_GIT_REFERENCE=/cvmfs/cms.cern.ch/cmssw.git/                       
source $VO_CMS_SW_DIR/cmsset_default.sh 
scramv1 project CMSSW $CMSSW_VERSION
cd $CMSSW_VERSION/src/
#suppress huge printout from "cmsenv"
set +x
echo "============================"
echo "SETTING UP CMSSW ENVIRONMENT"
echo "============================"
eval `scramv1 runtime -sh`
set -x
cp -r $TRAVIS_BUILD_DIR .
cd TopTagger/TopTagger/test
echo "========================================================================="
./configure
make -j
echo "========================================================================="
source taggerSetup.sh
getTaggerCfg.sh -t DeepCombined_Example_v1.0.2
./topTaggerTest
echo "========================================================================="
cd ../python
python TopTagger.py -e -f ../test/exampleInputs.root -b slimmedTuple -w ../test
echo "========================================================================="
cd ../..
scram b -j
