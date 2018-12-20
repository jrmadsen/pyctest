#FROM debian:latest
FROM continuumio/miniconda

ENV DEBIAN_FRONTEND noninteractive
WORKDIR /root
USER root
ENV HOME /root
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US
ENV LC_ALL C

SHELL [ "/bin/bash", "--login", "-c" ]

# default is Python 3.6
# pass "--build-arg PYTHON=2.7" to enable Python 2.7 or another version
ARG PYTHON=3.6
ARG BRANCH=master

RUN conda install -y -n root python=${PYTHON_VERSION} && \
    apt-get update --fix-missing && \
    apt-get -y install cmake git-core build-essential && \
    git clone -b ${BRANCH} https://github.com/jrmadsen/pyctest.git ${HOME}/pyctest && \
    cd ${HOME}/pyctest && \
    echo -e "\n\nUsing python = $(which python)\n\n" && \
    python setup.py install && \
    cd ${HOME} && \
    rm -rf ${HOME}/pyctest && \
    apt-get -y remove cmake git-core build-essential &&	\
    apt-get -y --purge autoremove && \
    apt-get -y autoclean && \
    rm -rf /var/lib/apt/lists/* && \
    conda clean --all
