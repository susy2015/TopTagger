#!/bin/bash

#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/DYJetsToLL_M-50_13TeV-madgraph-pythia8/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_DYJetsToLL.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/DYJetsToLL_M-50_HT-100to200_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_DYJetsToLL_HT_100to200.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/DYJetsToLL_M-50_HT-200to400_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_DYJetsToLL_HT_200to400.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/DYJetsToLL_M-50_HT-400to600_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_DYJetsToLL_HT_400to600.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/DYJetsToLL_M-50_HT-600toInf_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_DYJetsToLL_HT_600toInf.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/ZJetsToNuNu_HT-100to200_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_ZJetsToNuNu_HT_100to200.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/ZJetsToNuNu_HT-200to400_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_ZJetsToNuNu_HT_200to400.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/ZJetsToNuNu_HT-400to600_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_ZJetsToNuNu_HT_400to600.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/ZJetsToNuNu_HT-600toInf_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/0000/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_ZJetsToNuNu_HT_600toInf.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/TTJets_MSDecaysCKM_central_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/*/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_TTJets.txt
#ls -p -v /eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2/pastika/SMS-T2tt_2J_mStop-850_mLSP-100_Tune4C_13TeV-madgraph-tauola/PHYS14_PU20bx25_PHYS14_25_V1-FLAT/*/*/*.root | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > rootlist_T2tt_mStop850_mLSP100.txt

#baseDir=/eos/uscms/store/user/lpcsusyhad/PHYS14_720_Mar14_2014_v2
#baseDir=/eos/uscms/store/user/lpcsusyhad/PHYS14_720_March30_2015/
#baseDir=/eos/uscms/store/user/pastika/PHYS14_720_May19_2015_v1/
baseDir=$1

cd $baseDir

#for i in $(ls $baseDir); do
#    if [ -d $baseDir/$i ]; then
        for fn in $(ls $baseDir); do
            if [ -d $baseDir/$fn ]; then
                find $baseDir/$i/$fn/ -name "*.root" | grep -v "failed" | sed 's|/eos/uscms|root://cmsxrootd-site.fnal.gov/|' > $fn.txt
            fi
        done
#    fi
#done