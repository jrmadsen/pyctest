pyctest
=======

Example
~~~~~~~

Generates ``test_output/CTestTestfile.cmake``

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

