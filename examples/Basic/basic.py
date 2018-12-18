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
    pyctest.SOURCE_DIRECTORY = os.getcwd()
    pyctest.BINARY_DIRECTORY = directory

    args = helpers.ArgumentParser(pyctest.PROJECT_NAME,
                                  pyctest.SOURCE_DIRECTORY,
                                  pyctest.BINARY_DIRECTORY,
                                  update_command="git").parse_args()

    # set explicitly
    pyctest.MODEL = "Continuous"
    pyctest.SITE = platform.node()

    # create a Test object
    test = pyctest.test()
    test.SetName("list_directory")
    test.SetCommand(["ls", directory])
    test.SetProperty("WORKING_DIRECTORY", os.getcwd())
    test.SetProperty("LABELS", "shutil")

    # create a second test with direct initialization
    pyctest.test("hostname", ["hostname"],
                 {"RUN_SERIAL" : "ON", "TIMEOUT" : "10", "LABELS" : "network"})

    # run CTest -- e.g. ctest -VV ${PWD}/pycm-test
    pyctest.run()
