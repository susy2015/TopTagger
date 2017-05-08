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

AC_CHECK_HEADERS($OPENCVDIR/include/opencv/cv.h, [], [AC_ERROR([OpenCV includes are required])])
AC_CHECK_HEADERS($OPENCVDIR/include/opencv/ml.h, [], [AC_ERROR([OpenCV includes are required])])

AC_CHECK_FILE($OPENCVDIR/lib/libopencv_ml.so, [], [AC_ERROR([OpenCV lobraries are required])])
AC_CHECK_FILE($OPENCVDIR/lib/libopencv_core.so, [], [AC_ERROR([OpenCV lobraries are required])])

AC_SUBST(OPENCVDIR, $OPENCVDIR)

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
