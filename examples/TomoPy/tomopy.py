#!/usr/bin/env python

import os
import sys
import shutil
import platform
import argparse
import traceback

import pyctest.pyctest as pyctest
import pyctest.pycmake as pycmake


#------------------------------------------------------------------------------#
def configure():

    # just a help message
    default_pyexe_help = "Python executable to use this can be absolue, relative, or CMake path"
    # this can be absolue, relative, or CMake path
    default_pyexe = "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/python"
    # default arguments to provide to ctest
    default_ctest_args = ["-VV", "-S", "Test.cmake"]
    # where the source directory is located by default
    default_source = os.path.join(os.getcwd(), "tomopy-src")
    # where the binary directory is located by default
    default_binary = os.path.join(os.getcwd(), "ctest-tomopy")

    # argument parser
    parser = argparse.ArgumentParser()

    parser.add_argument("--ctest-args",
                        help="CTest arguments",
                        type=str,
                        default=default_ctest_args,
                        nargs='*')
    parser.add_argument('--python-exe',
                        help=default_pyexe_help,
                        type=str,
                        default=default_pyexe)
    parser.add_argument('--source-dir',
                        help="Source directory",
                        type=str,
                        default=default_source)
    parser.add_argument("--binary-dir",
                        help="Working directory",
                        type=str,
                        default=default_binary)

    args = parser.parse_args()

    binary_dir = os.path.realpath(args.binary_dir)
    source_dir = os.path.realpath(args.source_dir)
    # make binary directory
    if not os.path.exists(binary_dir):
        os.makedirs(binary_dir)

    # do we want to checkout or try to update?
    if os.path.exists(source_dir):
        # try to pull latest version. not a fatal error if fails
        try:
            import git
            repo = git.Repo(source_dir)
            repo.remotes['origin'].pull()
        except Exception as e:
            print("Warning! Failure pulling repo: {}".format(e))

    else:
        # execute a checkout command
        cmd = pyctest.command(["git", "clone",
            "https://github.com/tomopy/tomopy.git", source_dir])
        cmd.SetWorkingDirectory(os.getcwd())
        cmd.SetTimeout("7200")
        cmd.SetOutputQuiet(False)
        cmd.SetErrorQuiet(False)
        cmd.Execute()
        print("Command: {}".format(cmd.Command()))
        print("Output: {}".format(cmd.Output()))
        print("Error: {}".format(cmd.Error()))
        print("Result: {}".format(cmd.Result()))
        print("Results: {}".format(cmd.Results()))


    # copy cmake files over
    for f in [ "PreInit", "PostInit" ]:
        fsrc = os.path.join(os.getcwd(),     "PyCTest{}.cmake".format(f))
        fdst = os.path.join(args.binary_dir, "PyCTest{}.cmake".format(f))
        if os.path.exists(fsrc) and fsrc != fdst:
            print("Copying file '{}' to '{}'...".format(fsrc, fdst))
            shutil.copyfile(fsrc, fdst)

    # copy python files over
    for f in [ "measurement", "run_tomopy" ]:
        fsrc = os.path.join(os.getcwd(), "{}.py".format(f))
        fdst = os.path.join(args.binary_dir, "{}.py".format(f))
        if os.path.exists(fsrc) and fsrc != fdst:
            print("Copying file '{}' to '{}'...".format(fsrc, fdst))
            shutil.copyfile(fsrc, fdst)
            shutil.copymode(fsrc, fdst)

    return args


