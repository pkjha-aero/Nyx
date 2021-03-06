.. role:: cpp(code)
   :language: c++



Compiling Nyx with SUNDIALS 5
===============================

The following steps describe how to compile Nyx with
SUNDIALS 5 support. This library is necessary for non-adiabatic
heating-cooling Nyx runs, where ``USE_HEATCOOL=TRUE``, such as
the ``Nyx/Exec/LyA`` directory.

In order to use SUNDIALS:

#. We suggest using the Github mirror:
   https://github.com/LLNL/sundials

   ::

      #!/bin/bash
      set -e
      git clone https://github.com/LLNL/sundials
      cd sundials
      mkdir builddir instdir
      INSTALL_PREFIX=$(pwd)/instdir
      cd builddir
      cmake \
      -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}  \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
      -DCMAKE_C_COMPILER=$(which cc)  \
      -DCMAKE_CXX_COMPILER=$(which CC)   \
      -DCMAKE_CUDA_HOST_COMPILER=$(which CC)    \
      -DEXAMPLES_INSTALL_PATH=${INSTALL_PREFIX}/examples \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CUDA_FLAGS="-DSUNDIALS_DEBUG_CUDA_LASTERROR" \
      -DSUNDIALS_BUILD_PACKAGE_FUSED_KERNELS=ON \
      -DCMAKE_C_FLAGS_RELEASE="-O3 -DNDEBUG" \
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG"  \
      -DCUDA_ENABLE=ON  \
      -DMPI_ENABLE=OFF  \
      -DOPENMP_ENABLE=ON   \
      -DF2003_INTERFACE_ENABLE=OFF   \
      -DSUNDIALS_INDEX_SIZE:INT=32   \
      -DCUDA_ARCH=sm_70 ../
      make -j8
      make install -j8

#. To install without cuda support:
         
   ::

      #!/bin/bash
      set -e
      git clone https://github.com/LLNL/sundials
      cd sundials
      mkdir builddir instdir
      INSTALL_PREFIX=$(pwd)/instdir
      cd builddir
      cmake \
      -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}  \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
      -DCMAKE_C_COMPILER=$(which cc)  \
      -DCMAKE_CXX_COMPILER=$(which CC)   \
      -DCMAKE_CUDA_HOST_COMPILER=$(which CC)    \
      -DEXAMPLES_INSTALL_PATH=${INSTALL_PREFIX}/examples \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS_RELEASE="-O3 -DNDEBUG" \
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG"  \
      -DCUDA_ENABLE=OFF  \
      -DMPI_ENABLE=OFF  \
      -DOPENMP_ENABLE=ON   \
      -DF2003_INTERFACE_ENABLE=OFF   \
      -DSUNDIALS_INDEX_SIZE:INT=32 ../
      make -j8
      make install -j8

#. Note that we use ``cc`` and ``CC`` compiler wrappers, which need to be consistent with the Nyx's GNUMakefile
   variable COMP to ensure matching OMP runtime libraries for use with the OpenMP NVector. 

#. ``CUDA_ARCH`` must be set to the appropriate value for the GPU being targeted

#. For more detailed instructions for installing SUNDIALS with different flags and versions see
   the `SUNDIALS documentation <https://computing.llnl.gov/projects/sundials/sundials-software>`_.

#. In the ``GNUmakefile`` for the application which uses the interface to SUNDIALS, add
   ``USE_SUNDIALS = TRUE`` and ``SUNDIALS_ROOT=${INSTALL_PREFIX}``. Note that one must define the
   ``SUNDIALS_LIB_DIR`` make variable to point to the location where the libraries are installed
   if they are not installed in the default location which is ``${INSTALL_PREFIX}/lib64``.

#. If the application uses the SUNDIALS CVODE time integrator package, then the variable
   ``USE_CVODE_LIBS = TRUE`` should also be added in the ``GNUmakefile`` for the application.
   If the application used the SUNDIALS ARKode time integrator package, then the variable
   ``USE_ARKODE_LIBS = TRUE`` should be added.

Note that SUNDIALS can also be installed via Spack:

   ::
      
      spack install sundials+cuda+openmp
  
