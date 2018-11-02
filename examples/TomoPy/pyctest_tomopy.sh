#!/bin/bash -l

set -o errexit


# -----------------------------------------------------------------------------#
#
#       Clean up
#
# -----------------------------------------------------------------------------#
rm -rf tomopy-ctest


# -----------------------------------------------------------------------------#
#
#       Miniconda with pyctest
#
# -----------------------------------------------------------------------------#
module load python


# -----------------------------------------------------------------------------#
#
#       Environment settings for PyCTest
#
# -----------------------------------------------------------------------------#
CONDA_EXE=$(which conda)
if [ -n "${CONDA_EXE}" ]; then export CONDA_PREFIX=$(dirname $(dirname $(which conda))); fi
export CONDA_ENVIRONMENT=tomopy-pyctest
export PYTHON_VERSION=3.6


# -----------------------------------------------------------------------------#
#
#       Environment settings for TomoPy
#
# -----------------------------------------------------------------------------#
export TOMOPY_NUM_THREADS=2
export OMP_NUM_THREADS=$(nproc)
export TOMOPY_USE_TIMEMORY=OFF
export TOMOPY_USE_CPU=ON
export TOMOPY_USE_CUDA=OFF
export TOMOPY_USE_OPENACC=OFF
export TOMOPY_USE_OPENMP=OFF


# -----------------------------------------------------------------------------#
#
#       Diagnostic
#
# -----------------------------------------------------------------------------#
echo -e "\n##################################################"
echo -e "# Environment"
echo -e "##################################################\n"
env | egrep -i 'python|conda|tomo' | egrep -v 'PATH' | sort
echo -e "\n##################################################\n"


# -----------------------------------------------------------------------------#
#
#       Execution
#
# -----------------------------------------------------------------------------#
CMD="python ./tomopy.py --globus-path=${HOME}/devel/globus --num-iter=10 --ncores=4"
echo -e "Running '${CMD}'..."
eval ${CMD}
