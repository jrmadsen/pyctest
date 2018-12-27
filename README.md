# PyCTest

| Travis | AppVeyor |
| ------ | -------- |
| [![Build Status](https://travis-ci.org/jrmadsen/pyctest.svg?branch=master)](https://travis-ci.org/jrmadsen/pyctest) | [![Build status](https://ci.appveyor.com/api/projects/status/p7m76ovx7sg781pf/branch/master?svg=true)](https://ci.appveyor.com/project/jrmadsen/pyctest/branch/master) |

[PyCTest Documentation (readthedocs)](https://pyctest.readthedocs.io/en/latest/)
================================================================================

## GitHub

- `git clone https://github.com/jrmadsen/pyctest.git`

## PyPi

- `pip install pyctest`

## Anaconda Cloud

- `conda install -c jrmadsen pyctest`

| Name | Version | Platforms | Downloads |
| --- | --- | --- | --- |
| [![Conda Recipe](https://img.shields.io/badge/recipe-pyctest-green.svg)](https://anaconda.org/jrmadsen/pyctest) | [![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/version.svg)](https://anaconda.org/jrmadsen/pyctest) | [![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/platforms.svg)](https://anaconda.org/jrmadsen/pyctest) | [![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/downloads.svg)](https://anaconda.org/jrmadsen/pyctest) |

## conda-forge

- `conda install -c conda-forge pyctest`

| Name | Version | Platforms | Downloads |
| --- | --- | --- | --- |
| [![Conda Recipe](https://img.shields.io/badge/recipe-pyctest-green.svg)](https://anaconda.org/conda-forge/pyctest) | [![Conda Version](https://img.shields.io/conda/vn/conda-forge/pyctest.svg)](https://anaconda.org/conda-forge/pyctest) | [![Conda Platforms](https://img.shields.io/conda/pn/conda-forge/pyctest.svg)](https://anaconda.org/conda-forge/pyctest) | [![Conda Downloads](https://img.shields.io/conda/dn/conda-forge/pyctest.svg)](https://anaconda.org/conda-forge/pyctest) |

Benefits
========

- Integration into continuous integration systems (e.g. Travis, AppVeyor, CircleCI, etc.) and pushing to CDash dashboard will combine all the results in one place
  - The warnings and errors are enumerated in CDash (no more parsing stdout logs for errors)
- Easily create platform-independent testing
- No need to migrate build system to CMake -- just specify `pyctest.BUILD_COMMAND`

General Setup
=============

- Add PyCTest to conda environment: `conda install pyctest`
- Write a driver Python script, e.g. `pyctest-runner.py`, in top of the source code tree

## Example for Python project

The following is an example for a Python code with a compiled C extension that uses `nosetests` for unit-testing:

```python
#!/usr/bin/env python

import os, sys, platform
import pyctest.pyctest as pyctest
import pyctest.helpers as helpers

parser = helpers.ArgumentParser("ProjectName", source_dir=os.getcwd(),binary_dir=os.getcwd(), vcs_type="git")
args = parser.parse_args()

pyctest.BUILD_NAME = "{}".format(args.build)
pyctest.BUILD_COMMAND = "python setup.py build_ext --inplace"

test = pyctest.test()
test.SetName("unittest")
# insert the command to run the tests for project
test.SetCommand(["nosetests"])

pyctest.run()
```

## Example for autotools project

```python
#!/usr/bin/env python

import os, sys, platform
import multiprocessing as mp
import pyctest.pyctest as pyctest
import pyctest.helpers as helpers

parser = helpers.ArgumentParser("ProjectName", source_dir=os.getcwd(), binary_dir=os.getcwd(),
                                vcs_type="git")
parser.add_argument("-n", "--build", type=str, required=True, help="Build name for identification")
args = parser.parse_args()

# CONFIGURE_COMMAND can only run one command so if autogen is required, just execute it here
cmd = pyctest.command(["./autogen.sh"])
cmd.SetWorkingDirectory(pyctest.SOURCE_DIRECTORY)
cmd.SetErrorQuiet(False)
cmd.Execute()

pyctest.BUILD_NAME = "{}".format(args.build)
pyctest.CONFIGURE_COMMAND = "./configure"
pyctest.BUILD_COMMAND = "make -j{}".format(mp.cpu_count())

test = pyctest.test()
test.SetName("unittest")
# insert the command to run the tests for project
test.SetCommand(["./run-testing.sh"])

pyctest.run()
```

## Example for CMake project

```python
#!/usr/bin/env python

import os
import sys
import platform
import multiprocessing as mp
import pyctest.pyctest as pyctest
import pyctest.helpers as helpers

binary_dir = os.path.join(os.getcwd(), "build-ProjectName")
parser = helpers.ArgumentParser("ProjectName", os.getcwd(), binary_dir)
parser.add_argument("-n", "--build", type=str, required=True, help="Build name for identification")
args = parser.parse_args()

pyctest.BUILD_NAME = "{}".format(args.build)
pyctest.UPDATE_COMMAND = "git"
pyctest.CONFIGURE_COMMAND = "cmake {}".format(pyctest.SOURCE_DIRECTORY)
pyctest.BUILD_COMMAND = "cmake --build {} --target all -- -j{}".format(pyctest.BINARY_DIRECTORY, mp.cpu_count())

test = pyctest.test()
test.SetName("unittest")
# insert the command to run the tests for project
test.SetCommand(["./run-testing.sh"])

pyctest.run()
```

### Python Modules

- `import pyctest` -- global package
- `import pyctest.pyctest` -- CTest module
- `import pyctest.pycmake` -- CMake module
- `import pyctest.helpers` -- Helpers module
  - includes command line arguments (`argparse`) for PyCTest

### Direct Access to CMake/CTest/CPack Executables

- `python -m pyctest.cmake <ARGS>` == `cmake <ARGS>`
- `python -m pyctest.ctest <ARGS>` == `ctest <ARGS>`
- `python -m pyctest.cpack <ARGS>` == `cpack <ARGS>`

Following Python code:
```python
from pyctest.ctest import CTest
from pyctest.cmake import CMake
from pyctest.cpack import CPack

CMake({"CMAKE_BUILD_TYPE":"Release"}, os.getcwd(), "-G", "Ninja")
CTest("--build-and-test", os.getcwd(), "-VV")
CPack("-G", "TGZ")
```

is equivalent to the following shell commands:

```shell
$ cmake -DCMAKE_BUILD_TYPE=Release ${PWD} -G Ninja
$ ctest --build-and-test ${PWD} -VV
$ cpack -G TGZ
```

## Standard Configuration Variables

- `pyctest.PROJECT_NAME`
- `pyctest.SOURCE_DIRECTORY`
- `pyctest.BINARY_DIRECTORY`
- `pyctest.SITE`
- `pyctest.BUILD_NAME`
- `pyctest.TRIGGER`
- `pyctest.CHECKOUT_COMMAND`
- `pyctest.BUILD_COMMAND`
- `pyctest.MODEL`
- `pyctest.CUSTOM_COVERAGE_EXCLUDE`
- `pyctest.CUSTOM_MAXIMUM_NUMBER_OF_ERRORS`
- `pyctest.CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS`
- `pyctest.CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE`

### Setting Arbitrary Variables

```python
    pyctest.set("CTEST_TOKEN_FILE", "${CMAKE_CURRENT_LIST_DIR}/.ctest-token")
```

### Generating a Test

```python
    test = pyctest.test()
    test.SetName("nosetests")
    test.SetCommand(["nosetests", "test", "--cover-xml", "--cover-xml-file=coverage.xml"])
    # set directory to run test
    test.SetProperty("WORKING_DIRECTORY", pyctest.BINARY_DIRECTORY)
    test.SetProperty("RUN_SERIAL", "ON")
    test.SetProperty("ENVIRONMENT", "OMP_NUM_THREADS=1")
```

### Examples

- [Basic example](https://github.com/jrmadsen/pyctest/blob/master/examples/Basic/README.md)
- [Advanced example](https://github.com/jrmadsen/pyctest/blob/master/examples/TomoPy/README.md)
  - includes submission to CDash dashboard

### CDash Integration Example

Results from running the TomoPy example can be found at the [TomoPy CDash Testing Dashboard @ NERSC](https://cdash.nersc.gov/index.php?project=TomoPy)

- Python code with C extensions without CMake build system
- The build logs from "python setup.py install" are registered in the "Build" section
- The `nosetests test` command + other are wrapped into CTests

## Testing Example

PyCTest can be used to simple execute tests and submit to a dashboard without any configuration, build, etc. steps

```python
#!/usr/bin/env python

import os
import sys
import shutil
import argparse
import platform
import traceback

import pyctest.pyctest as pyctest
import pyctest.pycmake as pycmake
import pyctest.helpers as helpers

if __name__ == "__main__":

    directory = os.path.join(os.getcwd(), "pycm-test")

    # these are required
    pyctest.PROJECT_NAME = "PyCTest"
    pyctest.SOURCE_DIRECTORY = directory
    pyctest.BINARY_DIRECTORY = directory

    args = helpers.ArgumentParser(pyctest.PROJECT_NAME,
                                  pyctest.SOURCE_DIRECTORY,
                                  pyctest.BINARY_DIRECTORY).parse_args()

    # set explicitly
    pyctest.MODEL = "Continuous"
    pyctest.SITE = platform.node()

    # create a Test object
    test = pyctest.test()
    test.SetName("list_directory")
    test.SetCommand(["ls", directory])
    test.SetProperty("WORKING_DIRECTORY", os.getcwd())

    # create a second test
    # previous test is already stored by PyCTest
    test = pyctest.test()
    test.SetName("hostname")
    test.SetCommand(["hostname"])
    test.SetProperty("TIMEOUT", "10")

    # generate the CTestConfig.cmake and CTestCustom.cmake
    pyctest.generate_config(directory)

    # generate the CTestTestfile.cmake file
    pyctest.generate_test_file(directory)

    # run CTest -- e.g. ctest -VV ${PWD}/pycm-test
    pyctest.run(pyctest.ARGUMENTS, directory)
```

```bash
CTest arguments (default): '-V -DSTAGES=Start;Update;Configure;Build;Test;Coverage;MemCheck -S Stages.cmake -j1'
Writing CTest test file: "/Users/jrmadsen/devel/c++/pyctest-master/pycm-test/CTestTestfile.cmake"...
Generating test "list_directory"...
Generating test "hostname"...
-- STAGES = Start;Update;Configure;Build;Test;Coverage;MemCheck
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Running CTEST_START stage...
Run dashboard with model Continuous
   Source directory: /Users/jrmadsen/devel/c++/pyctest-master/pycm-test
   Build directory: /Users/jrmadsen/devel/c++/pyctest-master/pycm-test
   Track: Continuous
   Reading ctest configuration file: /Users/jrmadsen/devel/c++/pyctest-master/pycm-test/CTestConfig.cmake
   Site: JRM-macOS-DOE.local
   Build name: [Darwin macOS 10.13.6 x86_64] [Python 3.6.7]
   Use Continuous tag: 20181129-2118
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Skipping CTEST_UPDATE stage...
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Skipping CTEST_CONFIGURE stage...
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Skipping CTEST_BUILD stage...
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Running CTEST_TEST stage...
Test project /Users/jrmadsen/devel/c++/pyctest-master/pycm-test
    Start 1: list_directory
1/2 Test #1: list_directory ...................   Passed    0.00 sec
    Start 2: hostname
2/2 Test #2: hostname .........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.01 sec
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Skipping CTEST_COVERAGE stage...
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Skipping CTEST_MEMCHECK stage...
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Skipping CTEST_SUBMIT stage...
-- [[Darwin macOS 10.13.6 x86_64] [Python 3.6.7]] Finished Continuous Stages (Start;Update;Configure;Build;Test;Coverage;MemCheck)
```

## Command Line Interface

```shell
PyCTest args: ['--help']
  CTest args: []
  CMake args: []
usage: pyctest-runner.py [-h] [-F] [-S] [-A] [-j <INT>] [-m <TYPE>] [-b <NAME>] [-i <PATH>] [-o <PATH>] [-M <TYPE>] [-H <SITE>] [-P <EXE>] [-N <NAME>]
                         [-C <PATH> [<PATH> ...]] [-L <LABEL> [<LABEL> ...]] [-T [<TYPE> [<TYPE> ...]]] [-fc <EXE>] [-cc <EXE>] [-cxx <EXE>]
                         [--pyctest-token <TOKEN>] [--pyctest-token-file <FILE>] [--pyctest-vcs-type <TYPE>] [--pyctest-build-type <TYPE>]
                         [--pyctest-trigger <TYPE>] [--pyctest-use-launchers <BOOL>] [--pyctest-ctest-args [<ARG> [<ARG> ...]]] [--pyctest-cdash-version <STR>]
                         [--pyctest-submit-retry-count <INT>] [--pyctest-submit-retry-delay <INT>] [--pyctest-curl-options <LIST>]
                         [--pyctest-drop-location <STR>] [--pyctest-drop-method <STR>] [--pyctest-drop-site <STR>] [--pyctest-drop-site-password <STR>]
                         [--pyctest-drop-site-user <STR>] [--pyctest-nightly-start-time <STR>] [--pyctest-update-version-only]

PyCTest argparse. Arguments after first '--' are passed directly to CTest, arguments after second '--' are passed directly to CMake

optional arguments:
  -h, --help                                                                   show this help message and exit
  -F, --pyctest-clean-first                                                    Remove standard PyCTest files and binary directory (if not source directory)
  -S, --pyctest-submit                                                         Enable submission to dashboard
  -A, --pyctest-append                                                         Append to last invocation of CTest
  -j <INT>, --pyctest-jobs <INT>                                               number of concurrent jobs
  -m <TYPE>, --pyctest-mode <TYPE>                                             Run specific stage. Choices:
                                                                               [Start,Update,Configure,Build,Test,Coverage,MemCheck,Submit,Stages]
  -b <NAME>, --pyctest-build-name <NAME>                                       Build name for identification
  -i <PATH>, --pyctest-source-dir <PATH>                                       Source directory
  -o <PATH>, --pyctest-binary-dir <PATH>                                       Binary/build/working directory
  -M <TYPE>, --pyctest-model <TYPE>                                            CTest submission model (track). Choices: [Nightly,Continuous,Experimental]
  -H <SITE>, --pyctest-site <SITE>                                             CTest submission site
  -P <EXE>, --pyctest-python-exe <EXE>                                         Python executable to use. This can be absolue, relative, or CMake path
  -N <NAME>, --pyctest-project-name <NAME>                                     Name of project using PyCTest
  -C <PATH> [<PATH> ...], --pyctest-cleanup <PATH> [<PATH> ...]                Remove standard PyCTest files and binary directory (if not source directory) and
                                                                               exit
  -L <LABEL> [<LABEL> ...], --pyctest-subproject-labels <LABEL> [<LABEL> ...]  Add labels for subproject
  -T [<TYPE> [<TYPE> ...]], --pyctest-stages [<TYPE> [<TYPE> ...]]             Run multiple stages. Choices:
                                                                               [Start,Update,Configure,Build,Test,Coverage,MemCheck,Submit]
  -fc <EXE>, --pyctest-fortran-compiler <EXE>                                  Specify Fortan compiler (if needed)
  -cc <EXE>, --pyctest-c-compiler <EXE>                                        Specify the C compiler (if needed)
  -cxx <EXE>, --pyctest-cxx-compiler <EXE>                                     Specify C++ compiler (if needed)
  --pyctest-token <TOKEN>                                                      CTest site token for submission
  --pyctest-token-file <FILE>                                                  Path to file for CTest site token for submission
  --pyctest-vcs-type <TYPE>                                                    Set to enable revision ID discovery during the Update stage. Choices:
                                                                               [bzr,cvs,git,hg,p4,svn]
  --pyctest-build-type <TYPE>                                                  Specify CMAKE_BUILD_TYPE (if using CMake). Choices:
                                                                               [Release,RelWithDebInfo,Debug,MinSizeRel]
  --pyctest-trigger <TYPE>                                                     DEPRECATED
  --pyctest-use-launchers <BOOL>                                               Enable launchers
  --pyctest-ctest-args [<ARG> [<ARG> ...]]                                     CTest arguments
  --pyctest-cdash-version <STR>                                                Set CTest variable: 'CTEST_CDASH_VERSION'
  --pyctest-submit-retry-count <INT>                                           Set CTest variable: 'CTEST_SUBMIT_RETRY_COUNT'
  --pyctest-submit-retry-delay <INT>                                           Set CTest variable: 'CTEST_SUBMIT_RETRY_DELAY'
  --pyctest-curl-options <LIST>                                                Set CTest variable: 'CTEST_CURL_OPTIONS'
  --pyctest-drop-location <STR>                                                Set CTest variable: 'CTEST_DROP_LOCATION'
  --pyctest-drop-method <STR>                                                  Set CTest variable: 'CTEST_DROP_METHOD'
  --pyctest-drop-site <STR>                                                    Set CTest variable: 'CTEST_DROP_SITE'
  --pyctest-drop-site-password <STR>                                           Set CTest variable: 'CTEST_DROP_SITE_PASSWORD'
  --pyctest-drop-site-user <STR>                                               Set CTest variable: 'CTEST_DROP_SITE_USER'
  --pyctest-nightly-start-time <STR>                                           Set CTest variable: 'CTEST_NIGHTLY_START_TIME'
  --pyctest-update-version-only                                                Specify that you want the version control update command to only discover the
                                                                               current version that is checked out, and not to update to a different version

```
