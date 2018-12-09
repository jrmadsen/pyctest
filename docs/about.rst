=====
About
=====

PyCTest is python bindings of select portions of CMake/CTest package.
This enables the generation of CTest test files from Python without a CMake build system.

.. |Travis Build Status| image:: https://travis-ci.org/jrmadsen/pyctest.svg?branch=master
   :target: https://travis-ci.org/jrmadsen/pyctest

.. |AppVeyor Build status| image:: https://ci.appveyor.com/api/projects/status/p7m76ovx7sg781pf/branch/master?svg=true
   :target: https://ci.appveyor.com/project/jrmadsen/pyctest/branch/master
.. |Anaconda Version| image:: https://anaconda.org/jrmadsen/pyctest/badges/version.svg
   :target: https://anaconda.org/jrmadsen/pyctest
.. |Anaconda Release| image:: https://anaconda.org/jrmadsen/pyctest/badges/latest_release_date.svg
   :target: https://anaconda.org/jrmadsen/pyctest
.. |Anaconda Platforms| image:: https://anaconda.org/jrmadsen/pyctest/badges/platforms.svg
   :target: https://anaconda.org/jrmadsen/pyctest
.. |Anaconda Installer| image:: https://anaconda.org/jrmadsen/pyctest/badges/installer/conda.svg
   :target: https://conda.anaconda.org/jrmadsen
.. |Anaconda Downloads| image:: https://anaconda.org/jrmadsen/pyctest/badges/downloads.svg
   :target: https://anaconda.org/jrmadsen/pyctest

Available on PyPi and Anaconda
------------------------------

-  PyPi has the source distribution
-  PyPi installs can take a long time since CMake must be compiled from
   scratch
-  Anaconda has compiled distributions

General Setup
~~~~~~~~~~~~~

-  Create an anaconda environment for PyCTest:
   ``conda create -n pyctest python=3.6 pyctest``
-  Activate this environment: ``source activate pyctest``
-  Write a driver Python script

Example for Python project
^^^^^^^^^^^^^^^^^^^^^^^^^^

The following is an example for a Python code with a compiled C
extension that uses ``nosetests`` for unit-testing:

.. code:: python

    #!/usr/bin/env python

    import os, sys, platform
    import pyctest.pyctest as pyctest
    import pyctest.helpers as helpers

    parser = helpers.ArgumentParser("ProjectName", source_dir=os.getcwd(), binary_dir=os.getcwd())
    parser.add_argument("-n", "--build", type=str, required=True, help="Build name for identification")
    args = parser.parse_args()

    pyctest.BUILD_NAME = "{}".format(args.build)
    pyctest.BUILD_COMMAND = "python setup.py build_ext --inplace"
    pyctest.UPDATE_COMMAND = "git"

    test = pyctest.test()
    test.SetName("unittest")
    # insert the command to run the tests for project
    test.SetCommand(["nosetests"])

    pyctest.generate_config()
    pyctest.generate_test_file()
    pyctest.run(pyctest.ARGUMENTS)

Example for autotools project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: python

    #!/usr/bin/env python

    import os, sys, platform
    import multiprocessing as mp
    import pyctest.pyctest as pyctest
    import pyctest.helpers as helpers

    parser = helpers.ArgumentParser("ProjectName", source_dir=os.getcwd(), binary_dir=os.getcwd())
    parser.add_argument("-n", "--build", type=str, required=True, help="Build name for identification")
    args = parser.parse_args()

    # CONFIGURE_COMMAND can only run one command so if autogen is required, just execute it here
    cmd = pyctest.command(["./autogen.sh"])
    cmd.SetWorkingDirectory(pyctest.SOURCE_DIRECTORY)
    cmd.SetErrorQuiet(False)
    cmd.Execute()

    pyctest.BUILD_NAME = "{}".format(args.build)
    pyctest.UPDATE_COMMAND = "git"
    pyctest.CONFIGURE_COMMAND = "./configure"
    pyctest.BUILD_COMMAND = "make -j{}".format(mp.cpu_count())

    test = pyctest.test()
    test.SetName("unittest")
    # insert the command to run the tests for project
    test.SetCommand(["./run-testing.sh"])

    pyctest.generate_config()
    pyctest.generate_test_file()
    pyctest.run(pyctest.ARGUMENTS)

