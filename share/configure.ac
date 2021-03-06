AC_INIT([LibBi], [1.0.1], [bug-report@libbi.org], [libbi], [http://www.libbi.org])
AC_PREREQ([2.62])
AC_LANG([C++])
AM_INIT_AUTOMAKE([1.10 no-define foreign])
#AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Save user's flags
USER_CPPFLAGS=$CPPFLAGS
USER_CXXFLAGS=$CXXFLAGS
USER_LDFLAGS=$LDFLAGS

# Conditionals used by make
AC_ARG_ENABLE([assert],
     [  --enable-assert         enable assertion checking],
     [case "${enableval}" in
       yes) assert=true ;;
       no)  assert=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-assert]) ;;
     esac],[assert=true])

AC_ARG_ENABLE([single],
     [  --enable-single         use single-precision floating point],
     [case "${enableval}" in
       yes) single=true ;;
       no)  single=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-single]) ;;
     esac],[single=false])

AC_ARG_ENABLE([cuda],
     [  --enable-cuda           use CUDA code for compatible GPU device],
     [case "${enableval}" in
       yes) cuda=true ;;
       no)  cuda=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cuda]) ;;
     esac],[cuda=false])

AC_ARG_ENABLE([sse],
     [  --enable-sse            use SSE code],
     [case "${enableval}" in
       yes) sse=true ;;
       no)  sse=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-sse]) ;;
     esac],[sse=false])

AC_ARG_ENABLE([mpi],
     [  --enable-mpi            use MPI code],
     [case "${enableval}" in
       yes) mpi=true ;;
       no)  mpi=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-mpi]) ;;
     esac],[mpi=false])

AC_ARG_ENABLE([vampir],
     [  --enable-vampir         use Vampir],
     [case "${enableval}" in
       yes) vampir=true ;;
       no)  vampir=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-vampir]) ;;
     esac],[vampir=false])
     
AC_ARG_ENABLE([extradebug],
     [  --enable-extradebug    use extra debugging options],
     [case "${enableval}" in
       yes) extradebug=true ;;
       no)  extradebug=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-extradebug]) ;;
     esac],[extradebug=false])
     
AC_ARG_ENABLE([timing],
     [  --enable-timing    output timing information during run],
     [case "${enableval}" in
       yes) timing=true ;;
       no)  timing=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-timing]) ;;
     esac],[timing=false])

AC_ARG_ENABLE([diagnostics],
     [  --enable-diagnostics    output diagnostics during run],
     [case "${enableval}" in
       yes) diagnostics=true ;;
       no)  diagnostics=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-diagnostics]) ;;
     esac],[diagnostics=false])

AC_ARG_ENABLE([gperftools],
     [  --enable-gperftools    enable gperftools profiler],
     [case "${enableval}" in
       yes) gperftools=true ;;
       no)  gperftools=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-gperftools]) ;;
     esac],[gperftools=false])
     
# Add standard CUDA directories
#if test x$cuda = xtrue; then
# ^ don't test, may be using Thrust bundled with CUDA, even for host
    CUDA_CPPFLAGS="-I/usr/local/cuda/include -I$CUDA_ROOT/include -DHAVE_CUBLAS" # -DHAVE_CUBLAS needed for MAGMA 1.4
    CUDA_CXXFLAGS=
    CUDA_LDFLAGS="-L/usr/local/cuda/lib64 -L/usr/local/cuda/lib -L$CUDA_ROOT/lib64 -L$CUDA_ROOT/lib"
    CPPFLAGS="$CPPFLAGS $CUDA_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CUDA_LDFLAGS"
#fi

# Defines
if test x$cuda = xtrue; then
  AC_DEFINE([THRUST_DEVICE_SYSTEM], [THRUST_DEVICE_SYSTEM_CUDA])
elif test x$openmp = xtrue; then
  AC_DEFINE([THRUST_DEVICE_SYSTEM], [THRUST_DEVICE_SYSTEM_OMP])
fi
#AC_DEFINE([THRUST_HOST_SYSTEM], [THRUST_HOST_SYSTEM_OMP])
#AC_DEFINE([THRUST_HOST_SYSTEM], [THRUST_HOST_SYSTEM_TBB])
#AC_DEFINE([THRUST_HOST_SYSTEM], [THRUST_HOST_SYSTEM_CPP])
# ^ OpenMP very slow as at Thrust 1.6

# Checks for programs.
AC_PROG_CXX([icpc g++])
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_RANLIB

if test x$CXX = xicpc; then
  intel=true
