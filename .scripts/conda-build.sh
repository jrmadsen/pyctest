#!/bin/bash

set -o errexit

if [ ${PWD} = ${BASH_SOURCE[0]} ]; then
    cd ..
fi

module load python/conda/latest

if [ -f "${PWD}/conda.yaml" ]; then
    ./update-conda-yaml.py
    for i in 27 35 36
    do
        source activate py${i}-builder
        conda-build .
    done
    rm ./meta.yaml
fi
