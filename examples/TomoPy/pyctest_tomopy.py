#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
PyCTest driver for TomoPy
"""

import os
import sys
import platform
import traceback
import pyctest.pyctest as pyct
import pyctest.helpers as helpers


def run_pyctest():

    repo_src = os.path.join(os.getcwd(), "tomopy-src")
    repo_bin = os.path.join(os.getcwd(), "tomopy-bin")
    helpers.RemovePath(repo_src)
    helpers.RemovePath(repo_bin)

    # Get pyctest argument parser that include PyCTest arguments
    parser = helpers.ArgumentParser(
        project_name="TomoPy",
        source_dir=repo_src,
        binary_dir=repo_bin,
        python_exe=sys.executable,
        vcs_type="git",
        ctest_args=["-VV"],
    )

    default_repo_url = "https://github.com/tomopy/tomopy.git"
    default_repo_branch = "master"

    parser.add_argument(
        "--repo-url",
        help="Set the repository URL",
        type=str,
        default=default_repo_url,
    )
    parser.add_argument(
        "--repo-branch",
        help="Branch of the repository",
        type=str,
        default=default_repo_branch,
    )
    args = parser.parse_args()

    # executables
    pyexe = pyct.PYTHON_EXECUTABLE
    pyexe_dir = os.path.dirname(pyexe)
    coverage_exe = os.path.join(pyexe_dir, "coverage")
    nosetests_exe = os.path.join(pyexe_dir, "nosetests")

    # Set the build name
    pyct.BUILD_NAME = "[{}] [{} {} {}] [Python ({}) {}]".format(
        args.repo_branch,
        platform.uname()[0],
        helpers.GetSystemVersionInfo(),
        platform.uname()[4],
        platform.python_implementation(),
        platform.python_version(),
    )
    # Set the checkout command
    pyct.CHECKOUT_COMMAND = "git clone -b {} {} {}".format(
        args.repo_branch, args.repo_url, pyct.SOURCE_DIRECTORY
    )
    # Set the configuration command (copy to binary directory)
    pyct.CONFIGURE_COMMAND = "cmake -E copy_directory {}/ {}/".format(
        pyct.SOURCE_DIRECTORY, pyct.BINARY_DIRECTORY
    )
    # Set the build command
    pyct.BUILD_COMMAND = "{} -m pip install -e".format(
        pyct.PYTHON_EXECUTABLE
    )

    # create a CTest that wraps "nosetest"
    #
    pyct.test(
        name="nosetests",
        cmd=[pyexe, coverage_exe, "run", nosetests_exe],
        properties={
            "WORKING_DIRECTORY": pyct.BINARY_DIRECTORY,
            "TIMEOUT": "600",
            "ENVIRONMENT": "OMP_NUM_THREADS=1",
        },
    )

    return pyct.run()


if __name__ == "__main__":

    ret = 0
    try:
        ret = run_pyctest()
    except Exception as e:
        print("Error running pyctest - {}".format(e))
        exc_type, exc_value, exc_trback = sys.exc_info()
        traceback.print_exception(exc_type, exc_value, exc_trback, limit=10)
        ret = 1

    sys.exit(ret)