else
  intel=false
fi

# Compiler characteristics
if test x$opebmp = xtrue; then
  AC_OPENMP
fi

# Checks of programs
if test x$mpi = xtrue; then
    if test x$vampir = xtrue; then
	    AC_CHECK_PROG(have_mpicxxvt, [mpicxx-vt], [true], [false])
	    if test x$have_mpicxxvt = xfalse; then
	       AC_MSG_ERROR([program not found (required with --enable-vampir)])
	    fi
		CXX="mpicxx-vt --vt:cxx $CXX --vt:inst manual --vt:mpi"
		CXXCPP="mpicxx-vt --vt:cxx $CXXCPP --vt:mpi"
	else
	    AC_CHECK_PROG(have_mpicxx, [mpicxx], [true], [false])
	    if test x$have_mpicxx = xfalse; then
	       AC_MSG_ERROR([program not found (required with --enable-mpi)])
	    fi
		CXX="mpicxx"
		CXXCPP="mpicxx -E"
	fi
fi

# libtool initialisation
#LT_INIT

# Checks for libraries
AC_CHECK_LIB([m], [main], [], [AC_MSG_ERROR([required library not found])])
AC_CHECK_LIB([gfortran], [main], [], [AC_MSG_ERROR([required library not found])])

# Intel MKL if available, needing special treatment given multiple libs...
AC_SEARCH_LIBS([mkl_blas_dgemv], [mkl_intel mkl_intel_lp64], [], [], [-lmkl_sequential -lmkl_core])
AC_CHECK_LIB([mkl_sequential], [mkl_blas_dgemv], [], [], [-lmkl_core])
AC_CHECK_LIB([mkl_core], [mkl_blas_dgemv], [], [])

# atlas if available, may be required later
AC_CHECK_LIB([atlas], [main], [], [])

# ...otherwise search for alternatives
AC_SEARCH_LIBS([cblas_dgemv], [openblas openblaso cblas gslcblas], [], [AC_MSG_ERROR([required library not found])])
AC_SEARCH_LIBS([dgemv_], [openblas openblaso blas f77blas], [], [AC_MSG_ERROR([required library not found])])
AC_SEARCH_LIBS([dpotrf_], [openblas openblaso lapack], [], [AC_MSG_ERROR([required library not found])])

AC_CHECK_LIB([qrupdate], [dch1dn_], [], [AC_MSG_ERROR([required library not found])])
AC_CHECK_LIB([gsl], [main], [], [AC_MSG_ERROR([required library not found])])
AC_CHECK_LIB([netcdf], [main], [], [AC_MSG_ERROR([required library not found])])
AC_CHECK_LIB([profiler], [main], [], [])

if test x$cuda = xtrue; then
    AC_CHECK_LIB([cuda], [main], [], [])
    AC_CHECK_LIB([cudart], [main], [], [AC_MSG_ERROR([library not found (required with --enable-cuda)])])
    AC_CHECK_LIB([curand], [main], [], [AC_MSG_ERROR([library not found (required with --enable-cuda)])])
    AC_CHECK_LIB([cublas], [main], [], [AC_MSG_ERROR([library not found (required with --enable-cuda)])])
    AC_CHECK_LIB([magmablas], [magmablas_dgemv_MLU], [], []) # removed in MAGMA 1.4, so not required
    AC_CHECK_LIB([magma], [magma_dpotrf], [], [AC_MSG_ERROR([library not found (required with --enable-cuda)])])
fi

if test x$mpi = xtrue; then
    AC_CHECK_LIB([mpi], [main], [], [AC_MSG_ERROR([library not found (required with --enable-mpi)])])
    AC_CHECK_LIB([boost_mpi], [main], [], [AC_MSG_ERROR([library not found (required with --enable-mpi)])])
    AC_CHECK_LIB([boost_serialization], [main], [], [AC_MSG_ERROR([library not found (required with --enable-mpi)])])
fi

# Checks for library functions
m4_foreach_w([func], [pow sqrt exp log], [AC_CHECK_FUNC(func, [], \
    AC_MSG_ERROR([required math function not found]))])

# Checks for header files
AC_HEADER_ASSERT

AC_CHECK_HEADER([netcdf.h], [], \
    AC_MSG_ERROR([required NetCDF header not found]), [-])

AC_CHECK_HEADERS([mkl_cblas.h cblas.h gsl/gsl_cblas.h], [], [], [-])
if test x$ac_cv_header_mkl_cblas_h = xfalse && test x$ac_cv_header_cblas_h = xfalse && x$ac_cv_header_gsl_gsl_cblas_h = xfalse; then
    AC_MSG_ERROR([required header not found])
