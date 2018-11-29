# Basic example

- Setup some tests and run `ctest` in `basic.py`

```bash
# create amd run ctests
$ python ./basic.py
```

### Output

```bash
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
