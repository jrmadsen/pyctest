#!/bin/bash -l

set -o errexit

if [ ${PWD} = ${BASH_SOURCE[0]} ]; then
    cd ..
fi

command-exists ()
{
    command -v $@
}

if [ ! -z "$(command-exists module)" ]; then
    if [ -n "$(module avail python &> /dev/stdout | grep 'python/conda')" ]; then
        module load python/conda
    fi
fi

if [ -z "$(command-exists conda)" ]; then
    echo "Unable to find conda executiable"
    exit 1
fi

if [ -z "$(command-exists conda-build)" ]; then
    echo "Unable to find conda-build script"
    exit 1
fi

if [ -f "${PWD}/meta.yaml" ]; then
    ./update-conda-yaml.py
    conda config --set anaconda_upload no
    for i in 2.7 3.{6,7,4,5}
    do
        conda-build --no-test \
            --cache-dir=/tmp/pyctest-conda-build \
            -c conda-forge -c jrmadsen \
            --override-channels --python=${i} .
        conda-build purge
    done
    conda config --set anaconda_upload no
fi