#------------------------------------------------------------------------------#
def run_pyctest():

    # run argparse, checkout source, copy over files
    args = configure()

    #--------------------------------------------------------------------------#
    # set the python used in the tests (installed in PyCTestPreInit.cmake)
    pyexe = args.python_exe
    #--------------------------------------------------------------------------#
    # define source directory and binary directory
    # NOTE: source directory does not need a CMakeLists.txt
    binary_dir = os.path.realpath(args.binary_dir)
    source_dir = os.path.realpath(args.source_dir)

    # the name of the project
    pyctest.PROJECT_NAME = "TomoPy"
    # source directory
    pyctest.SOURCE_DIRECTORY = source_dir
    # binary directory
    pyctest.BINARY_DIRECTORY = binary_dir
    # site of CDash submission
    pyctest.SITE = platform.node()
    # build name for CDash
    pyctest.BUILD_NAME = "[{}] [{}] [{} {} {}] [Python ({}) {}]".format(
        pyctest.PROJECT_NAME,
        pyctest.GetGitBranch(source_dir),
        platform.uname()[0],
        platform.mac_ver()[0],
        platform.uname()[4],
        platform.python_implementation(),
        platform.python_version())

    # submit after "Test" has been called
    pyctest.TRIGGER = "Test"
    # how to checkout the code
    pyctest.CHECKOUT_COMMAND = "${} -E copy_directory {} {}/".format(
        "{CTEST_CMAKE_COMMAND}", source_dir, binary_dir)
    # how to build the code
    pyctest.BUILD_COMMAND = "{} setup.py build_ext --inplace".format(pyexe)
    # the submission track in CDash (default = Nightly)
    pyctest.MODEL = "Continuous"
    # custom variables
    pyctest.CUSTOM_COVERAGE_EXCLUDE = "NONE"
    pyctest.CUSTOM_MAXIMUM_NUMBER_OF_ERRORS = "200"
    pyctest.CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS = "300"
    pyctest.CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE = "104857600"

    #--------------------------------------------------------------------------#
    # find the CTEST_TOKEN_FILE
    home = os.environ.get("HOME")
    if home is None:
        home_d = os.environ.get("HomeDrive")
        home_p = os.environ.get("HomePath")
        if home_d is not None and home_p is not None:
            home = os.path.join(home_d, home_p)
    if home is not None:
        pyctest.set("CTEST_TOKEN_FILE",
                    os.path.join(home, os.path.join(".tokens", "nersc-tomopy")))

    #--------------------------------------------------------------------------#
    # run CMake to generate DartConfiguration.tcl
    cm = pycmake.cmake(binary_dir, pyctest.PROJECT_NAME)

    #--------------------------------------------------------------------------#
    # create a CTest that wraps "nosetest"
    test = pyctest.test()
    test.SetName("nosetests")
    test.SetCommand(["nosetests", "test",
                     "--with-coverage",
                     # "--cover-xml", "--cover-xml-file=cover.xml",
                     "--nocapture"])
    # set directory to run test
    test.SetProperty("WORKING_DIRECTORY", binary_dir)

    # phantoms to test
    phantoms = [ "shepp3d", "baboon", "cameraman",
                 "barbara", "checkerboard", "lena", "peppers" ]
    # the algorithms to test
    algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                   'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]

    # loop over phantoms
    for phantom in phantoms:
        nsize = "128"
        if phantom != "shepp3d":
            nsize = "512"
        else:
            # for shepp3d only
            # loop over algorithms and create tests for each
            for algorithm in algorithms:
                # algorithms
                test = pyctest.test()
                name = "{}_{}".format(phantom, algorithm)
                test.SetName(name)
                test.SetProperty("WORKING_DIRECTORY", binary_dir)
                test.SetCommand(["./run_tomopy.py",
                                 "-a", algorithm,
                                 "-p", phantom,
                                 "-s", nsize,
                                 "-A", "360",
                                 "-f", "jpeg",
                                 "-S", "2",
                                 "-c", "8"])

        # create a test comparing all the algorithms
        test = pyctest.test()
        name = "{}_{}".format(phantom, "comparison")
        test.SetName(name)
        test.SetProperty("WORKING_DIRECTORY", binary_dir)
        test.SetCommand(["./run_tomopy.py",
                         "--compare", "all",
                         "-p", phantom,
                         "-s", nsize,
                         "-A", "360",
                         "-f", "jpeg",
                         "-S", "1"])

    # generate the CTestConfig.cmake and CTestCustom.cmake
    # configuration files, copy over
    # {Build,Coverage,Glob,Init,Memcheck,Stages,Submit,Test}.cmake
    # files located in the pyctest installation directory
    # - These are helpers for the workflow
    pyctest.generate_config(binary_dir)

    # generate the CTestTestfile.cmake file
    # CRITICAL:
    #   call after creating/running dummy CMake as the cmake call will
    #   generate an empty CTestTestfile.cmake file that this package overwrites
    pyctest.generate_test_file(binary_dir)

    # not used but can run scripts
    #pyctest.add_presubmit_command(binary_dir,
    #    [os.path.join(binary_dir, "measurement.py"), "Coverage",
    #     os.path.join(binary_dir, "cover.xml"), "text/xml"], clobber=True)
    #pyctest.add_note(binary_dir, os.path.join(binary_dir, "cover.xml"), clobber=True)

    # run CTest -- e.g. ctest -VV -S Test.cmake <binary_dir>
    pyctest.run(args.ctest_args, binary_dir)


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
