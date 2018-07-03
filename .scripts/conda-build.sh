#!/bin/bash

set -o errexit

if [ ${PWD} = ${BASH_SOURCE[0]} ]; then
    cd ..
fi

module load python/conda/latest

if [ -f "${PWD}/meta.yaml" ]; then
    ./update-conda-yaml.py
    for i in 36 35 27
    do
        _ENV="py${i}-builder"
        echo -e "\nEnvironment: ${_ENV}..."
        source activate ${_ENV}
        conda-build .
        source deactivate ${_ENV}
    done
fi
