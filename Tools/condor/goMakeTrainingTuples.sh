#!/bin/bash

export PATH=${PATH}:/cvmfs/cms.cern.ch/common
export CMS_PATH=/cvmfs/cms.cern.ch

#get the release setup and in place
tar -xzf $2.tar.gz
cd $2/
mkdir -p src
cd src
scram b ProjectRename
eval `scramv1 runtime -sh`

#set up local code
tar -xzf ${_CONDOR_SCRATCH_DIR}/gtp.tar.gz
cd WORLDSWORSESOLUTIONTOAPROBLEM

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PWD}

echo "xrdcp root://cmseos.fnal.gov/$(echo $6 | sed 's|/eos/uscms||') ."
xrdcp root://cmseos.fnal.gov/$(echo $6 | sed 's|/eos/uscms||') .

ls -lhrt

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${_CONDOR_SCRATCH_DIR}

./makeTrainingTuples --condor -D $1 -N $3 -M $4 -R "10:1:1"

ls -lhrt

#mv trainingTuple_* ${_CONDOR_SCRATCH_DIR}
for i in trainingTuple_*; do
    echo xrdcp $i root://cmseos.fnal.gov//store/user/cmadrid/trainingTuples/BackGroundMVA_V1/$i
    xrdcp $i root://cmseos.fnal.gov//store/user/cmadrid/trainingTuples/BackGroundMVA_V1/$i
    rm $i
done

rm $(echo $6 | sed 's|.*/||')
rm -r ${_CONDOR_SCRATCH_DIR}/$2

