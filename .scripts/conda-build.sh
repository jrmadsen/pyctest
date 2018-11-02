#!/bin/bash -l

set -o errexit

if [ ${PWD} = ${BASH_SOURCE[0]} ]; then
    cd ..
fi

module load python/conda

if [ -f "${PWD}/meta.yaml" ]; then
    ./update-conda-yaml.py
    conda config --set anaconda_upload yes
    for i in 3.6 3.5 2.7 3.4 3.7
    do
        conda-build -c conda-forge -c jrmadsen --override-channels --python=${i} .
        conda-build purge
    done
    conda config --set anaconda_upload no
fi
