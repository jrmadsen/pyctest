# Basic example

- Setup some tests and run `ctest` in `basic.py`
- Use `check_env.py` to verify environment variables set by property

```bash
# create amd run ctests
$ python ./basic.py
```

### Output

```bash
Running cmake with project role
-- The C compiler identification is GNU 7.3.0
-- Checking whether C compiler has -isysroot
-- Checking whether C compiler has -isysroot - yes
-- Checking whether C compiler supports OSX deployment target flag
-- Checking whether C compiler supports OSX deployment target flag - yes
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - failed
-- Detecting C compile features
-- Detecting C compile features - failed
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-example
Found test "make_test_directory" with RUN_SERIAL property = 0
Found test "make_test_directory" with RUN_SERIAL property = ON
Found test "list_directories" with RUN_SERIAL property = 0
Found test "list_directories" with RUN_SERIAL property = ON
Found test "print_hostname" with RUN_SERIAL property = 0
Found test "print_hostname" with RUN_SERIAL property = ON
Found test "check_environment" with RUN_SERIAL property = 0
Found test "check_environment" with RUN_SERIAL property = ON
Writing CTest test file: "pycm-example/CTestTestfile.cmake"...
Generating test "make_test_directory"...
Generating test "list_directories"...
Generating test "print_hostname"...
Generating test "check_environment"...
UpdateCTestConfiguration  from :/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-example/DartConfiguration.tcl
Parse Config file:/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-example/DartConfiguration.tcl
 Add coverage exclude regular expressions.
SetCTestConfiguration:CMakeCommand:/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pyctest/bin/cmake
UpdateCTestConfiguration  from :/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-example/DartConfiguration.tcl
Parse Config file:/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-example/DartConfiguration.tcl
Test project /Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/pycm-example
Constructing a list of tests
Done constructing a list of tests
Updating test list for fixtures
Added 0 tests to meet fixture requirements
Checking test dependency graph...
Checking test dependency graph end
test 1
    Start 1: make_test_directory

1: Test command: /bin/mkdir "-p" "-v" "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/test_dir"
1: Environment variables:
1:  EXAMPLE_ENV_A=Successfully set example environment variable A
1:  EXAMPLE_ENV_B=Successfully set example environment variable B
1: Test timeout computed to be: 1500
1/4 Test #1: make_test_directory ..............   Passed    0.00 sec
test 2
    Start 2: list_directories

2: Test command: /bin/ls "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug"
2: Environment variables:
2:  EXAMPLE_ENV_A=Successfully set example environment variable A
2:  EXAMPLE_ENV_B=Successfully set example environment variable B
2: Test timeout computed to be: 1500
2: CMakeCache.txt
2: CMakeFiles
2: CMakeLibBuild.cmake
2: CPackConfig.cmake
2: CPackSourceConfig.cmake
2: CTestConfig.cmake
2: CTestCustom.cmake
2: CTestLibBuild.cmake
2: CTestTestfile.cmake
2: DartConfiguration.tcl
2: Testing
2: basic.py
2: build.ninja
2: check_env.py
2: cmake_install.cmake
2: examples
2: install_manifest.txt
2: pycm-example
2: pycm-test
2: pyctest
2: pyctest.cbp
2: rules.ninja
2: run_pycmake.py
2: source
2: test.py
2: test_dir
2: test_output
2/4 Test #2: list_directories .................   Passed    0.00 sec
test 3
    Start 3: print_hostname

3: Test command: /bin/hostname
3: Environment variables:
3:  EXAMPLE_ENV_A=Successfully set example environment variable A
3:  EXAMPLE_ENV_B=Successfully set example environment variable B
3: Test timeout computed to be: 1500
3: JRM-macOS-DOE.local
3/4 Test #3: print_hostname ...................   Passed    0.00 sec
test 4
    Start 4: check_environment

4: Test command: /opt/local/bin/python "/Users/jrmadsen/devel/c++/qt-pyctest/clang-Debug/check_env.py"
4: Environment variables:
4:  EXAMPLE_ENV_A=Successfully set example environment variable A
4:  EXAMPLE_ENV_B=Successfully set example environment variable B
4: Test timeout computed to be: 1500
4: EXAMPLE_ENV_A is 'Successfully set example environment variable A'
4: EXAMPLE_ENV_B is 'Successfully set example environment variable B'
4/4 Test #4: check_environment ................   Passed    0.06 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.07 sec
```
