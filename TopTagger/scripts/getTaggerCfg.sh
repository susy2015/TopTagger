#!/bin/bash

# getTaggerCfg.sh

# Required to run this script:
# - release tag from TopTaggerCfg repo that you wish to checkout

GITHUB_SUSY2015_URL=https://github.com/susy2015
REPO_NAME=TopTaggerCfg
RELEASE_URL="$GITHUB_SUSY2015_URL/$REPO_NAME/releases"

STARTING_DIR=$PWD
CFG_DIRECTORY=$PWD
CHECKOUT_DIRECTORY=$PWD
TAG=
NO_SOFTLINK=
OVERWRITE=
VERBOSE=

# A POSIX variable
OPTIND=1    # Reset in case getopts has been used previously in the shell.

TOP_CFG_NAME=TopTagger.cfg

function print_help {
    echo ""
    echo "Usage:"
    echo "    getTaggerCfg.sh -t RELEASE_TAG [-d checkout_directory] [-f cfg_filename] [-o] [-n] [-v]"
    echo ""
    echo "Options:"
    echo "    -t RELEASE_TAG :         This is the github release tag to check out (required option)"
    echo "    -d checkout_directory :  This is the directory where the configuration files will be downloaded to (default: .)"
    echo "    -f cfg_filename :        Specify this option to name the softlink to the cfg file something other than \"TopTagger.cfg\""
    echo "    -o :                     Overwrite the softlinks if they already exist"
    echo "    -l checkout location :   Location to check out tagger cfg files (default: .)"
    echo "    -n :                     Download files without producing softlinks"
    echo "    -v :                     increase verbosity: print more stuff... for those who like stuff"
    echo ""
    echo "Description:"
    echo "    This script automatically downloads the top tagger configuration file and MVA training files (if necessary)"
    echo "    and produces a softlink to this file in your corrent directory.  This script should be run from the directory where"
    echo "    the tagger code will be run from.  Tagger configuration releases can be browsed at"
    echo "    $RELEASE_URL"
    echo ""
}

function print_ok {
    echo "  ______    __  ___" 
    echo " /  __  \\  |  |/  /" 
    echo "|  |  |  | |  '  / " 
    echo "|  |  |  | |    <  " 
    echo "|  \`--'  | |  .  \\ " 
    echo " \\______/  |__|\\__\\" 
    echo ""
}

# Initialize our own variables:

while getopts "h?d:f:t:l:nov" opt; do
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
    l)  CHECKOUT_DIRECTORY=$OPTARG
        ;;
    o) OVERWRITE="-f"
        ;;
    n) NO_SOFTLINK=NO
        ;;
    v) VERBOSE=1
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
    # check if softlink exists
    if [[ -L $TOP_CFG_NAME ]]
    then
        echo "INFO: OVERWRITE is not set. Existing softlinks will not be replaced."
        printf "  Enter \"o\" to overwrite existing softlinks, and anything else to continue without overwriting: "
        read answer
        if [[ $answer == "ok" ]]
        then
            # "Easter Egg"...
            print_ok
        elif [[ $answer == "o" ]]
        then
            OVERWRITE="-f"
        fi
    fi
else
    # OVERWRITE is set
    # check if file exists and is not a softlink
    if [[ -f $TOP_CFG_NAME && ! -L $TOP_CFG_NAME ]]
    then
        # ask user for confirmation before continuing
        echo   "INFO: OVERWRITE is set. Existing files will be replaced."
        printf "  Enter (Y/y/yes/si/oui/ja/da) to replace existing files, and anything else to quit: "
        read answer
        if [[ $answer == "ok" ]]
        then
            # "Easter Egg"...
            print_ok
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
    echo " - Downloading this REPO-TAG: $REPO_NAME-$TAG"
    if [[ ! -z $VERBOSE ]] # True if VERBOSE is set
    then
        wget $GITHUB_SUSY2015_URL/$REPO_NAME/archive/$TAG.tar.gz
    else
        wget $GITHUB_SUSY2015_URL/$REPO_NAME/archive/$TAG.tar.gz &> /dev/null
    fi
    if [ -f $TAG.tar.gz ]
    then
        tar xzf $TAG.tar.gz
        rm $TAG.tar.gz
    else
        echo "ERROR: Failed to download $GITHUB_SUSY2015_URL/$REPO_NAME/archive/$TAG.tar.gz"
        echo "  Check that the REPO-TAG that you entered ($REPO_NAME-$TAG)"
        echo "  exists at $RELEASE_URL"
        echo "  Check your spelling... you may have a typo! Copy and paste are your friends."
        exit 1
    fi
else
    echo " - Skipping the download of the requested REPO-TAG because the directory "$REPO_NAME-$TAG" is already present"
fi


cd $REPO_NAME-$TAG
DOWNLOAD_DIR=$PWD

if [[ ! -z $VERBOSE ]] # True if VERBOSE is set
then
    echo "INFO: DOWNLOAD_DIR is $DOWNLOAD_DIR"
fi

MVAFILES=

if [ -f TopTagger.cfg ]
then
    MVAFILES=$(grep -e "modelFile" -e "inputFile"  TopTagger.cfg | sed 's/[^"]*"\([^"]*\)"/\1/')
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
            echo " - Downloading MVA files"
            MVATARBALL=MVAFILES.tar.gz
            if [[ ! -z $VERBOSE ]] # True if VERBOSE is set
            then
                wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL
            else
                wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL &> /dev/null
            fi
            if [ ! -f $MVATARBALL ]
            then
                echo "ERROR: MVA tarball "$MVATARBALL" not found!!!"
                MVATARBALL=${MVAFILES%.*}.tar.gz
                echo "Now trying $MVATARBALL instead"
                if [[ ! -z $VERBOSE ]] # True if VERBOSE is set
                then
                    wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL
                else
                    wget $GITHUB_SUSY2015_URL/$REPO_NAME/releases/download/$TAG/$MVATARBALL &> /dev/null
                fi
                if [ ! -f $MVATARBALL ]
                then
                    echo "ERROR: MVA tarball "$MVATARBALL" not found!!!"
                    exit 1
                fi
            fi
            tar xzf $MVATARBALL
            rm $MVATARBALL
        fi
    fi
fi

# make all files in DOWNLOAD_DIR read only
# a (all) = ugo (user group others)
chmod a-w *

cd $STARTING_DIR

if [[ ! -z $VERBOSE ]] # True if VERBOSE is set
then
    echo "INFO: STARTING_DIR is $STARTING_DIR"
fi

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
    # create softlinks
    ln $OVERWRITE -s $DOWNLOAD_DIR/TopTagger.cfg $CHECKOUT_DIRECTORY/$TOP_CFG_NAME > /dev/null 2>&1 && echo " - Created softlinks to $REPO_NAME config file"
    if [[ ! -z ${MVAFILES// } ]] 
    then
        for MVAFILE in $MVAFILES; do
            ln $OVERWRITE -s $DOWNLOAD_DIR/$MVAFILE $CHECKOUT_DIRECTORY/$MVAFILE > /dev/null 2>&1 && echo " - Created softlinks to $REPO_NAME MVA files"
        done
    fi
fi

