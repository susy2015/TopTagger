AC_INIT(program, 1.0)

dnl Switch to a C++ compiler, and check if it works.
AC_LANG(C++)
AC_PROG_CXX

dnl set --std=c++11
case "$CXXFLAGS" in 
  *"--std=c++11"*)
    # Do nothing
    ;;
  *)
    CXXFLAGS="$CXXFLAGS --std=c++11" 
esac

dnl check for ldconfig
AC_CHECK_PROG(ldconfigfound, ldconfig, yes, no)

LDCONFIG=
if test ${ldconfigfound} == yes; then
   LDCONFIG="ldconfig"
fi

AC_SUBST(LDCONFIG, $LDCONFIG)

dnl check if this is a mac
LIBSUFFIX="so"
OSNAME=$(uname -s)
if [[ "$OSNAME" == "Darwin" ]]; then
   LIBSUFFIX="dylib"
fi

AC_SUBST(LIBSUFFIX, $LIBSUFFIX)

dnl check for root
AC_CHECK_PROG(rootconfigfound, root-config, yes, no)
AC_CHECK_PROG(rootcintfound, rootcint, yes, no)

if test ${rootconfigfound} == no; then
   AC_ERROR([ROOT: root-config not found.])
fi

if test ${rootcintfound} == no; then
   AC_ERROR([ROOT: rootcint not found.])
fi   

dnl check for openCV
if test -z ${OPENCVDIR+x}; then
   OPENCVDIR=/usr/local/
fi

OPENCVINCLUDECVH=$OPENCVDIR/include/opencv/cv.h
OPENCVINCLUDEMLH=$OPENCVDIR/include/opencv/ml.h
AC_CHECK_FILE($OPENCVINCLUDECVH, [], [])
AC_CHECK_FILE($OPENCVINCLUDEMLH, [], [])

OPENCVLIBML=$OPENCVDIR/lib/libopencv_ml.${LIBSUFFIX}
OPENCVLIBCV=$OPENCVDIR/lib/libopencv_core.${LIBSUFFIX}
AC_CHECK_FILE($OPENCVLIBCV, [], [])
AC_CHECK_FILE($OPENCVLIBML, [], [])

OPENCVINCLUDECVH=ac_cv_file_$(echo ${OPENCVINCLUDECVH} | sed -e 's/[\/]/_/g' -e 's/\./_/')
OPENCVINCLUDEMLH=ac_cv_file_$(echo ${OPENCVINCLUDEMLH} | sed -e 's/[\/]/_/g' -e 's/\./_/')
OPENCVLIBCV=ac_cv_file_$(echo ${OPENCVLIBCV} | sed -e 's/[\/]/_/g' -e 's/\./_/')
OPENCVLIBML=ac_cv_file_$(echo ${OPENCVLIBML} | sed -e 's/[\/]/_/g' -e 's/\./_/')

dnl configure OPENCV if it is found
if test "${!OPENCVINCLUDECVH}" = "no" || test "${!OPENCVINCLUDEMLH}" = "no" || test "${!OPENCVLIBCV}" = "no" || test "${!OPENCVLIBML}" = "no"; then
   AC_WARN([OpenCV includes or libraries are not found. Specify location with "./configure OPENCVDIR=/Path" if required.])
   AC_SUBST(OPENCVDIR, "")
else
   AC_SUBST(OPENCVDIR, $OPENCVDIR)
fi


dnl check for xgboost
if test -z ${XGBOOSTDIR+x}; then
   XGBOOSTDIR=/usr/local/
fi

XGBOOSTINCLUDEXGBCAPIH=$XGBOOSTDIR/include/xgboost/c_api.h
XGBOOSTINCLUDERABCAPIH=$XGBOOSTDIR/rabit/include/rabit/c_api.h
AC_CHECK_FILE($XGBOOSTINCLUDEXGBCAPIH, [], [])
AC_CHECK_FILE($XGBOOSTINCLUDERABCAPIH, [], [])

XGBOOSTLIBXGB=$XGBOOSTDIR/lib/libxgboost.${LIBSUFFIX}
AC_CHECK_FILE($XGBOOSTLIBXGB, [], [])

XGBOOSTINCLUDEXGBCAPIH=ac_cv_file_$(echo ${XGBOOSTINCLUDEXGBCAPIH} | sed -e 's/[\/]/_/g' -e 's/\./_/')
XGBOOSTINCLUDERABCAPIH=ac_cv_file_$(echo ${XGBOOSTINCLUDERABCAPIH} | sed -e 's/[\/]/_/g' -e 's/\./_/')
XGBOOSTLIBXGB=ac_cv_file_$(echo ${XGBOOSTLIBXGB} | sed -e 's/[\/]/_/g' -e 's/\./_/')

dnl configure OPENCV if it is found
if test "${!XGBOOSTINCLUDEXGBCAPIH}" = "no" || test "${!XGBOOSTINCLUDERABCAPIH}" = "no" || test "${!XGBOOSTLIBXGB}" = "no"; then
   AC_WARN([XGBoost includes or libraries are not found. Specify location with "./configure XGBOOSTDIR=/Path" if required.])
   AC_SUBST(XGBOOSTDIR, "")
else
   AC_SUBST(XGBOOSTDIR, $XGBOOSTDIR)
fi

dnl check for top tagger code
if test -z ${TOPTAGGERDIR+x}; then
   TOPTAGGERDIR="${PWD}/../../"
fi

AC_CHECK_HEADER($TOPTAGGERDIR/TopTagger/include/TopTagger.h,        [], [AC_ERROR([TopTagger includes are not found!!!!!!!!!!])])
AC_CHECK_HEADERS($TOPTAGGERDIR/CfgParser/include/TTException.h,     [], [AC_ERROR([CfgParser includes are not found!!!!!!!!!!])])

AC_SUBST(TAGGERDIR, $TOPTAGGERDIR)

dnl Process Makefile.in to create Makefile
AC_OUTPUT(Makefile)

dnl Process taggerSetup.sh.in to create taggerSetup.sh
AC_OUTPUT(taggerSetup.sh)

dnl Process taggerSetup.csh.in to create taggerSetup.csh
AC_OUTPUT(taggerSetup.csh)