Example for CMake project
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: python

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

    pyctest.generate_config(pyctest.BINARY_DIRECTORY)
    pyctest.generate_test_file(pyctest.BINARY_DIRECTORY)
    pyctest.run(pyctest.ARGUMENTS, pyctest.BINARY_DIRECTORY)

Python Modules
~~~~~~~~~~~~~~

-  ``import pyctest`` -- global package
-  ``import pyctest.pyctest`` -- CTest module
-  ``import pyctest.pycmake`` -- CMake module
-  ``import pyctest.helpers`` -- Helpers module

   -  includes command line arguments (``argparse``) for PyCTest

-  NOTES:

   -  This document uses ``pyctest.<...>`` as shorthand for
      ``pyctest.pyctest.<...>`` (e.g.
      ``import pyctest.pyctest as pyctest``)
   -  It is possible to call CMake from this package but it is generally
      not the purpose

Benefits
~~~~~~~~

-  Integration into continuous integration systems (e.g. Travis,
   AppVeyor, Jenkins, etc.) and pushing to CDash dashboard will combine
   all the results in one place

   -  The warnings and errors are enumerated in CDash (no more parsing
      stdout logs for errors)

-  Easily create platform-independent testing
-  No need to migrate build system to CMake -- just specify
   ``pyctest.BUILD_COMMAND``

Standard Configuration Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  ``pyctest.PROJECT_NAME``
-  ``pyctest.SOURCE_DIRECTORY``
-  ``pyctest.BINARY_DIRECTORY``
-  ``pyctest.SITE``
-  ``pyctest.BUILD_NAME``
-  ``pyctest.TRIGGER``
-  ``pyctest.CHECKOUT_COMMAND``
-  ``pyctest.BUILD_COMMAND``
-  ``pyctest.MODEL``
-  ``pyctest.CUSTOM_COVERAGE_EXCLUDE``
-  ``pyctest.CUSTOM_MAXIMUM_NUMBER_OF_ERRORS``
-  ``pyctest.CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS``
-  ``pyctest.CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE``

Setting Arbitrary Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: python

        pyctest.set("CTEST_TOKEN_FILE", "${CMAKE_CURRENT_LIST_DIR}/.ctest-token")

Generating a Test
~~~~~~~~~~~~~~~~~

.. code:: python

        test = pyctest.test()
        test.SetName("nosetests")
        test.SetCommand(["nosetests", "test", "--cover-xml", "--cover-xml-file=coverage.xml"])
        # set directory to run test
        test.SetProperty("WORKING_DIRECTORY", pyctest.BINARY_DIRECTORY)
        test.SetProperty("RUN_SERIAL", "ON")
        test.SetProperty("ENVIRONMENT", "OMP_NUM_THREADS=1")

Examples
~~~~~~~~

-  `Basic example <examples/Basic/README.md>`__
-  `Advanced example <examples/TomoPy/README.md>`__
-  includes submission to CDash dashboard

CDash Integration Example
~~~~~~~~~~~~~~~~~~~~~~~~~

Results from running the TomoPy example can be found at the `TomoPy
CDash Testing Dashboard @
NERSC <https://cdash.nersc.gov/index.php?project=TomoPy>`__

-  Python code with C extensions without CMake build system
-  The build logs from "python setup.py install" are registered in the
   "Build" section
-  The ``nosetests test`` command + other are wrapped into CTests

Testing Example
---------------

PyCTest can be used to simple execute tests and submit to a dashboard
without any configuration, build, etc. steps

.. code:: python

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

.. code:: bash

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

