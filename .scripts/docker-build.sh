#!/bin/bash

set -o errexit

if [ ${PWD} = ${BASH_SOURCE[0]} ]; then
    cd ..
fi

# set if not set
: ${LATEST:="3.6"}
: ${VERSIONS:="2.7 3.4 3.5 3.6 3.7"}

BUILD_DIR=${PWD}

for i in ${VERSIONS}
do
    TAG=$(echo ${i} | sed 's/\.//g')

    echo -e "\n\n---> Building for Python version ${i}...\n\n"

    docker build \
        --pull \
        -t jrmadsen/pyctest:py${TAG} \
        --build-arg PYTHON_VERSION=${i} \
        -f Dockerfile \
        ${PWD}

    echo -e "\n\n---> Pushing Python version ${i} in background...\n\n"
    docker push jrmadsen/pyctest:py${TAG} &

done

while [ ! -z "$(ps | grep 'docker push' | grep -v ' grep ')" ];
do
    echo -e "Waiting for docker push to complete..."
    ps | grep 'docker push' | grep -v ' grep '
    sleep 10
done

LATEST_TAG=$(echo ${LATEST} | sed 's/\.//g')
docker tag jrmadsen/pyctest:py${LATEST_TAG} jrmadsen/pyctest:latest
docker push jrmadsen/pyctest:latest
