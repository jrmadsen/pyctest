#!/usr/bin/env python

import os
import sys
import pyctest.pyctest as pyctest
import pyctest.pycmake as pycmake
import argparse
from shutil import copyfile, copymode
import git

#------------------------------------------------------------------------------#
if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--directory", help="Working directory", type=str,
        default="ctest-tomopy")
    parser.add_argument('-T', "--ctest-args", help="CTest arguments", nargs='*',
        type=str, default=["-VV", "-S", "Test.cmake"])

    args = parser.parse_args()
    dir = os.path.realpath(args.directory)
    if not os.path.exists(dir):
        os.makedirs(dir)

    tomopy_src = os.path.join(os.getcwd(), "tomopy-src")
    repo = None
    if os.path.exists(tomopy_src):
        repo = git.Repo(tomopy_src)
        repo.remotes['origin'].pull()
    else:
        repo = git.Repo()
        print("Checking out tomopy to '{}'...".format(tomopy_src))
        repo.clone_from("https://github.com/tomopy/tomopy.git", tomopy_src)


    # copy cmake files over
    for f in [ "PreInit", "PostInit" ]:
        fsrc = os.path.join(os.getcwd(), "PyCTest{}.cmake".format(f))
        fdst = os.path.join(dir, "PyCTest{}.cmake".format(f))
        if os.path.exists(fsrc) and fsrc != fdst:
            print("Copying file '{}' to '{}'...".format(fsrc, fdst))
            copyfile(fsrc, fdst)

    # copy python files over
    for f in [ "measurement" ]:
        fsrc = os.path.join(os.getcwd(), "{}.py".format(f))
        fdst = os.path.join(dir, "{}.py".format(f))
        if os.path.exists(fsrc) and fsrc != fdst:
            print("Copying file '{}' to '{}'...".format(fsrc, fdst))
            copyfile(fsrc, fdst)
            copymode(fsrc, fdst)

    minicondapy = "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/python"
    pyctest.PROJECT_NAME = "TomoPy"
    cm = pycmake.cmake(dir, pyctest.PROJECT_NAME)
    pyctest.SOURCE_DIRECTORY = tomopy_src
    pyctest.BINARY_DIRECTORY = dir
    pyctest.BUILD_NAME = "tomopy-pyctest"
    pyctest.SITE = "JRM-macOS-DOE"
    pyctest.TRIGGER = "Test"
    pyctest.CHECKOUT_COMMAND = "${} -E copy_directory {} {}/".format(
        "{CTEST_CMAKE_COMMAND}", tomopy_src, dir)
    pyctest.BUILD_COMMAND = "{} setup.py build_ext --inplace".format(minicondapy)
    pyctest.MODEL = "Continuous"
    pyctest.CUSTOM_COVERAGE_EXCLUDE = "NONE"
    pyctest.CUSTOM_MAXIMUM_NUMBER_OF_ERRORS = "200"
    pyctest.CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS = "300"
    pyctest.CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE = "104857600"
    pyctest.set("CTEST_TOKEN_FILE", "/Users/jrmadsen/.tokens/nersc-tomopy")

    test = pyctest.test()
    test.SetName("nosetests")
    test.SetCommand(["nosetests", "test", "--with-coverage", "--cover-xml",
                     "--cover-xml-file=cover.xml"])
    test.SetProperty("WORKING_DIRECTORY", dir)

    test = pyctest.test()
    test.SetName("xml-coverage")
    test.SetCommand([os.path.join(dir, "measurement.py"), "Coverage",
                     os.path.join(dir, "cover.xml"), "text/xml"])
    test.SetProperty("WORKING_DIRECTORY", dir)
    test.SetProperty("DEPENDS", "nosetests")

    test = pyctest.test()
    test.SetName("html-coverage")
    test.SetCommand([os.path.join(dir, "measurement.py"), "Coverage",
                     os.path.join(dir, "cover.xml"), "text/html"])
    test.SetProperty("WORKING_DIRECTORY", dir)
    test.SetProperty("DEPENDS", "nosetests")

    pyctest.generate_config(dir)
    pyctest.generate_test_file(dir)
    pyctest.add_presubmit_command(dir,
        [os.path.join(dir, "measurement.py"), "Coverage",
         os.path.join(dir, "cover.xml"), "text/xml"], clobber=True)
    pyctest.add_note(dir, os.path.join(dir, "cover.xml"), clobber=True)

    pyctest.run(args.ctest_args, dir)

