#!/usr/bin/env python

import os
import sys
import platform
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
