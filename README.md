# pyctest

## Available on PyPi and Anaconda

- PyPi has the source distribution
  - PyPi installs can take a long time since CMake must be compiled from scratch
- Anaconda has compiled distributions

### Travis

[![Build Status](https://travis-ci.org/jrmadsen/pyctest.svg?branch=master)](https://travis-ci.org/jrmadsen/pyctest)

### AppVeyor

[![Build status](https://ci.appveyor.com/api/projects/status/p7m76ovx7sg781pf/branch/master?svg=true)](https://ci.appveyor.com/project/jrmadsen/pyctest/branch/master)

### Anaconda

[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/version.svg)](https://anaconda.org/jrmadsen/pyctest)

[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/latest_release_date.svg)](https://anaconda.org/jrmadsen/pyctest)

[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/platforms.svg)](https://anaconda.org/jrmadsen/pyctest)

[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/installer/conda.svg)](https://conda.anaconda.org/jrmadsen)

[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/pyctest/badges/downloads.svg)](https://anaconda.org/jrmadsen/pyctest)

### Python Modules

- `import pyctest` -- global package
- `import pyctest.pyctest` -- CTest module
- `import pyctest.pycmake` -- CMake module

- NOTES:
    - This document uses `pyctest.<...>` as shorthand for `pyctest.pyctest.<...>` (e.g. `import pyctest.pyctest as pyctest`)
    - It is possible to call CMake from this package but it is generally not the purpose

### Benefits

- Integration into continuous integration systems (e.g. Travis, AppVeyor, Jenkins, etc.) and pushing to CDash dashboard will combine all the results in one place
    - The warnings and errors are enumerated in CDash (no more parsing stdout logs for errors)
- Easily create platform-independent testing
- No need to migrate build system to CMake -- just specify `pyctest.BUILD_COMMAND`
- Easily download Miniconda/Anaconda in workflow before build + testing
    - Using default python: `pip install pyctest` and follow recipe in [TomoPy PyCTestPreInit.cmake example](examples/TomoPy/PyCTestPreInit.cmake)


### Standard Configuration Variables

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

- [Basic example](examples/Basic/README.md)
- [Advanced example](examples/TomoPy/README.md)
  - includes submission to CDash dashboard

### CDash Integration Example

Results from running the TomoPy example can be found at the [TomoPy CDash Testing Dashboard @ NERSC](https://cdash.nersc.gov/index.php?project=TomoPy)

- Python code with C extensions without CMake build system
- The build logs from "python setup.py install" are registered in the "Build" section
- The `nosetests test` command + other are wrapped into CTests

### Basic Usage

Generates `pycm-test/CTestTestfile.cmake` but does not submit to dashboard

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

if __name__ == "__main__":

    directory = os.path.join(os.getcwd(), "pycm-test")

    # these are required
    pyctest.PROJECT_NAME = "PyCTest"
    pyctest.SOURCE_DIRECTORY = directory
    pyctest.BINARY_DIRECTORY = directory
    pyctest.MODEL = "Continuous"
    pyctest.SITE = platform.node()
    pyctest.BUILD_NAME = "[{}] [{} {} {}] [{} {}]".format(
        pyctest.PROJECT_NAME,
        platform.uname()[0],
        platform.mac_ver()[0],
        platform.uname()[4],
        platform.python_implementation(),
        platform.python_version())

    #--------------------------------------------------------------------------#
    # run CMake for initialization
    cm = pycmake.cmake(directory, pyctest.PROJECT_NAME)

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
    pyctest.run(["-V"], directory)

```

```bash
Running cmake with project role
-- The C compiler identification is GNU 7.3.0
-- Checking whether C compiler has -isysroot
-- Checking whether C compiler has -isysroot - yes
-- Checking whether C compiler supports OSX deployment target flag
-- Checking whether C compiler supports OSX deployment target flag - yes
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - failed
-- Detecting C compile features
-- Detecting C compile features - failed
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test
Writing CTest test file: "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test/CTestTestfile.cmake"...
Generating test "list_directory"...
Generating test "hostname"...
UpdateCTestConfiguration  from :/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test/DartConfiguration.tcl
Parse Config file:/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test/DartConfiguration.tcl
 Add coverage exclude regular expressions.
SetCTestConfiguration:CMakeCommand:/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pyctest/bin/cmake
UpdateCTestConfiguration  from :/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test/DartConfiguration.tcl
Parse Config file:/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test/DartConfiguration.tcl
Test project /Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test
Constructing a list of tests
Done constructing a list of tests
Updating test list for fixtures
Added 0 tests to meet fixture requirements
Checking test dependency graph...
Checking test dependency graph end
test 1
    Start 1: list_directory

1: Test command: /bin/ls "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-test"
1: Test timeout computed to be: 1500
1: Build.cmake
1: CMakeCache.txt
1: CMakeFiles
1: CMakeLists.txt
1: CTestConfig.cmake
1: CTestCustom.cmake
1: CTestTestfile.cmake
1: Coverage.cmake
1: DartConfiguration.tcl
1: Glob.cmake
1: Init.cmake
1: Makefile
1: MemCheck.cmake
1: Stages.cmake
1: Submit.cmake
1: Test.cmake
1: Testing
1: cmake_install.cmake
1/2 Test #1: list_directory ...................   Passed    0.00 sec
test 2
    Start 2: hostname

2: Test command: /bin/hostname
2: Test timeout computed to be: 10
2: JRM-macOS-DOE.local
2/2 Test #2: hostname .........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.01 sec
```
