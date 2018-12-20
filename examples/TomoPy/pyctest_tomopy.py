#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
PyCTest driver for TomoPy
"""

import os, sys, shutil, platform, argparse, traceback, multiprocessing as mp
import pyctest.pyctest as pyctest
import pyctest.helpers as helpers


#------------------------------------------------------------------------------#
def run_pyctest():

    repo_src = os.path.join(os.getcwd(), "tomopy-src")
    repo_bin = os.path.join(os.getcwd(), "tomopy-bin")
    helpers.RemovePath(repo_src)
    helpers.RemovePath(repo_bin)

    # Get pyctest argument parser that include PyCTest arguments
    parser = helpers.ArgumentParser(project_name="TomoPy",
                                    source_dir=repo_src,
                                    binary_dir=repo_bin,
                                    python_exe=sys.executable,
                                    vcs_type="git",
                                    ctest_args=["-VV"])

    default_repo_url = "https://github.com/tomopy/tomopy.git"
    default_repo_branch = "master"

    parser.add_argument("--repo-url",
                        help="Set the repository URL",
                        type=str, default=default_repo_url)
    parser.add_argument("--repo-branch",
                        help="Branch of the repository",
                        type=str, default=default_repo_branch)
    args = parser.parse_args()

    # executables
    pyexe = pyctest.PYTHON_EXECUTABLE
    pyexe_dir = os.path.dirname(pyexe)
    coverage_exe = os.path.join(pyexe_dir, "coverage")
    nosetests_exe = os.path.join(pyexe_dir, "nosetests")

    # Set the build name
    pyctest.BUILD_NAME = "[{}] [{} {} {}] [Python ({}) {}]".format(
        args.repo_branch, platform.uname()[0],
        helpers.GetSystemVersionInfo(), platform.uname()[4],
        platform.python_implementation(), platform.python_version())
    # Set the checkout command
    pyctest.CHECKOUT_COMMAND = "git clone -b {} {} {}".format(
        args.repo_branch, args.repo_url, pyctest.SOURCE_DIRECTORY)
    # Set the configuration command (copy to binary directory)
    pyctest.CONFIGURE_COMMAND = "cmake -E copy_directory {}/ {}/".format(
        pyctest.SOURCE_DIRECTORY, pyctest.BINARY_DIRECTORY)
    # Set the build command
    pyctest.BUILD_COMMAND = "{} setup.py build".format(pyctest.PYTHON_EXECUTABLE)

    #--------------------------------------------------------------------------#
    # create a CTest that wraps "nosetest"
    #
    pyctest.test(name="nosetests",
                 cmd=[pyexe, coverage_exe, "run", nosetests_exe],
                 properties={
                     "WORKING_DIRECTORY": pyctest.BINARY_DIRECTORY,
                     "TIMEOUT": "600",
                     "ENVIRONMENT": "OMP_NUM_THREADS=1"})

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
