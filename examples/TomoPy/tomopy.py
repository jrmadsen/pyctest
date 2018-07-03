#!/usr/bin/env python

import os
import sys
import shutil
import platform
import argparse
import traceback
import multiprocessing

import pyctest.pyctest as pyctest
import pyctest.pycmake as pycmake


#------------------------------------------------------------------------------#
def configure():

    # limit the mode choices
    mode_choices = [ "Build", "Test", "Coverage", "MemCheck" ]
    model_choices = [ "Nightly", "Continuous", "Experimental" ]
    # just a help message
    default_pyexe_help = "Python executable to use this can be absolue, relative, or CMake path"
    # this can be absolue, relative, or CMake path
    default_pyexe = "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/python"
    # the default testing mode
    default_ctest_mode = "Test"
    # default arguments to provide to ctest
    default_ctest_args = ["-VV"]
    # where the source directory is located by default
    default_source = os.path.join(os.getcwd(), "tomopy-src")
    # where the binary directory is located by default
    default_binary = os.path.join(os.getcwd(), "ctest-tomopy")
    # default model
    default_model = "Continuous"
    # number of cores
    default_ncores = multiprocessing.cpu_count()
    # number of simultaneous jobs
    default_njobs = 1
    # number of iterations
    default_nitr = 1
    # submission site
    default_site = platform.node()

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
    parser.add_argument("-m", "--mode",
                        help="Test mode",
                        type=str,
                        choices=mode_choices,
                        default=default_ctest_mode)
    parser.add_argument("-M", "--model",
                        help="CTest submission track",
                        type=str,
                        choices=model_choices,
                        default=default_model)
    parser.add_argument("-n", "--ncores",
                        help="number of cores",
                        type=int,
                        default=default_ncores)
    parser.add_argument("-j", "--jobs",
                        help="number of concurrent jobs",
                        type=int,
                        default=default_njobs)
    parser.add_argument("-i", "--num-iter",
                        help="number of iterations",
                        type=int,
                        default=default_nitr)
    parser.add_argument("--site",
                        help="CTest submission site",
                        type=str,
                        default=default_site)

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
        cmd.SetTimeout("600")
        cmd.SetOutputQuiet(False)
        cmd.SetErrorQuiet(False)
        cmd.Execute()

    # copy cmake files over
    for f in [ "PreInit", "PostInit" ]:
        fsrc = os.path.join(os.getcwd(),     "PyCTest{}.cmake".format(f))
        fdst = os.path.join(args.binary_dir, "PyCTest{}.cmake".format(f))
        if os.path.exists(fsrc) and fsrc != fdst:
            print("Copying file '{}' to '{}'...".format(fsrc, fdst))
            shutil.copyfile(fsrc, fdst)

    # copy python files over
    for f in [ "coverage", "run_tomopy" ]:
        fsrc = os.path.join(os.getcwd(), "{}.py".format(f))
        fdst = os.path.join(args.binary_dir, "{}.py".format(f))
        if os.path.exists(fsrc) and fsrc != fdst:
            print("Copying file '{}' to '{}'...".format(fsrc, fdst))
            shutil.copyfile(fsrc, fdst)
            shutil.copymode(fsrc, fdst)

    # append the mode to the CTest args
    args.ctest_args.extend(["-S", "{}.cmake".format(args.mode)])
    if args.jobs > 0:
        args.ctest_args.append("-j{}".format(args.jobs))

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
    pyctest.SITE = args.site
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
    pyctest.TRIGGER = args.mode
    # how to checkout the code
    pyctest.CHECKOUT_COMMAND = "${} -E copy_directory {} {}/".format(
        "{CTEST_CMAKE_COMMAND}", source_dir, binary_dir)
    # how to build the code
    pyctest.BUILD_COMMAND = "{} setup.py build_ext --inplace".format(pyexe)
    # the submission track in CDash (default = Nightly)
    pyctest.MODEL = args.model
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
    test.SetCommand(["nosetests", "test", "--cover-xml", "--cover-xml-file=coverage.xml"])
    # set directory to run test
    test.SetProperty("WORKING_DIRECTORY", binary_dir)
    test.SetProperty("RUN_SERIAL", "ON")
    test.SetProperty("ENVIRONMENT", "OMP_NUM_THREADS=1")

    # phantoms to test
    phantoms = [ "baboon", "cameraman",
                 "barbara", "checkerboard",
                 "lena", "peppers",
                 "shepp3d" ]
    # the algorithms to test
    algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                   'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]


    # loop over phantoms
    for phantom in phantoms:
        nsize = 96
        if phantom != "shepp3d":
            nsize = 128
        else:
            # for shepp3d only
            # loop over algorithms and create tests for each
            for algorithm in algorithms:
                # SKIP FOR NOW -- TOO MUCH OUTPUT/INFORMATION
                continue
                # SKIP FOR NOW -- TOO MUCH OUTPUT/INFORMATION
                # algorithms
                test = pyctest.test()
                name = "{}_{}".format(phantom, algorithm)
                test.SetName(name)
                test.SetProperty("WORKING_DIRECTORY", binary_dir)
                test.SetCommand([pyexe,
                                 "./run_tomopy.py",
                                 "-a", algorithm,
                                 "-p", phantom,
                                 "-s", "{}".format(nsize),
                                 "-A", "360",
                                 "-f", "jpeg",
                                 "-S", "2",
                                 "-c", "8",
                                 "-n", "{}".format(args.ncores),
                                 "-i", "{}".format(args.num_iter)])

        # create a test comparing all the algorithms
        test = pyctest.test()
        name = "{}_{}".format(phantom, "comparison")
        test.SetName(name)
        test.SetProperty("WORKING_DIRECTORY", binary_dir)
        test.SetProperty("ENVIRONMENT", "OMP_NUM_THREADS=1")
        test.SetProperty("TIMEOUT", "10800") # 3 hours
        test.SetCommand([pyexe,
                         "./run_tomopy.py",
                         "--compare", "all",
                         "-p", phantom,
                         "-s", "{}".format(nsize),
                         "-A", "360",
                         "-f", "jpeg",
                         "-S", "1",
                         "-n", "{}".format(args.ncores),
                         "-i", "{}".format(args.num_iter)])

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
