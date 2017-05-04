AC_INIT(program, 1.0)

dnl Switch to a C++ compiler, and check if it works.
AC_LANG(C++)
AC_PROG_CXX

dnl check for c++11

dnl set --std=c++11
if test -z ${CXXFLAGS+x}; then
   CXXFLAGS="--std=c++11"
else
   CXXFLAGS="--std=c++11 "$CXXFLAGS
fi

dnl check for root
AC_CHECK_PROG(rootfound, root-config, yes, no)

if test $rootfound == no; then
   AC_ERROR([ROOT not found.])
fi

dnl check for openCV
if test -z ${OPENCVDIR+x}; then
   OPENCVDIR=/usr/local/
fi

AC_CHECK_HEADERS($OPENCVDIR/include/opencv/cv.h, [], [AC_ERROR([OpenCV includes are required])])
AC_CHECK_HEADERS($OPENCVDIR/include/opencv/ml.h, [], [AC_ERROR([OpenCV includes are required])])

AC_SUBST(OPENCVDIR, $OPENCVDIR)

dnl check for top tagger code
if test -z ${TOPTAGGERDIR+x}; then
   TOPTAGGERDIR="${PWD}/../../"
fi

AC_CHECK_HEADER($TOPTAGGERDIR/TopTagger/include/TopTagger.h, [], [AC_ERROR([TopTagger includes are not found!!!!!!!!!!])])
AC_CHECK_HEADERS($TOPTAGGERDIR/CfgParser/include/TTException.h, [], [AC_ERROR([CfgParser includes are not found!!!!!!!!!!])])

AC_SUBST(TAGGERDIR, $TOPTAGGERDIR)

dnl AC_SEARCH_LIBS(cv::ml::RTrees::load, opencv, [], [AC_ERROR([NO openCV library found])])

echo $LIBS

dnl Process Makefile.in to create Makefile
AC_OUTPUT(Makefile)
