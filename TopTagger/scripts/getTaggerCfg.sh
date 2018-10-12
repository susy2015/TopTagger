#!/bin/bash

# getTaggerCfg.sh

GITHUB_SUSY2015_URL=https://github.com/susy2015
REPO_NAME=TopTaggerCfg

STARTING_DIR=$PWD
CFG_DIRECTORY=$PWD
TAG=
NO_SOFTLINK=
OVERWRITE=

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

TOP_CFG_NAME=TopTagger.cfg

function print_help {
    echo ""
    echo "Usage:"
    echo "    getTaggerCfg.sh -t RELEASE_TAG [-d checkout_directory] [-f cfg_filename] [-n]"
    echo ""
    echo "Options:"
    echo "    -t RELEASE_TAG :         This is the github release tag to check out (required option)"
    echo "    -d checkout_directory :  This is the directory where the configuration files will be downloaded to (default: .)"
    echo "    -f cfg_filename :        Specify this option to name the softlink to the cfg file something other than \"TopTagger.cfg\""
    echo "    -o :                     Overwrite the softlinks if they already exist"
    echo "    -n :                     Download files without producing softlinks"
    echo ""
    echo "Description:"
    echo "    This script automatically downloads the top tagger configuration file and MVA training file (if necessary)"
    echo "    and produces a softlink to this file in your corrent directory.  This script should be run from the directory where"
    echo "    the tagger code will be run from.  Tagger configuration releases can be browsed at"
    echo "    https://github.com/susy2015/TopTaggerCfg/releases."
    echo ""
}


# Initialize our own variables:

while getopts "h?d:f:t:no" opt; do
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
    o) OVERWRITE="-f"
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

echo " - Running getTaggerCfg.sh"

# get source directory of bash script
# used for "Easter Egg"...
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

# check if OVERWRITE is set
# if OVERWRITE is set, ask user for confirmation before continuing
if [[ -z $OVERWRITE ]]
then
    # OVERWRITE is not set
    # continue
    echo "INFO: OVERWRITE is not set. Existing files and softlinks will not be replaced."
    echo "  To replace existing softlinks and files, use the OVERWRITE option -o."
else
    # OVERWRITE is set
    # ask user for confirmation before continuing
    echo   "INFO: OVERWRITE is set. Existing files and softlinks will be replaced."
    echo   "  Would you like to continue and replace existing files?"
    printf "  Enter (Y/y/yes/si/oui/ja/da) to continue, and anything else to quit: "
    read answer
    if [[ $answer == "ok" ]]
    then
        # "Easter Egg"...
        cat $SCRIPTDIR/ok.txt
        exit 0
    fi
    if [[ $answer == "Y" || $answer == "y" || $answer == "yes" || $answer == "si" || $answer == "oui" || $answer == "ja" || $answer == "da" ]]
    then
        echo " - Continuing..."
    else
        echo " - Quitting..."
        exit 0
    fi
fi


# Check that CFG_DIRECTORY is a directory
if [ ! -d $CFG_DIRECTORY ]
then
    echo $CFG_DIRECTORY " Is not a valid directory!"
    exit 1
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

echo "INFO: DOWNLOAD_DIR is $DOWNLOAD_DIR"

MVAFILES=

if [ -f TopTagger.cfg ]
then
    MVAFILES=$(grep "modelFile" TopTagger.cfg | sed 's/[^"]*"\([^"]*\)"/\1/')
    MISSING=
    if [[ ! -z ${MVAFILES// } ]]
    then
        for MVAFILE in $MVAFILES; do
            if [ ! -f $MVAFILE ]
            then
                MISSING="yes"
                break
            fi
        done
        if [[ ! -z ${MISSING// } ]]
        then
            MVATARBALL=MVAFILES.tar.gz
            wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL
            if [ ! -f $MVATARBALL ]
            then
                echo "MVA tarball "$MVATARBALL" not found!!!"
                MVATARBALL=${MVAFILES%.*}.tar.gz
                echo "trying "$MVATARBALL
                wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL
                if [ ! -f $MVATARBALL ]
                then
                    echo "MVA tarball "$MVATARBALL" not found!!!"
                    exit 0
                fi
            fi
            tar xzf $MVATARBALL
            rm $MVATARBALL
        fi
    fi
fi

cd $STARTING_DIR

echo "INFO: STARTING_DIR is $STARTING_DIR"

# If OVERWRITE is set, make solftlinks (using ln) with -f
# If OVERWRITE is not set, make solftlinks (using ln)
# Pipe output to /dev/null

# Note: "> /dev/null 2>&1" does this:
# stdin  ==> fd 0      (default fd 0)
# stdout ==> /dev/null (default fd 1)
# stderr ==> stdout    (default fd 2)

# [[ -z STRING ]] : True if the length of "STRING" is zero, False if "STRING" has nonzero length
if [[ -z $NO_SOFTLINK ]]
then
    ln $OVERWRITE -s $DOWNLOAD_DIR/TopTagger.cfg $TOP_CFG_NAME > /dev/null 2>&1
    if [[ ! -z ${MVAFILES// } ]] 
    then
        for MVAFILE in $MVAFILES; do
            ln $OVERWRITE -s $DOWNLOAD_DIR/$MVAFILE $MVAFILE > /dev/null 2>&1
        done
    fi
fi


