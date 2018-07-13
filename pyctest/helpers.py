#!@PYTHON_EXECUTABLE@
# MIT License
#
# Copyright (c) 2018, The Regents of the University of California,
# through Lawrence Berkeley National Laboratory (subject to receipt of any
# required approvals from the U.S. Dept. of Energy).  All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

from __future__ import absolute_import

__author__ = "Jonathan Madsen"
__copyright__ = "Copyright 2018, The Regents of the University of California"
__credits__ = ["Jonathan Madsen"]
__license__ = "MIT"
__version__ = "@PROJECT_VERSION@"
__maintainer__ = "Jonathan Madsen"
__email__ = "jonrobm.programming@gmail.com"
__status__ = "Development"
__all__ = ['pyctest', 'pycmake', 'helpers']


import os
import sys
import platform
import warnings
import argparse

from . import pyctest
from . import pycmake


#------------------------------------------------------------------------------#
def GetHomePath():
    home = os.environ.get("HOME")
    if home is None:
        home_d = os.environ.get("HomeDrive")
        home_p = os.environ.get("HomePath")
        if home_d is not None and home_p is not None:
            home = os.path.join(home_d, home_p)
    return home


#------------------------------------------------------------------------------#
def RemoveTree(path):
    """
    function for safely removing a folder
    """
    try:
        import shutil
        if os.path.exists(path):
            shutil.rmtree(path)
    except:
        pass

#------------------------------------------------------------------------------#


