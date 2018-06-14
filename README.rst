pyctest
=======

Results from running the TomoPy example can be found at the `TomoPy
CDash Testing Dashboard @
NERSC <https://cdash.nersc.gov/index.php?project=TomoPy>`__

CTestTestfile.cmake generation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Generates ``test_output/CTestTestfile.cmake`` but does not submit to
dashboard

.. code:: python

  #!/usr/bin/env python

  import os
  import pyctest

  #------------------------------------------------------------------------------#
  def modify(test):
      pyctest.remove_test(test)
      pyctest.add_test(test)
      test.SetProperty("RUN_SERIAL", "ON")


  #------------------------------------------------------------------------------#
  def info(name):
      test = pyctest.find_test(name)
      print('Found test "{}" with RUN_SERIAL property = {}'.format(
          test.GetName(), test.GetProperty("RUN_SERIAL")))


  #------------------------------------------------------------------------------#
  def generate(name):
      test = pyctest.test()
      test.SetName(name)
      test.SetCommand(["ls", "-a", os.getcwd()])
      test.SetProperty("WORKING_DIRECTORY", os.getcwd())
      test.SetProperty("RUN_SERIAL", "0")
      test.SetProperty("ENVIRONMENT", "DIR1={};DIR2={}".format(os.getcwd(),
          os.path.join(os.getcwd(), "test_output")))
      info(name)
      modify(test)
      info(name)


  #------------------------------------------------------------------------------#
  if __name__ == "__main__":
      generate("dummy_a")
      generate("dummy_b")
      generate("dummy_c")
      pyctest.generate("test_output")

Python Output
^^^^^^^^^^^^^

Command :
``python test.py && echo -e "\n\t### test_output/CTestTestfile.cmake ###\n" && cat test_output/CTestTestfile.cmake``

