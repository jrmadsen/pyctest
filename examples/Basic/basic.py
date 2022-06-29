#!/usr/bin/env python

import os
import sys
import platform
import pyctest.pyctest as pyct
import pyctest.helpers as helpers

if __name__ == "__main__":

    directory = os.path.join(os.getcwd(), "pycm-test")

    # these are required
    pyct.PROJECT_NAME = "PyCTest"
    pyct.SOURCE_DIRECTORY = os.getcwd()
    pyct.BINARY_DIRECTORY = directory

    args = helpers.ArgumentParser(
        pyct.PROJECT_NAME,
        pyct.SOURCE_DIRECTORY,
        pyct.BINARY_DIRECTORY,
        update_command="git",
    ).parse_args()

    # set explicitly
    pyct.MODEL = "Continuous"
    pyct.SITE = platform.node()

    # create a Test object
    test = pyct.test()
    test.SetName("list_directory")
    test.SetCommand(["ls", directory])
    test.SetProperty("WORKING_DIRECTORY", os.getcwd())
    test.SetProperty("LABELS", "shutil")

    # create a second test with direct initialization
    pyct.test(
        "hostname",
        ["hostname"],
        {"RUN_SERIAL": "ON", "TIMEOUT": "10", "LABELS": "network"},
    )

    # run CTest -- e.g. ctest -VV ${PWD}/pycm-test
    ret = pyct.run()

    sys.exit(ret)