def ParseArgs(project_name, source_dir, binary_dir,
              python_exe=sys.executable,
              ctest_mode="Test",
              ctest_trigger="Test",
              ctest_stages=["Build", "Test"],
              ctest_flags=[],
              ctest_model="Continuous",
              ctest_site=platform.node(),
              ctest_njobs=1):
    """
    - Adds common options for PyCTest
    - Parses the known options
    - Removes from sys.argv[]
    - Sets the defaults if not specified
    """

    # limit the mode choices
    mode_choices = ["Build", "Test", "Coverage",
                    "MemCheck", "Submit", "Stages"]
    # stage choices
    stage_choices = ["Build", "Test", "Coverage", "MemCheck", "Submit"]
    # choices for submission model
    model_choices = ["Nightly", "Continuous", "Experimental"]
    # choices for submit trigger
    trigger_choices = ["Build", "Test",
                       "Coverage", "MemCheck", "Submit", "None"]

    # just a help message
    default_pyexe_help = "{}. {}. Example: '{}'".format(
        "Python executable to use",
        "This can be absolue, relative, or CMake path",
        "${CMAKE_CURRENT_LIST_DIR}/Miniconda/bin/python")

    parser = argparse.ArgumentParser()

    parser.add_argument("--pyctest-ctest-args",
                        help="CTest arguments",
                        type=str,
                        default=[],
                        nargs='*')
    parser.add_argument("--pyctest-project-name",
                        help="Name of project using PyCTest",
                        type=str,
                        default=project_name)
    parser.add_argument("--pyctest-python-exe",
                        help=default_pyexe_help,
                        type=str,
                        default=python_exe)
    parser.add_argument("--pyctest-source-dir",
                        help="Source directory",
                        type=str,
                        default=source_dir)
    parser.add_argument("--pyctest-binary-dir",
                        help="Working directory",
                        type=str,
                        default=binary_dir)
    parser.add_argument("--pyctest-mode",
                        help="Testing mode (how to run the setup)",
                        type=str,
                        choices=mode_choices,
                        default=ctest_mode)
    parser.add_argument("--pyctest-stages",
                        help="Run modes separately in stages",
                        choices=stage_choices,
                        default=ctest_stages)
    parser.add_argument("--pyctest-trigger",
                        help="Submit to dashboard after this mode was run",
                        choices=trigger_choices,
                        default=ctest_trigger)
    parser.add_argument("--pyctest-model",
                        help="CTest submission model (CDash)",
                        type=str,
                        choices=model_choices,
                        default=ctest_model)
    parser.add_argument("--pyctest-jobs",
                        help="number of concurrent jobs",
                        type=int,
                        default=ctest_njobs)
    parser.add_argument("--pyctest-site",
                        help="CTest submission site",
                        type=str,
                        default=ctest_site)
    parser.add_argument("--pyctest-clean-first",
                        help="Remove any old source/binary directories",
                        action="store_true")
    parser.add_argument("--pyctest-token",
                        help="CTest site token for submission",
                        type=str)
    parser.add_argument("--pyctest-token-file",
                        help="Path to file for CTest site token for submission",
                        type=str)

    # read known arguments and remove
    args, left = parser.parse_known_args()

    binary_dir = os.path.realpath(args.pyctest_binary_dir)
    source_dir = os.path.realpath(args.pyctest_source_dir)

    # clean
    if args.pyctest_clean_first:
        RemoveTree(source_dir)
        RemoveTree(binary_dir)

    # make binary directory
    if not os.path.exists(binary_dir):
        os.makedirs(binary_dir)

    # append the stages to the CTest args
    if args.pyctest_mode == "Stages":
        args.pyctest_ctest_args.append(
            "-DSTAGES={}".format(";".join(args.pyctest_stages)))

    # append the mode to the CTest args
    args.pyctest_ctest_args.extend(
        ["-S", "{}.cmake".format(args.pyctest_mode)])
    if args.pyctest_jobs > 0:
        args.pyctest_ctest_args.append("-j{}".format(args.pyctest_jobs))

    # the name of the project
    pyctest.PROJECT_NAME = args.pyctest_project_name
    # source directory
    pyctest.SOURCE_DIRECTORY = source_dir
    # binary directory
    pyctest.BINARY_DIRECTORY = binary_dir
    # site of CDash submission
    pyctest.SITE = args.pyctest_site
    # build name for CDash
    pyctest.BUILD_NAME = "[{}] [{} {} {}] [Python ({}) {}]".format(
        pyctest.PROJECT_NAME,
        platform.uname()[0],
        platform.mac_ver()[0],
        platform.uname()[4],
        platform.python_implementation(),
        platform.python_version())

    # the Python to use with scripts
    pyctest.PYTHON_EXECUTABLE = args.pyctest_python_exe
    # submit after "Test" has been called
    pyctest.TRIGGER = args.pyctest_trigger
    # how to checkout the code
    pyctest.CHECKOUT_COMMAND = "${} -E copy_directory {} {}/".format(
        "{CTEST_CMAKE_COMMAND}", source_dir, binary_dir)
    # how to build the code
    pyctest.BUILD_COMMAND = "{} setup.py install".format(
        pyctest.PYTHON_EXECUTABLE)
    # the submission model in CDash (default = Nightly)
    pyctest.MODEL = args.pyctest_model

    # custom variables
    pyctest.CUSTOM_COVERAGE_EXCLUDE = "NONE"
    pyctest.CUSTOM_MAXIMUM_NUMBER_OF_ERRORS = "200"
    pyctest.CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS = "300"
    pyctest.CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE = "104857600"

    if args.pyctest_token:
        pyctest.set("CTEST_TOKEN", args.pyctest_token)
    elif args.pyctest_token_file:
        pyctest.set("CTEST_TOKEN_FILE", args.pyctest_token_file)
    else:
        home = GetHomePath()
        if home is not None:
            relpath = os.path.join(".tokens", pyctest.PROJECT_NAME)
            token_path = os.path.join(home, relpath)
            if os.path.exists(token_path):
                print("Setting CTEST_TOKEN_FILE to '{}'...".format(token_path))
                pyctest.set("CTEST_TOKEN_FILE", token_path)

    if len(args.pyctest_ctest_args) > 0:
        pyctest.ARGUMENTS.extend(args.pyctest_ctest_args)

    # replace sys.argv with unknown args only
    sys.argv = sys.argv[:1]+left

    return args
