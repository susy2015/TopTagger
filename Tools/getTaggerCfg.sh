#!/bin/bash

GITHUB_SUSY2015_URL=https://github.com/susy2015
REPO_NAME=TopTaggerCfg

CFG_DIRECTORY=$PWD
TAG=
NO_SOFTLINK=

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

TOP_CFG_NAME=TopTagger.cfg

function print_help {
    echo "Usage:"
    echo "getTaggerCfg.sh -t RELEASE_TAG [-d checkout_directory] [-f cfg_filename] [-n]"
    echo ""
    echo "Description: This script automatically downloads the top tagger configuration file and MVA training file (if necessary)"
    echo "And produces a softlink to this files in your corrent directory.  This script should be run from the directory where"
    echo "the tagger code will be run from.  Tagger configuration releases can be browsed here"
    echo "https://github.com/susy2015/TopTaggerCfg/releases."
    echo ""
    echo "-t RELEASE_TAG :         This is the github release tag to check out (required option)"
    echo "-d checkout_directory :  This is the directory where the configuration files will be downloaded to (default: .)"
    echo "-f cfg_filename :        Specify this option to name the softlink to the cfg file something other than \"TopTagger.cfg\""
    echo "-n :                     Download files without producing softlinks"
}


# Initialize our own variables:

while getopts "h?d:f:t:n" opt; do
    case "$opt" in
    h|\?)
        print_help
        exit 0
        ;;
    d)  CFG_DIRECTORY=$OPTARG
        ;;
    f)  TOP_CFG_NAME=$OPTARG
        ;;
    t)  TAG=$OPTARG
        ;;
    n) NO_SOFTLINK=NO
        ;;
    esac
done

shift $((OPTIND-1))

[ "$1" = "--" ] && shift

if [[ -z $TAG ]]
then
    print_help
    exit 0
fi

STARTING_DIR=$PWD

if [ ! -d $CFG_DIRECTORY ]
then
    echo $CFG_DIRECTORY " Is not a valid directory!"
    exit 0
fi

cd $CFG_DIRECTORY

if [ ! -d $REPO_NAME-$TAG ]
then
    echo "Checking out tag: " $TAG
    wget $GITHUB_SUSY2015_URL/$REPO_NAME/archive/$TAG.tar.gz
    if [ -f $TAG.tar.gz ]
    then
        tar xzf $TAG.tar.gz
        rm $TAG.tar.gz
    else
        echo "Failed to get " $TAG.tar.gz
        exit 0
    fi
else
    echo "Directory "$REPO_NAME-$TAG" already present"
fi

cd $REPO_NAME-$TAG
DOWNLOAD_DIR=$PWD

MVAFILE=

if [ -f TopTagger.cfg ]
then
    MVAFILE=$(grep "modelFile" TopTagger.cfg | sed 's/[^"]*"\([^"]*\)"/\1/')
    if [[ ! -z ${MVAFILE// } ]]
    then
        MVATARBALL=${MVAFILE%.*}.tar.gz
        echo $MVAFILE
        if [ ! -f $MVAFILE ]
        then
            wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL
            if [ -f $MVATARBALL ]
            then
                tar xzf $MVATARBALL
                rm $MVATARBALL
            else
                echo "File "$MVATARBALL" failed to download"
                exit 0
            fi
        else
            echo "Model file already present "$MVAFILE
        fi
    fi
fi

cd $STARTING_DIR

if [[ -z $NO_SOFTLINK ]]
then
    ln -s $DOWNLOAD_DIR/TopTagger.cfg $TOP_CFG_NAME
    if [[ ! -z ${MVAFILE// } ]] 
    then
        ln -s $DOWNLOAD_DIR/$MVAFILE $MVAFILE
    fi
fi