fi

AC_CHECK_HEADERS([\
    boost/mpl/if.hpp \
    boost/random/bernoulli_distribution.hpp \
    boost/random/gamma_distribution.hpp \
    boost/random/mersenne_twister.hpp \
    boost/random/normal_distribution.hpp \
    boost/random/uniform_int.hpp \
    boost/random/uniform_real.hpp \
    boost/random/variate_generator.hpp \
    boost/typeof/typeof.hpp \
    ], [], AC_MSG_ERROR([required boost header not found]), [-])

AC_CHECK_HEADERS([\
    thrust/adjacent_difference.h \
    thrust/binary_search.h \
    thrust/copy.h \
    thrust/device_ptr.h \
    thrust/distance.h \
    thrust/extrema.h \
    thrust/fill.h \
    thrust/for_each.h \
    thrust/functional.h \
    thrust/gather.h \
    thrust/inner_product.h \
    thrust/iterator/counting_iterator.h \
    thrust/iterator/detail/normal_iterator.h \
    thrust/iterator/discard_iterator.h \
    thrust/iterator/permutation_iterator.h \
    thrust/iterator/transform_iterator.h \
    thrust/iterator/zip_iterator.h \
    thrust/logical.h \
    thrust/reduce.h \
    thrust/scan.h \
    thrust/sequence.h \
    thrust/sort.h \
    thrust/transform.h \
    thrust/transform_reduce.h \
    thrust/transform_scan.h \
    thrust/tuple.h \
    ], [], AC_MSG_ERROR([required Thrust header not found]), [-])

AC_CHECK_HEADER([gsl/gsl_multimin.h], [], AC_MSG_ERROR([required GSL header not found]), [-])

if test x$cuda = xtrue; then
    AC_CHECK_HEADER([cublas_v2.h], [], AC_MSG_ERROR([CUBLAS header not found (required with --enable-cuda)]), [-])
    AC_CHECK_HEADER([curand.h], [], AC_MSG_ERROR([CURAND header not found (required with --enable-cuda)]), [-])
    AC_CHECK_HEADERS([magma_s.h magma_d.h], [], AC_MSG_ERROR([MAGMA header not found (required with --enable-cuda)]), [-])
fi

if test x$mpi = xtrue; then
    AC_CHECK_HEADER([mpi.h], [], [AC_MSG_ERROR([MPI header not found (required with --enable-mpi)])], [])
	AC_CHECK_HEADER([boost/mpi.hpp], [], [AC_MSG_ERROR([MPI header not found (required with --enable-mpi)])], [])
fi

if test x$gperftools = xtrue; then
    AC_CHECK_HEADER([google/profiler.h], [], [AC_MSG_ERROR([gperftools header not found (required with --enable-gperftools)])], [])
fi

# Restore user's flags
CPPFLAGS=$USER_CPPFLAGS
CXXFLAGS=$USER_CXXFLAGS
LDFLAGS=$USER_LDFLAGS

# Defines
AM_CONDITIONAL([ENABLE_ASSERT], [test x$assert = xtrue])
AM_CONDITIONAL([ENABLE_SINGLE], [test x$single = xtrue])
AM_CONDITIONAL([ENABLE_CUDA], [test x$cuda = xtrue])
AM_CONDITIONAL([ENABLE_SSE], [test x$sse = xtrue])
AM_CONDITIONAL([ENABLE_MPI], [test x$mpi = xtrue])
AM_CONDITIONAL([ENABLE_VAMPIR], [test x$vampir = xtrue])
AM_CONDITIONAL([ENABLE_INTEL], [test x$intel = xtrue])
AM_CONDITIONAL([ENABLE_EXTRADEBUG], [test x$extradebug = xtrue])
AM_CONDITIONAL([ENABLE_TIMING], [test x$timing = xtrue])
AM_CONDITIONAL([ENABLE_DIAGNOSTICS], [test x$diagnostics = xtrue])
AM_CONDITIONAL([ENABLE_GPERFTOOLS], [test x$gperftools = xtrue])

# Variables for automake
AC_SUBST([CUDA_CPPFLAGS], $CUDA_CPPFLAGS)
AC_SUBST([CUDA_CXXFLAGS], $CUDA_CXXFLAGS)
AC_SUBST([CUDA_LDFLAGS], $CUDA_LDFLAGS)

# Checks for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