::

  Created test "0x7fc03f51d140"...
  Found test "dummy_a" with RUN_SERIAL property = 0
  Found test "dummy_a" with RUN_SERIAL property = ON
  Created test "0x7fc03f51d880"...
  Found test "dummy_b" with RUN_SERIAL property = 0
  Found test "dummy_b" with RUN_SERIAL property = ON
  Created test "0x7fc03f51dbc0"...
  Found test "dummy_c" with RUN_SERIAL property = 0
  Found test "dummy_c" with RUN_SERIAL property = ON
  Writing CTest test file: "test_output/CTestTestfile.cmake"...
  Generating test "dummy_a"...
  Generating test "dummy_b"...
  Generating test "dummy_c"...

      ### test_output/CTestTestfile.cmake ###

  add_test(dummy_a "ls" "-a" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug")
  set_tests_properties(dummy_a PROPERTIES  ENVIRONMENT "DIR1=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug;DIR2=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output" RUN_SERIAL "ON" WORKING_DIRECTORY "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug")
  add_test(dummy_b "ls" "-a" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug")
  set_tests_properties(dummy_b PROPERTIES  ENVIRONMENT "DIR1=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug;DIR2=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output" RUN_SERIAL "ON" WORKING_DIRECTORY "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug")
  add_test(dummy_c "ls" "-a" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug")
  set_tests_properties(dummy_c PROPERTIES  ENVIRONMENT "DIR1=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug;DIR2=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output" RUN_SERIAL "ON" WORKING_DIRECTORY "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug")

CTest Output
^^^^^^^^^^^^

Command : ``ctest -V``

::

  UpdateCTestConfiguration  from :/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output/DartConfiguration.tcl
  UpdateCTestConfiguration  from :/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output/DartConfiguration.tcl
  Test project /Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output
  Constructing a list of tests
  Done constructing a list of tests
  Updating test list for fixtures
  Added 0 tests to meet fixture requirements
  Checking test dependency graph...
  Checking test dependency graph end
  test 1
      Start 1: dummy_a

  1: Test command: /bin/ls "-a" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug"
  1: Environment variables:
  1:  DIR1=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug
  1:  DIR2=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output
  1: Test timeout computed to be: 10000000
  1: .
  1: ..
  1: .ninja_deps
  1: .ninja_log
  1: BC
  1: CMakeCache.txt
  1: CMakeFiles
  1: CMakeLibBuild.cmake
  1: CMakeLibTests
  1: CMakeServerLibTests
  1: CPackConfig.cmake
  1: CPackSourceConfig.cmake
  1: CTestConfig.cmake
  1: CTestLibBuild.cmake
  1: CTestTestfile.cmake
  1: LIBCURL
  1: OSXScriptLauncher
  1: build.ninja
  1: cmake
  1: cmake_install.cmake
  1: cmakexbuild
  1: cmsysTestProcess
  1: cmsysTestSharedForward
  1: cmsysTestsC
  1: cmsysTestsCxx
  1: cpack
  1: ctest
  1: exit_code
  1: libCMakeLib.a
  1: libCMakeServerLib.a
  1: libCPackLib.a
  1: libCTestLib.a
  1: libcmbzip2.a
  1: libcmcompress.a
  1: libcmcurl.a
  1: libcmexpat.a
  1: libcmjsoncpp.a
  1: libcmlibarchive.a
  1: libcmliblzma.a
  1: libcmlibrhash.a
  1: libcmlibuv.a
  1: libcmsys.a
  1: libcmsysTestDynload.so
  1: libcmsys_c.a
  1: libcmzlib.a
  1: memcheck_fail
  1: pseudo_cppcheck
  1: pseudo_cpplint
  1: pseudo_emulator
  1: pseudo_emulator_custom_command
  1: pseudo_iwyu
  1: pseudo_tidy
  1: purify
  1: pycmake.cpython-36m-darwin.so
  1: pyctest.cbp
  1: pyctest.cpython-36m-darwin.so
  1: pyctestConfig.cmake
  1: pyctestConfigVersion.cmake
  1: rules.ninja
  1: runcompilecommands
  1: source
  1: test-folder
  1: test.py
  1: testAffinity
  1: testConsoleBufChild
  1: testEncoding
  1: test_output
  1: valgrind
  1/3 Test #1: dummy_a ..........................   Passed    0.00 sec
  test 2
      Start 2: dummy_b

  2: Test command: /bin/ls "-a" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug"
  2: Environment variables:
  2:  DIR1=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug
  2:  DIR2=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output
  2: Test timeout computed to be: 10000000
  2: .
  2: ..
  2: .ninja_deps
  2: .ninja_log
  2: BC
  2: CMakeCache.txt
  2: CMakeFiles
  2: CMakeLibBuild.cmake
  2: CMakeLibTests
  2: CMakeServerLibTests
  2: CPackConfig.cmake
  2: CPackSourceConfig.cmake
  2: CTestConfig.cmake
  2: CTestLibBuild.cmake
  2: CTestTestfile.cmake
  2: LIBCURL
  2: OSXScriptLauncher
  2: build.ninja
  2: cmake
  2: cmake_install.cmake
  2: cmakexbuild
  2: cmsysTestProcess
  2: cmsysTestSharedForward
  2: cmsysTestsC
  2: cmsysTestsCxx
  2: cpack
  2: ctest
  2: exit_code
  2: libCMakeLib.a
  2: libCMakeServerLib.a
  2: libCPackLib.a
  2: libCTestLib.a
  2: libcmbzip2.a
  2: libcmcompress.a
  2: libcmcurl.a
  2: libcmexpat.a
  2: libcmjsoncpp.a
  2: libcmlibarchive.a
  2: libcmliblzma.a
  2: libcmlibrhash.a
  2: libcmlibuv.a
  2: libcmsys.a
  2: libcmsysTestDynload.so
  2: libcmsys_c.a
  2: libcmzlib.a
  2: memcheck_fail
  2: pseudo_cppcheck
  2: pseudo_cpplint
  2: pseudo_emulator
  2: pseudo_emulator_custom_command
  2: pseudo_iwyu
  2: pseudo_tidy
  2: purify
  2: pycmake.cpython-36m-darwin.so
  2: pyctest.cbp
  2: pyctest.cpython-36m-darwin.so
  2: pyctestConfig.cmake
  2: pyctestConfigVersion.cmake
  2: rules.ninja
  2: runcompilecommands
  2: source
  2: test-folder
  2: test.py
  2: testAffinity
  2: testConsoleBufChild
  2: testEncoding
  2: test_output
  2: valgrind
  2/3 Test #2: dummy_b ..........................   Passed    0.00 sec
  test 3
      Start 3: dummy_c

  3: Test command: /bin/ls "-a" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug"
  3: Environment variables:
  3:  DIR1=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug
  3:  DIR2=/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_output
  3: Test timeout computed to be: 10000000
  3: .
  3: ..
  3: .ninja_deps
  3: .ninja_log
  3: BC
  3: CMakeCache.txt
  3: CMakeFiles
  3: CMakeLibBuild.cmake
  3: CMakeLibTests
  3: CMakeServerLibTests
  3: CPackConfig.cmake
  3: CPackSourceConfig.cmake
  3: CTestConfig.cmake
  3: CTestLibBuild.cmake
  3: CTestTestfile.cmake
  3: LIBCURL
  3: OSXScriptLauncher
  3: build.ninja
  3: cmake
  3: cmake_install.cmake
  3: cmakexbuild
  3: cmsysTestProcess
  3: cmsysTestSharedForward
  3: cmsysTestsC
  3: cmsysTestsCxx
  3: cpack
  3: ctest
  3: exit_code
  3: libCMakeLib.a
  3: libCMakeServerLib.a
  3: libCPackLib.a
  3: libCTestLib.a
  3: libcmbzip2.a
  3: libcmcompress.a
  3: libcmcurl.a
  3: libcmexpat.a
  3: libcmjsoncpp.a
  3: libcmlibarchive.a
  3: libcmliblzma.a
  3: libcmlibrhash.a
  3: libcmlibuv.a
  3: libcmsys.a
  3: libcmsysTestDynload.so
  3: libcmsys_c.a
  3: libcmzlib.a
  3: memcheck_fail
  3: pseudo_cppcheck
  3: pseudo_cpplint
  3: pseudo_emulator
  3: pseudo_emulator_custom_command
  3: pseudo_iwyu
  3: pseudo_tidy
  3: purify
  3: pycmake.cpython-36m-darwin.so
  3: pyctest.cbp
  3: pyctest.cpython-36m-darwin.so
  3: pyctestConfig.cmake
  3: pyctestConfigVersion.cmake
  3: rules.ninja
  3: runcompilecommands
  3: source
  3: test-folder
  3: test.py
  3: testAffinity
  3: testConsoleBufChild
  3: testEncoding
  3: test_output
  3: valgrind
  3/3 Test #3: dummy_c ..........................   Passed    0.00 sec

  100% tests passed, 0 tests failed out of 3

  Total Test time (real) =   0.02 sec

Advanced Example
~~~~~~~~~~~~~~~~

-  located in examples/TomoPy of source code
-  Downloads Miniconda in PyCTestPreInit.cmake
-  Installs conda packages in PyCTestPreInit.cmake
-  Checkouts source code of Python package via ``git clone ...`` in
   ``tomopy.py``
-  Build source code via ``python setup.py install`` when running CTest
-  Generates a test around the ``nosetests`` unit testing
-  Generates tests around several algorithms by calling
   ``run_tomopy.py``
-  Submits to CDash dashboard at ``cdash.nersc.gov``

   -  Timing and memory plots provided via TiMemory
   -  Attaches CTest notes (e.g. ASCII results)
   -  Attaches images from reconstruction (e.g. Dart measurement files)

PyCTestPreInit.cmake
^^^^^^^^^^^^^^^^^^^^

.. code:: python


  message(STATUS "Including ${CMAKE_CURRENT_LIST_FILE}...")

  find_program(DEFAULT_CONDA_EXE NAMES conda)

  macro(SET_PYTHON_VERSION VAL)
      if(NOT DEFINED PYTHON_VERSION OR "${PYTHON_VERSION}" STREQUAL "")
          if(NOT "${VAL}" STREQUAL "")
              set(PYTHON_VERSION "${VAL}")
          endif(NOT "${VAL}" STREQUAL "")
      endif(NOT DEFINED PYTHON_VERSION OR "${PYTHON_VERSION}" STREQUAL "")
  endmacro(SET_PYTHON_VERSION VAL)

  set_python_version("$ENV{PYTHON_VERSION}")
  set_python_version("$ENV{TRAVIS_PYTHON_VERSION}")
  set_python_version("2.7")

  find_program(WGET_EXE NAMES wget)
  find_program(BASH_EXE NAMES bash)

  set(CONDA_PACKAGE_INSTALL OFF)
  if(NOT "$ENV{CONDA_PACKAGE_INSTALL}" STREQUAL "")
      set(CONDA_PACKAGE_INSTALL ON)
  endif()
  set(ENV{PYTHONPATH} "")
  set(CONDA_PKGS nose six numpy h5py scipy scikit-image pywavelets mkl-devel
      mkl_fft python-coveralls dxchange numexpr timemory scikit-image)
  #set(PIP_PKGS timemory)

  #------------------------------------------------------------------------------#
  #   local `miniconda` installation
  #------------------------------------------------------------------------------#
  # if not already installed
  if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/conda")

      set(CONDA_PACKAGE_INSTALL ON)
      if("${PYTHON_VERSION}" VERSION_EQUAL "2.7")
          set(MINICONDA_VER "Miniconda")
          list(APPEND CONDA_PKGS "futures")
      else()
          set(MINICONDA_VER "Miniconda3")
      endif()

      if(APPLE)
          set(_OS "MacOSX")
      elseif(WIN32)
          set(_OS "Windows")
      else()
          set(_OS "Linux")
      endif(APPLE)

      set(MINICONDA_URL "https://repo.continuum.io/miniconda/${MINICONDA_VER}-latest-${_OS}-x86_64.sh")
      execute_process(COMMAND
          ${WGET_EXE} ${MINICONDA_URL} -O ${CMAKE_CURRENT_LIST_DIR}/miniconda.sh)
      execute_process(COMMAND
          ${BASH_EXE} ${CMAKE_CURRENT_LIST_DIR}/miniconda.sh -b -p ${CMAKE_CURRENT_LIST_DIR}/miniconda)

  endif(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/conda")

  # use miniconda
  set(ENV{PATH} "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin:$ENV{PATH}")
  set(SEARCH_PATHS
      ${CMAKE_CURRENT_LIST_DIR}/miniconda
      ${CMAKE_CURRENT_LIST_DIR}/miniconda/bin)

  #------------------------------------------------------------------------------#
  #   local `conda`
  #------------------------------------------------------------------------------#
  find_program(CONDA_EXE
      NAMES           conda
      PATHS           ${SEARCH_PATHS}
      HINTS           ${SEARCH_PATHS}
      PATH_SUFFIXES   bin
      NO_DEFAULT_PATH)

  if("${CONDA_EXE}" STREQUAL "${DEFAULT_CONDA_EXE}")
      set(CONDA_EXE ${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/conda CACHE
          FILEPATH "Local conda installation" FORCE)
  endif()

  if(NOT CONDA_EXE OR NOT EXISTS "${CONDA_EXE}")
      message(FATAL_ERROR "Error! Could not find 'conda' command in \"${SEARCH_PATHS}\"")
  endif()

  get_filename_component(CONDA_DIR "${CONDA_EXE}" PATH)
  set(SEARCH_PATHS ${CONDA_DIR} ${SEARCH_PATHS})

  #------------------------------------------------------------------------------#
  #   local `python`
  #------------------------------------------------------------------------------#
  find_program(PYTHON_EXE
      NAMES           python
      PATHS           ${SEARCH_PATHS}
      HINTS           ${SEARCH_PATHS}
      PATH_SUFFIXES   bin
      NO_DEFAULT_PATH)

  if(NOT PYTHON_EXE OR NOT EXISTS "${PYTHON_EXE}")
      message(FATAL_ERROR "Error! Could not find 'python' command in \"${SEARCH_PATHS}\"")
  endif()

  #------------------------------------------------------------------------------#
  #   setup
  #------------------------------------------------------------------------------#
  if(CONDA_PACKAGE_INSTALL)
      execute_process(COMMAND ${CONDA_EXE} config --add channels conda-forge)
      execute_process(COMMAND ${CONDA_EXE} config --add channels jrmadsen)
      execute_process(COMMAND ${CONDA_EXE} config --set always_yes yes --set changeps1 no)
      execute_process(COMMAND ${CONDA_EXE} update conda)
      execute_process(COMMAND ${CONDA_EXE} install python=${PYTHON_VERSION} ${CONDA_PKGS})
      # update matplotlib and freetype due to MacOS issues with default
      execute_process(COMMAND ${CONDA_EXE} update matplotlib)
      execute_process(COMMAND ${CONDA_EXE} update freetype)
  endif()

tomopy.py
^^^^^^^^^

.. code:: python

  #!/usr/bin/env python
  # -*- coding: utf-8 -*-

  import sys
  import os
  import argparse
  import traceback

  import tomopy
  import dxchange
  import tornado
  import matplotlib
  import timemory
  import timemory.options as options
  import signal
  import numpy as np
  import time as t
  import pylab


  #------------------------------------------------------------------------------#
  algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                 'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]


  #------------------------------------------------------------------------------#
  def convert_image(fname, current_format, new_format):

      _fext = new_format
      _success = True

      try:

          from PIL import Image
          _cur_img = "{}.{}".format(fname, current_format)
          img = Image.open(_cur_img)
          out = img.convert("RGB")
          out.save(fname, "jpeg", quality=95)
          print("  --> Converted '{}' to {} format...".format(fname, new_format.upper()))

      except Exception as e:

          print("  --> ##### {}...".format(e))
          print("  --> ##### Exception occurred converting '{}' to {} format...".format(
              fname, new_format.upper()))

          _fext = current_format
          _success = False

      _fname = "{}.{}".format(fname, _fext)
      return [ _fname, _success, _fext ]


  #------------------------------------------------------------------------------#
  def exit_action(errcode):
      man = timemory.manager()
      timemory.report(ign_cutoff=True)
      fname = 'signal_error_{}.out'.format(errcode)
      f = open(fname, 'w')
      f.write('{}\n'.format(man))
      f.close()


  #------------------------------------------------------------------------------#
  def output_image(image, fname):

      img = pylab.imsave(fname, image, cmap='gray')

      if os.path.exists(fname):
          print("  --> Image file found @ '{}'...".format(fname))
      else:
          print("  ##################### WARNING #####################")
          print("  --> No image file at @ '{}' (expected) ...".format(fname))


  #------------------------------------------------------------------------------#
  def rescale_image(rec, nimages, scale, transform=True):

      rec_n = rec.copy()
      try:
          _min = np.amin(rec_n)
          rec_n -= _min
          _max = np.amax(rec_n)
          if _max > 0.0:
              rec_n /= 0.5 * _max
          rec_n -= 1
          import skimage.transform
          if transform is True:
              _nrows = rec[0].shape[0] * scale
              _ncols = rec[0].shape[1] * scale
              rec_tmp = np.ndarray([nimages, _nrows, _ncols])
              for i in range(nimages):
                  rec_tmp[i] = skimage.transform.resize(rec_n[i],
                      (rec_n[i].shape[0] * scale, rec_n[i].shape[1] * scale))
              rec_n = rec_tmp

      except Exception as e:
          print("  --> ##### {}...".format(e))
          rec_n = rec.copy()

      return rec_n


  #------------------------------------------------------------------------------#
  @timemory.util.auto_timer()
  def output_images(rec, nimages, full_basename, format="jpeg", scale=1, ncol=1):

      imgs = []
      nitr = 0
      rec_i = None
      fname = "{}".format(full_basename)

      rec_n = rec.copy()
      if scale > 1:
          rescale_image(rec, nimages, scale)

      print("Image size: {} x {} x {}".format(
          rec[0].shape[0],
          rec[0].shape[1],
          rec.shape[0]))

      print("Scaled Image size: {} x {} x {}".format(
          rec_n[0].shape[0],
          rec_n[0].shape[1],
          rec_n.shape[0]))

      for i in range(nimages):
          nitr += 1;

          _f = "{}{}".format(full_basename, i)
          _fimg = "{}.{}".format(_f, format)

          if rec_i is None:
              rec_i = rec_n[i]
          else:
              rec_i = np.concatenate((rec_i, rec_n[i]), axis=1)

          if nitr % ncol == 0 or i+1 == nimages:
              fname = "{}{}.{}".format(fname, i, format)
              output_image(rec_i, fname)
              imgs.append(fname)
              rec_i = None
              fname = "{}".format(full_basename)
          else:
              fname = "{}{}_".format(fname, i)

      return imgs


  #------------------------------------------------------------------------------#
  @timemory.util.auto_timer()
  def generate(nsize = 512, nangles = 360):

      with timemory.util.auto_timer("[tomopy.shepp3d]"):
          obj = tomopy.shepp3d(size=nsize)
      with timemory.util.auto_timer("[tomopy.angles]"):
          ang = tomopy.angles(nangles)
      with timemory.util.auto_timer("[tomopy.project]"):
          prj = tomopy.project(obj, ang)

      np.save('projection.npy', prj)
      np.save('angles.npy', ang)

      return [ prj, ang, obj ]


  #------------------------------------------------------------------------------#
  @timemory.util.auto_timer()
  def run(algorithm, size, nangles, ncores, format, scale, ncol, get_recon = False):

      nitr = size
      ndigits = 6
      imgs = []
      bname = os.path.join(algorithm, "stack_{}_".format(algorithm))
      bname = os.path.join(os.getcwd(), bname)

      prj, ang, obj = generate(size, nangles)

      with timemory.util.auto_timer("[tomopy.recon(algorithm='{}')]".format(algorithm)):
          rec = tomopy.recon(prj, ang, algorithm=algorithm, ncore=ncores)

      if get_recon is True:
          return rec

      imgs.extend(output_images(rec, nitr, bname, format, scale, ncol))

      return imgs


  #------------------------------------------------------------------------------#
  def main(args):

      manager = timemory.manager()

      algorithm = args.algorithm
      if len(args.compare) > 0:
          algorithm = "comparison"

      print('\nArguments:\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n'.format(
          "\tAlgorithm", algorithm,
          "\tSize", args.size,
          "\tAngles", args.angles,
          "\t# of cores", args.ncores,
          "\tFormat", args.format,
          "\tScale", args.scale,
          "\t# of columns", args.ncol,
          "\tcomparison", args.compare))

      if len(args.compare) > 0:
          args.ncol = 1
          args.scale = 1
          arr = None
          _nrows = None
          _ncols = None
          _nitr = 0
          seq = None
          for alg in args.compare:
              print("Reconstructing with {}...".format(alg))
              if seq is None:
                  seq = "{}".format(alg)
              else:
                  seq = "{}-{}".format(seq, alg)
              tmp = run(alg, args.size, args.angles, args.ncores,
                        args.format, args.scale, args.ncol, get_recon=True)
              tmp = rescale_image(tmp, args.size, args.scale, transform=False)
              _nrow = tmp[0].shape[0]
              _ncol = tmp[0].shape[1]
              if arr is None:
                  _nrows = _nrow
                  _ncols = _ncol * len(args.compare)
                  arr = np.ndarray([args.size, _nrows, _ncols], dtype=float)
              _b = (_ncol*_nitr)
              _nitr += 1
              _e = (_ncol*_nitr)
              arr[:, :, _b:_e] = tmp[:,:,:]
          #
          print("Total array size: {} x {} x {}".format(
              arr[0].shape[0],
              arr[0].shape[1],
              arr.shape[0]))

          fname = os.path.join(algorithm, "stack_{}_".format(seq))
          fname = os.path.join(os.getcwd(), fname)
          imgs = output_images(arr, args.size, fname,
                               args.format, args.scale, args.ncol)
      else:
          print("Reconstructing with {}...".format(args.algorithm))
          imgs = run(args.algorithm, args.size, args.angles, args.ncores,
                     args.format, args.scale, args.ncol)


      # timing report to stdout
      print('{}'.format(manager))

      timemory.options.output_dir = "./{}".format(algorithm)
      timemory.options.set_report("run_tomopy_{}.out".format(algorithm))
      timemory.options.set_serial("run_tomopy_{}.json".format(algorithm))
      manager.report()

      #------------------------------------------------------------------#
      # provide timing plots
      try:
          timemory.plotting.plot(files=[timemory.options.serial_filename], echo_dart=True)
      except Exception as e:
          print("Exception - {}".format(e))

      #------------------------------------------------------------------#
      # provide results to dashboard
      try:
          for i in range(0, len(imgs)):
              img_base = "{}_stack_".format(algorithm, i)
              img_name = os.path.basename(imgs[i]).replace(
                  ".{}".format(args.format), "").replace(
                  "stack_{}_".format(algorithm), img_base)
              img_type = args.format
              img_path = imgs[i]
              timemory.plotting.echo_dart_tag(img_name, img_path, img_type)
      except Exception as e:
          print("Exception - {}".format(e))

      #------------------------------------------------------------------#
      # provide ASCII results
      try:
          notes = manager.write_ctest_notes(directory="{}".format(algorithm))
          print('"{}" wrote CTest notes file : {}'.format(__file__, notes))
      except Exception as e:
          print("Exception - {}".format(e))

      modu_len = args.size % args.ncol
      real_len = len(imgs)
      if modu_len > 0:
          expt_len = (args.size / args.ncol) + 1
      else:
          expt_len = (args.size / args.ncol)
      if real_len != expt_len:
          exc_type, exc_value, exc_traceback = sys.exc_info()
          traceback.print_exception(exc_type, exc_value, exc_traceback, limit=5)
          msg = "Error! number of images ({}) does not match the expected total ({})!".format(
            expt_len, real_len)
          raise Exception(msg)


  #------------------------------------------------------------------------------#
  if __name__ == "__main__":

      parser = argparse.ArgumentParser()

      import multiprocessing as mp
      ncores = mp.cpu_count()

      parser.add_argument("-a", "--algorithm", help="Select the algorithm",
          default="gridrec", choices=algorithms, type=str)
      parser.add_argument("-A", "--angles", help="number of angles",
          default=360, type=int)
      parser.add_argument("-s", "--size", help="size of image",
          default=512, type=int)
      parser.add_argument("-n", "--ncores", help="number of cores",
          default=ncores, type=int)
      parser.add_argument("-f", "--format", help="output image format",
          default="jpeg", type=str)
      parser.add_argument("-S", "--scale", help="scale image by a positive factor",
          default=1, type=int)
      parser.add_argument("-c", "--ncol", help="Number of images per row",
          default=1, type=int)
      parser.add_argument("--compare", help="Generate comparison",
          nargs='*', default=["none"], type=str)

      args = timemory.options.add_args_and_parse_known(parser)

      if len(args.compare) == 1 and args.compare[0].lower() == "all":
          args.compare = list(algorithms)
      elif len(args.compare) == 1:
          args.compare = []

      if len(args.compare) == 0:
          try:
              import shutil
              dir = os.path.join(os.getcwd(), args.algorithm)
              shutil.rmtree(dir)
              os.makedirs(dir)
          except:
              pass
      else:
          try:
              import shutil
              dir = os.path.join(os.getcwd(), "comparison")
              shutil.rmtree(dir)
              os.makedirs(dir)
          except:
              pass


      ret = 0
      try:

          with timemory.util.timer('\nTotal time for "{}"'.format(__file__)):
              main(args)

      except Exception as e:
          exc_type, exc_value, exc_traceback = sys.exc_info()
          traceback.print_exception(exc_type, exc_value, exc_traceback, limit=5)
          print ('Exception - {}'.format(e))
          ret = 2
          os.kill(os.getpid(), signal.SIGHUP)

      sys.exit(ret)

run\_tomopy.py
^^^^^^^^^^^^^^

.. code:: python

  #!/usr/bin/env python
  # -*- coding: utf-8 -*-

  import sys
  import os
  import argparse
  import traceback

  import tomopy
  import dxchange
  import tornado
  import matplotlib
  import timemory
  import timemory.options as options
  import signal
  import numpy as np
  import time as t
  import pylab


  #------------------------------------------------------------------------------#
  algorithms = [ 'gridrec', 'art', 'fbp', 'bart', 'mlem', 'osem', 'sirt',
                 'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad' ]


  #------------------------------------------------------------------------------#
  def convert_image(fname, current_format, new_format):

      _fext = new_format
      _success = True

      try:

          from PIL import Image
          _cur_img = "{}.{}".format(fname, current_format)
          img = Image.open(_cur_img)
          out = img.convert("RGB")
          out.save(fname, "jpeg", quality=95)
          print("  --> Converted '{}' to {} format...".format(fname, new_format.upper()))

      except Exception as e:

          print("  --> ##### {}...".format(e))
          print("  --> ##### Exception occurred converting '{}' to {} format...".format(
              fname, new_format.upper()))

          _fext = current_format
          _success = False

      _fname = "{}.{}".format(fname, _fext)
      return [ _fname, _success, _fext ]


  #------------------------------------------------------------------------------#
  def exit_action(errcode):
      man = timemory.manager()
      timemory.report(ign_cutoff=True)
      fname = 'signal_error_{}.out'.format(errcode)
      f = open(fname, 'w')
      f.write('{}\n'.format(man))
      f.close()


  #------------------------------------------------------------------------------#
  def output_image(image, fname):

      img = pylab.imsave(fname, image, cmap='gray')

      if os.path.exists(fname):
          print("  --> Image file found @ '{}'...".format(fname))
      else:
          print("  ##################### WARNING #####################")
          print("  --> No image file at @ '{}' (expected) ...".format(fname))


  #------------------------------------------------------------------------------#
  def rescale_image(rec, nimages, scale, transform=True):

      rec_n = rec.copy()
      try:
          _min = np.amin(rec_n)
          rec_n -= _min
          _max = np.amax(rec_n)
          if _max > 0.0:
              rec_n /= 0.5 * _max
          rec_n -= 1
          import skimage.transform
          if transform is True:
              _nrows = rec[0].shape[0] * scale
              _ncols = rec[0].shape[1] * scale
              rec_tmp = np.ndarray([nimages, _nrows, _ncols])
              for i in range(nimages):
                  rec_tmp[i] = skimage.transform.resize(rec_n[i],
                      (rec_n[i].shape[0] * scale, rec_n[i].shape[1] * scale))
              rec_n = rec_tmp

      except Exception as e:
          print("  --> ##### {}...".format(e))
          rec_n = rec.copy()

      return rec_n


  #------------------------------------------------------------------------------#
  @timemory.util.auto_timer()
  def output_images(rec, nimages, full_basename, format="jpeg", scale=1, ncol=1):

      imgs = []
      nitr = 0
      rec_i = None
      fname = "{}".format(full_basename)

      rec_n = rec.copy()
      if scale > 1:
          rescale_image(rec, nimages, scale)

      print("Image size: {} x {} x {}".format(
          rec[0].shape[0],
          rec[0].shape[1],
          rec.shape[0]))

      print("Scaled Image size: {} x {} x {}".format(
          rec_n[0].shape[0],
          rec_n[0].shape[1],
          rec_n.shape[0]))

      for i in range(nimages):
          nitr += 1;

          _f = "{}{}".format(full_basename, i)
          _fimg = "{}.{}".format(_f, format)

          if rec_i is None:
              rec_i = rec_n[i]
          else:
              rec_i = np.concatenate((rec_i, rec_n[i]), axis=1)

          if nitr % ncol == 0 or i+1 == nimages:
              fname = "{}{}.{}".format(fname, i, format)
              output_image(rec_i, fname)
              imgs.append(fname)
              rec_i = None
              fname = "{}".format(full_basename)
          else:
              fname = "{}{}_".format(fname, i)

      return imgs


  #------------------------------------------------------------------------------#
  @timemory.util.auto_timer()
  def generate(nsize = 512, nangles = 360):

      with timemory.util.auto_timer("[tomopy.shepp3d]"):
          obj = tomopy.shepp3d(size=nsize)
      with timemory.util.auto_timer("[tomopy.angles]"):
          ang = tomopy.angles(nangles)
      with timemory.util.auto_timer("[tomopy.project]"):
          prj = tomopy.project(obj, ang)

      np.save('projection.npy', prj)
      np.save('angles.npy', ang)

      return [ prj, ang, obj ]


  #------------------------------------------------------------------------------#
  @timemory.util.auto_timer()
  def run(algorithm, size, nangles, ncores, format, scale, ncol, get_recon = False):

      nitr = size
      ndigits = 6
      imgs = []
      bname = os.path.join(algorithm, "stack_{}_".format(algorithm))
      bname = os.path.join(os.getcwd(), bname)

      prj, ang, obj = generate(size, nangles)

      with timemory.util.auto_timer("[tomopy.recon(algorithm='{}')]".format(algorithm)):
          rec = tomopy.recon(prj, ang, algorithm=algorithm, ncore=ncores)

      if get_recon is True:
          return rec

      imgs.extend(output_images(rec, nitr, bname, format, scale, ncol))

      return imgs


  #------------------------------------------------------------------------------#
  def main(args):

      manager = timemory.manager()

      algorithm = args.algorithm
      if len(args.compare) > 0:
          algorithm = "comparison"

      print('\nArguments:\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n{} = {}\n'.format(
          "\tAlgorithm", algorithm,
          "\tSize", args.size,
          "\tAngles", args.angles,
          "\t# of cores", args.ncores,
          "\tFormat", args.format,
          "\tScale", args.scale,
          "\t# of columns", args.ncol,
          "\tcomparison", args.compare))

      if len(args.compare) > 0:
          args.ncol = 1
          args.scale = 1
          arr = None
          _nrows = None
          _ncols = None
          _nitr = 0
          seq = None
          for alg in args.compare:
              print("Reconstructing with {}...".format(alg))
              if seq is None:
                  seq = "{}".format(alg)
              else:
                  seq = "{}-{}".format(seq, alg)
              tmp = run(alg, args.size, args.angles, args.ncores,
                        args.format, args.scale, args.ncol, get_recon=True)
              tmp = rescale_image(tmp, args.size, args.scale, transform=False)
              _nrow = tmp[0].shape[0]
              _ncol = tmp[0].shape[1]
              if arr is None:
                  _nrows = _nrow
                  _ncols = _ncol * len(args.compare)
                  arr = np.ndarray([args.size, _nrows, _ncols], dtype=float)
              _b = (_ncol*_nitr)
              _nitr += 1
              _e = (_ncol*_nitr)
              arr[:, :, _b:_e] = tmp[:,:,:]
          #
          print("Total array size: {} x {} x {}".format(
              arr[0].shape[0],
              arr[0].shape[1],
              arr.shape[0]))

          fname = os.path.join(algorithm, "stack_{}_".format(seq))
          fname = os.path.join(os.getcwd(), fname)
          imgs = output_images(arr, args.size, fname,
                               args.format, args.scale, args.ncol)
      else:
          print("Reconstructing with {}...".format(args.algorithm))
          imgs = run(args.algorithm, args.size, args.angles, args.ncores,
                     args.format, args.scale, args.ncol)


      # timing report to stdout
      print('{}'.format(manager))

      timemory.options.output_dir = "./{}".format(algorithm)
      timemory.options.set_report("run_tomopy_{}.out".format(algorithm))
      timemory.options.set_serial("run_tomopy_{}.json".format(algorithm))
      manager.report()

      #------------------------------------------------------------------#
      # provide timing plots
      try:
          timemory.plotting.plot(files=[timemory.options.serial_filename], echo_dart=True)
      except Exception as e:
          print("Exception - {}".format(e))

      #------------------------------------------------------------------#
      # provide results to dashboard
      try:
          for i in range(0, len(imgs)):
              img_base = "{}_stack_".format(algorithm, i)
              img_name = os.path.basename(imgs[i]).replace(
                  ".{}".format(args.format), "").replace(
                  "stack_{}_".format(algorithm), img_base)
              img_type = args.format
              img_path = imgs[i]
              timemory.plotting.echo_dart_tag(img_name, img_path, img_type)
      except Exception as e:
          print("Exception - {}".format(e))

      #------------------------------------------------------------------#
      # provide ASCII results
      try:
          notes = manager.write_ctest_notes(directory="{}".format(algorithm))
          print('"{}" wrote CTest notes file : {}'.format(__file__, notes))
      except Exception as e:
          print("Exception - {}".format(e))

      modu_len = args.size % args.ncol
      real_len = len(imgs)
      if modu_len > 0:
          expt_len = (args.size / args.ncol) + 1
      else:
          expt_len = (args.size / args.ncol)
      if real_len != expt_len:
          exc_type, exc_value, exc_traceback = sys.exc_info()
          traceback.print_exception(exc_type, exc_value, exc_traceback, limit=5)
          msg = "Error! number of images ({}) does not match the expected total ({})!".format(
            expt_len, real_len)
          raise Exception(msg)


  #------------------------------------------------------------------------------#
  if __name__ == "__main__":

      parser = argparse.ArgumentParser()

      import multiprocessing as mp
      ncores = mp.cpu_count()

      parser.add_argument("-a", "--algorithm", help="Select the algorithm",
          default="gridrec", choices=algorithms, type=str)
      parser.add_argument("-A", "--angles", help="number of angles",
          default=360, type=int)
      parser.add_argument("-s", "--size", help="size of image",
          default=512, type=int)
      parser.add_argument("-n", "--ncores", help="number of cores",
          default=ncores, type=int)
      parser.add_argument("-f", "--format", help="output image format",
          default="jpeg", type=str)
      parser.add_argument("-S", "--scale", help="scale image by a positive factor",
          default=1, type=int)
      parser.add_argument("-c", "--ncol", help="Number of images per row",
          default=1, type=int)
      parser.add_argument("--compare", help="Generate comparison",
          nargs='*', default=["none"], type=str)

      args = timemory.options.add_args_and_parse_known(parser)

      if len(args.compare) == 1 and args.compare[0].lower() == "all":
          args.compare = list(algorithms)
      elif len(args.compare) == 1:
          args.compare = []

      if len(args.compare) == 0:
          try:
              import shutil
              dir = os.path.join(os.getcwd(), args.algorithm)
              shutil.rmtree(dir)
              os.makedirs(dir)
          except:
              pass
      else:
          try:
              import shutil
              dir = os.path.join(os.getcwd(), "comparison")
              shutil.rmtree(dir)
              os.makedirs(dir)
          except:
              pass


      ret = 0
      try:

          with timemory.util.timer('\nTotal time for "{}"'.format(__file__)):
              main(args)

      except Exception as e:
          exc_type, exc_value, exc_traceback = sys.exc_info()
          traceback.print_exception(exc_type, exc_value, exc_traceback, limit=5)
          print ('Exception - {}'.format(e))
          ret = 2
          os.kill(os.getpid(), signal.SIGHUP)

      sys.exit(ret)

