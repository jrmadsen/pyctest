#!/bin/bash

set -o errexit

if [ ${PWD} = ${BASH_SOURCE[0]} ]; then
    cd ../
fi

rm -rf pyctest.* .eggs build dist
python setup.py sdist
cd dist
gpg --detach-sign -a *
twine upload *
sha256sum pyctest*.tar.gz
