#!/usr/bin/env python

import os, sys, platform, traceback
import pyctest.pyctest as pyctest
import pyctest.helpers as helpers


#------------------------------------------------------------------------------#
def run_pyctest():

    cwd = os.getcwd()
    # process standard arguments
    helpers.ArgumentParser("PyCTest", cwd, cwd, vcs_type="git",
                           build_type="MinSizeRel").parse_args()
    # base web address of dashboard
    pyctest.DROP_SITE = "cdash.nersc.gov"
    # custom setup.py command (runs CMake)
    pyctest.CONFIGURE_COMMAND = "python setup.py configure"
    # build and install
    pyctest.BUILD_COMMAND = "python setup.py install"
    # basic example test
    examples_dir = os.path.join(pyctest.SOURCE_DIRECTORY, "examples")
    basic_dir = os.path.join(examples_dir, "Basic")
    pyctest.test(name="basic", cmd=["python", "basic.py", "--", "-VV"],
                 properties={"WORKING_DIRECTORY": basic_dir})
    # tomopy example test
    tomopy_dir = os.path.join(examples_dir, "TomoPy")
    pyctest.test(name="tomopy", cmd=["python", "pyctest_tomopy.py",
                                     "--pyctest-stages", "Start",
                                     "Configure", "Build", "--", "-VV"],
                 properties={"WORKING_DIRECTORY": tomopy_dir})
    # run stages
    pyctest.run()


#------------------------------------------------------------------------------#
if __name__ == "__main__":
    try:
        run_pyctest()
    except Exception as e:
        print('Error running pyctest - {}'.format(e))
        exc_type, exc_value, exc_trback = sys.exc_info()
        traceback.print_exception(exc_type, exc_value, exc_trback, limit=10)
        sys.exit(1)
    sys.exit(0)
