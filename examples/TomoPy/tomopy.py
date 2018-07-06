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

    # default algorithm choices
    available_algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                             'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]
    # default phantom choices
    available_phantoms = [ "baboon", "cameraman", "barbara", "checkerboard",
                           "lena", "peppers", "shepp3d" ]

    # limit the mode choices
    mode_choices = [ "Build", "Test", "Coverage", "MemCheck", "Submit" ]
    # choices for submission track
    model_choices = [ "Nightly", "Continuous", "Experimental" ]
    # choices for submit trigger
    trigger_choices = [ "Build", "Test", "Coverage", "MemCheck", "Submit", "None" ]
    # choices for algorithms
    algorithm_choices = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                          'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad',
                          'none', 'all' ]
    # phantom choices
    phantom_choices = [ "baboon", "cameraman", "barbara", "checkerboard",
                        "lena", "peppers", "shepp3d", "none", "all" ]

    # just a help message
    default_pyexe_help = "Python executable to use this can be absolue, relative, or CMake path"
    # this can be absolue, relative, or CMake path
    default_pyexe = "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/python"
    # the default testing mode
    default_ctest_mode = "Test"
    # the default submit trigger
    default_ctest_trigger = "{}".format(default_ctest_mode)
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
    # default algorithm choices
    default_algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                           'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]
    # default phantom choices
    default_phantoms = [ "baboon", "cameraman", "barbara", "checkerboard",
                         "lena", "peppers", "shepp3d" ]

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
    parser.add_argument("-t", "--trigger",
                        help="Submit to dashboard after this mode was run",
                        choices=trigger_choices,
                        default=default_ctest_trigger)
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
    parser.add_argument("--phantoms",
                        help="Phantoms to simulate",
                        type=str,
                        nargs='*',
                        choices=phantom_choices,
                        default=default_phantoms)
    parser.add_argument("--algorithms",
                        help="Algorithms to use",
                        type=str,
                        nargs='*',
                        choices=algorithm_choices,
                        default=default_algorithms)
    parser.add_argument("--globus-path",
                        help="Path to tomobank datasets",
                        type=str,
                        default=None)

    args = parser.parse_args()

    binary_dir = os.path.realpath(args.binary_dir)
    source_dir = os.path.realpath(args.source_dir)

    # make binary directory
    if not os.path.exists(binary_dir):
        os.makedirs(binary_dir)

    pyctest.git_checkout("https://github.com/tomopy/tomopy.git", source_dir)

    # append the mode to the CTest args
    args.ctest_args.extend(["-S", "{}.cmake".format(args.mode)])
    if args.jobs > 0:
        args.ctest_args.append("-j{}".format(args.jobs))

    def remove_entry(entry, container):
        for itr in container:
            if itr == entry:
                del itr

    def remove_duplicates(container):
        container = list(set(container))

    if "all" in args.algorithms:
        remove_entry("all", args.algorithms)
        args.algorithms.extend(available_algorithms)

    if "all" in args.phantoms:
        remove_entry("all", args.phantoms)
        args.phantoms.extend(available_phantoms)

    if "none" in args.algorithms:
        args.algorithms = []

    if "none" in args.phantoms:
        args.phantoms = []

    remove_duplicates(args.algorithms)
    remove_duplicates(args.phantoms)

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
    pyctest.TRIGGER = args.trigger
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
    # copy over files from os.getcwd() to pyctest.BINARY_DIR
    # (implicitly copies over PyCTest{Pre,Post}Init.cmake if they exist)
    pyctest.copy_files([ "tomopy_test_utils.py", "run_tomopy.py", "tomopy_rec.py"])

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

    if args.globus_path is not None:
        phantom = "tomo_00001"
        h5file = os.path.join(args.globus_path, os.path.join(phantom, phantom + ".h5"))
        # loop over args.algorithms and create tests for each
        for algorithm in args.algorithms:
            if not os.path.exists(h5file):
                print("Warning! HDF5 file '{}' does not exists! Skipping test...".format(h5file))
            # args.algorithms
            test = pyctest.test()
            name = "{}_{}".format(phantom, algorithm)
            test.SetName(name)
            test.SetProperty("WORKING_DIRECTORY", binary_dir)
            test.SetProperty("TIMEOUT", "3600") # 1 hour
            test.SetProperty("ENVIRONMENT", "OMP_NUM_THREADS=1")
            test.SetCommand([pyexe,
                             "./tomopy_rec.py",
                             h5file,
                             "-a", algorithm,
                             "--type", "slice",
                             "-f", "jpeg",
                             "-S", "1",
                             "-c", "4",
                             "-n", "{}".format(args.ncores),
                             "-i", "{}".format(args.num_iter)])

    # loop over args.phantoms
    for phantom in args.phantoms:
        nsize = 96
        if phantom != "shepp3d":
            nsize = 128
        else:
            # for shepp3d only
            # loop over args.algorithms and create tests for each
            for algorithm in args.algorithms:
                # SKIP FOR NOW -- TOO MUCH OUTPUT/INFORMATION
                continue
                # SKIP FOR NOW -- TOO MUCH OUTPUT/INFORMATION
                # args.algorithms
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

        # create a test comparing all the args.algorithms
        test = pyctest.test()
        name = "{}_{}".format(phantom, "comparison")
        test.SetName(name)
        test.SetProperty("WORKING_DIRECTORY", binary_dir)
        test.SetProperty("ENVIRONMENT", "OMP_NUM_THREADS=1")
        test.SetProperty("TIMEOUT", "10800") # 3 hours
        test.SetCommand([pyexe,
                         "./run_tomopy.py",
                         "-p", phantom,
                         "-s", "{}".format(nsize),
                         "-A", "360",
                         "-f", "jpeg",
                         "-S", "1",
                         "-n", "{}".format(args.ncores),
                         "-i", "{}".format(args.num_iter),
                         "--compare" ] + args.algorithms)

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
