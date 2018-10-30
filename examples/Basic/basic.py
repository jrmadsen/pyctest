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

#------------------------------------------------------------------------------#
def modify(test):
    """
    Modify something about the test
    Input:
        name: str -- the test name
    """
    pyctest.remove_test(test)
    pyctest.add_test(test)
    test.SetProperty("RUN_SERIAL", "ON")


#------------------------------------------------------------------------------#
def info(name):
    """
    Print some info about the test
    Input:
        name: str -- the test name
    """
    test = pyctest.find_test(name)
    print('Found test "{}" with RUN_SERIAL property = {}'.format(
        test.GetName(), test.GetProperty("RUN_SERIAL")))


#------------------------------------------------------------------------------#
def generate(name, cmd):
    """
    Demonstrate the generation of test with PyCTest
    Input:
        name: str -- the test name
        cmd: list -- the test command
    """

    #--------------------------------------------------------------------------#
    # create a Test object (automatically stored in a list of tests with pyctest)
    #
    test = pyctest.test()

    #--------------------------------------------------------------------------#
    # set the test name
    #
    test.SetName(name)

    #--------------------------------------------------------------------------#
    # set the test command
    #
    test.SetCommand(cmd)

    #--------------------------------------------------------------------------#
    # set some properties -- reference:
    # https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#properties-on-tests
    #
    test.SetProperty("WORKING_DIRECTORY", pyctest.BINARY_DIRECTORY)

    #--------------------------------------------------------------------------#
    # this test should not be run in parallel with any other tests
    #
    test.SetProperty("RUN_SERIAL", "0")

    #--------------------------------------------------------------------------#
    # set some environment variables for the test
    #
    test.SetProperty("ENVIRONMENT", "EXAMPLE_ENV_A={};EXAMPLE_ENV_B={}".format(
                     "Successfully set example environment variable A",
                     "Successfully set example environment variable B"))

    #--------------------------------------------------------------------------#
    # print info on test
    #
    info(name)

    #--------------------------------------------------------------------------#
    # modify the test
    #
    modify(test)

    #--------------------------------------------------------------------------#
    # show info updated
    #
    info(name)


#------------------------------------------------------------------------------#
def run_pyctest():
    """
    Example of using pyctest
    """
    parser = helpers.ArgumentParser(project_name="PyCTest-Basic-Example",
                                    source_dir=os.getcwd(),
                                    binary_dir=os.path.join(
                                        os.getcwd(), "work_dir"),
                                    python_exe=sys.executable)
    args = parser.parse_args()

    #--------------------------------------------------------------------------#
    # echo the arguments
    #
    print('Arguments:\n{}'.format(args))

    #--------------------------------------------------------------------------#
    # create some tests
    #
    test_dir = os.path.join(pyctest.BINARY_DIRECTORY, "test_dir")
    env_check_script = os.path.join(os.getcwd(), "check_env.py")

    generate("make_test_directory", [ "mkdir", "-p", "-v", test_dir ])
    generate("list_directories",    [ "ls", pyctest.BINARY_DIRECTORY ])
    generate("print_hostname",      [ "hostname" ])
    generate("check_environment",   [ sys.executable, env_check_script ])

    #--------------------------------------------------------------------------#
    # generate the CTestConfig.cmake and CTestCustom.cmake
    #
    pyctest.generate_config(pyctest.BINARY_DIRECTORY)

    #--------------------------------------------------------------------------#
    # generate the CTestTestfile.cmake file
    # CRITICAL: call after creating/running dummy pyctest.pycmake.cmake object
    #
    pyctest.generate_test_file(pyctest.BINARY_DIRECTORY)

    #--------------------------------------------------------------------------#
    # run CTest -- e.g. ctest -VV <binary_dir>
    #
    pyctest.run(pyctest.ARGUMENTS, pyctest.BINARY_DIRECTORY)


#------------------------------------------------------------------------------#
if __name__ == "__main__":

    try:

        run_pyctest()

    except Exception as e:
        print ('Error running pyctest - {}'.format(e))
        exc_type, exc_value, exc_trback = sys.exc_info()
        traceback.print_exception(exc_type, exc_value, exc_trback, limit=10)
        sys.exit(1)

    sys.exit(0)
