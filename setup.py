#!/usr/bin/env python

import os
import re
import sys
import sysconfig
import platform
import subprocess
import shutil

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools import Command
from setuptools.command.test import test as TestCommand
from setuptools.command.install_egg_info import install_egg_info


# ---------------------------------------------------------------------------- #
#   Setup the compilers
#
if platform.system() == "Darwin":
    # force Clang on macOS
    os.environ["CC"] = "/usr/bin/clang"
    os.environ["CXX"] = "/usr/bin/clang++"

elif platform.system() == "Linux":
    # choose GCC if not set
    if os.environ.get("CC") is None and os.path.exists("/usr/bin/gcc"):
        os.environ["CC"] = "/usr/bin/gcc"
    if os.environ.get("CXX") is None and os.path.exists("/usr/bin/g++"):
        os.environ["CXX"] = "/usr/bin/g++"
    # make sure we don't use clang
    if os.environ.get("CC") is not None:
        if re.search(r'clang', os.environ.get("CC")) is not None:
            os.environ["CC"] = "/usr/bin/gcc"
    # make sure we don't use clang++
    if os.environ.get("CXX") is not None:
        if re.search(r'clang', os.environ.get("CXX")) is not None:
            os.environ["CC"] = "/usr/bin/g++"


# ---------------------------------------------------------------------------- #
#   work around to avoid using disttools.LooseVersion
#
def get_integer_version(version_string):
    version_array = version_string.split('.')
    # if more than 3 version numbers
    while len(version_array) > 3:
        version_array.pop()
    # if less than 3 version numbers
    while len(version_array) < 3:
        version_array.append('0')
    integer_version = 0
    factors = [ 100000, 1000, 1 ]
    for i in range(0, len(factors)):
        integer_version += factors[i] * int(version_array[i])
    return integer_version


# ---------------------------------------------------------------------------- #
#   work around to avoid using disttools.LooseVersion
#
def get_string_version(version_integer):
    major_ver = "{}".format(int(version_integer / 100000))
    minor_ver = "{}".format(int((version_integer % 100000) / 1000))
    patch_ver = "{}".format(int(version_integer % 1000))
    return "{}.{}.{}".format(major_ver, minor_ver, patch_ver)


# ---------------------------------------------------------------------------- #
#
def get_project_version():
    # open ".version.txt"
    with open(os.path.join(os.getcwd(), 'VERSION'), 'r') as f:
        data = f.read().replace('\n', '')
    # make sure is string
    if isinstance(data, list) or isinstance(data, tuple):
        return data[0]
    else:
        return data


# ---------------------------------------------------------------------------- #
#
class CMakeExtension(Extension):

    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


# ---------------------------------------------------------------------------- #
#
class CMakeBuild(build_ext, Command):

    cmake_version = get_integer_version('2.7.12')
    cmake_min_version = get_integer_version('2.8.12')
    build_type = 'Release'
    cxx_standard = 11
    cmake_prefix_path = ''
    cmake_include_path = ''
    cmake_library_path = ''
    pybind11_install = 'OFF'
    devel_install = 'ON'


    #--------------------------------------------------------------------------#
    def check_env(self, var, key):
        ret = var
        try:
            ret = os.environ[key]
        except:
            pass
        return ret


    #--------------------------------------------------------------------------#
    def cmake_version_error(self):
        """
        Raise exception about CMake
        """
        ma = 'Error finding/putting cmake in path. Either no CMake found or no cmake module.'
        mb = 'This error can commonly be resolved with "{}"'.format("pip install -U pip cmake")
        mc = 'CMake version found: {}'.format(get_string_version(CMakeBuild.cmake_version))
        md = "CMake must be installed to build the following extensions: " + \
        ", ".join(e.name for e in self.extensions)
        mt = '\n\n\t{}\n\t{}\n\t{}\n\t{}\n\n'.format(ma, mb, mc, md)
        raise RuntimeError(mt)


    #--------------------------------------------------------------------------#
    def init_cmake(self):
        """
        Ensure cmake is in PATH
        """
        try:
            out = subprocess.check_output(['cmake', '--version'])
            CMakeBuild.cmake_version = get_integer_version(
                re.search(r'version\s*([\d.]+)', out.decode()).group(1))
        except OSError:
            # if fail, try the module
            import cmake
            try:
                if not cmake.CMAKE_BIN_DIR in sys.path:
                    sys.path.append(cmake.CMAKE_BIN_DIR)
                if platform.system() != "Windows":
                    curr_path = os.environ['PATH']
                    if not cmake.CMAKE_BIN_DIR in curr_path:
                        os.environ['PATH'] = "{}:{}".format(curr_path, cmake.CMAKE_BIN_DIR)

                CMakeBuild.cmake_version = cmake.sys.version.split(' ')[0]
            except:
                self.cmake_version_error()


    #--------------------------------------------------------------------------#
    # run
    def run(self):
        self.init_cmake()

        print ('Using CMake version {}...'.format(
            get_string_version(CMakeBuild.cmake_version)))

        if CMakeBuild.cmake_version < CMakeBuild.cmake_min_version:
            raise RuntimeError("CMake >= {} is required. Found CMake version {}".format(
                               get_string_version(CMakeBuild.cmake_min_version),
                               get_string_version(CMakeBuild.cmake_version)))

        for ext in self.extensions:
            self.build_extension(ext)


    #--------------------------------------------------------------------------#
    # build extension
    def build_extension(self, ext):

        self.init_cmake()

        # check function for setup.cfg
        def valid_string(_str):
            if len(_str) > 0 and _str != '""' and _str != "''":
                return True
            return False

        # allow environment to over-ride setup.cfg
        # options are prefixed with PYCTEST_ if not already
        def compose(str):
            return 'PYCTEST_{}'.format(str.upper())

        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))

        # Always the same
        cmake_args = ['-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DPYCTEST_SETUP_PY=ON',
                      ]

        #----------------------------------------------------------------------#
        #
        #   Process options
        #
        #----------------------------------------------------------------------#
        self.build_type = self.check_env(self.build_type, compose("build_type"))
        self.cxx_standard = self.check_env(self.cxx_standard, compose("cxx_standard"))
        self.cmake_prefix_path = self.check_env(self.cmake_prefix_path, compose("cmake_prefix_path"))
        self.cmake_include_path = self.check_env(self.cmake_include_path, compose("cmake_include_path"))
        self.cmake_library_path = self.check_env(self.cmake_library_path, compose("cmake_library_path"))
        self.pybind11_install = self.check_env(self.pybind11_install, compose("pybind11_install"))
        self.devel_install = self.check_env(self.devel_install, compose("devel_install"))

        _valid_type = False
        for _type in [ 'Release', 'Debug', 'RelWithDebInfo', 'MinSizeRel' ]:
            if _type == self.build_type:
                _valid_type = True
                break

        if not _valid_type:
            self.build_type = 'Release'

        cmake_args += [ '-DCMAKE_BUILD_TYPE={}'.format(self.build_type) ]
        cmake_args += [ '-DBUILD_SHARED_LIBS=OFF' ]

        _cxxstd = int(self.cxx_standard)
        if _cxxstd < 14 and platform.system() == "Windows":
            # unique_ptr support
            _cxxstd = 14
            self.cxx_standard = '{}'.format(_cxxstd)

        if _cxxstd == 11 or _cxxstd == 14 or _cxxstd == 17:
            cmake_args += [ '-DCMAKE_CXX_STANDARD={}'.format(self.cxx_standard) ]

        if valid_string(self.cmake_prefix_path):
            cmake_args += [ '-DCMAKE_PREFIX_PATH={}'.format(self.cmake_prefix_path) ]

        if valid_string(self.cmake_library_path):
            cmake_args += [ '-DCMAKE_LIBRARY_PATH={}'.format(self.cmake_library_path) ]

        if valid_string(self.cmake_include_path):
            cmake_args += [ '-DCMAKE_INCLUDE_PATH={}'.format(self.cmake_include_path) ]

        cmake_args += [ '-DCMAKE_INSTALL_PREFIX={}'.format(os.path.join(extdir, "pyctest")) ]
        cmake_args += [ '-DPYBIND11_INSTALL={}'.format(str.upper(self.pybind11_install)) ]

        build_args = [ '--config', self.build_type ]
        install_args = [ '-DBUILD_TYPE={}'.format(self.build_type),
                         '-P', 'cmake_install.cmake' ]

        env_arch = os.environ.get("PYTHON_ARCH")
        if platform.system() == "Windows":
            if platform.architecture()[0] == '64bit':
                cmake_args += ['-A', 'x64']
            elif env_arch is None:
                if sys.maxsize > 2**32:
                    cmake_args += ['-A', 'x64']
            elif env_arch is not None and env_arch == "64":
                cmake_args += ['-A', 'x64']
            cmake_args += ['-DCMake_MSVC_PARALLEL=ON']
            build_args += ['--target', 'ALL_BUILD', '--', '/m' ]
        else:
            nproc = '-j4'
            try:
                import multiprocessing as mp
                ncpu = mp.cpu_count()
                if ncpu > 8:
                    ncpu = 8
                nproc = '-j{}'.format(ncpu+1)
            except:
                pass
            build_args += [ '--', nproc ]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{}'.format(env.get('CXXFLAGS', ''))

        # make directory if not exist
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        # set to absolute path
        self.build_temp=os.path.abspath(self.build_temp)

        # print the CMake args
        print('CMake args: {}'.format(cmake_args))
        # print the build_args
        print('Build args: {}'.format(build_args))
        # print the install args
        print('Install args: {}'.format(install_args))

        # configure the project
        subprocess.check_call(['cmake'] + cmake_args + [ ext.sourcedir ],
                              cwd=self.build_temp, env=env)

        # build the project
        subprocess.check_call(['cmake', '--build', self.build_temp] + build_args,
                              cwd=self.build_temp, env=env)

        # install the CMake build
        subprocess.check_call(['cmake', '-DCOMPONENT=python' ] + install_args,
                              cwd=self.build_temp, env=env)

        # install the development
        subprocess.check_call(['cmake', '-DCOMPONENT=development' ] + install_args,
                              cwd=self.build_temp, env=env)

        CMakeInstallEggInfo.dirs[self.build_temp] = extdir

        pyctestdir = os.path.join(extdir, "pyctest")
        initpath = os.path.join(pyctestdir, "__init__.py")
        if not os.path.exists(initpath):
            f = open(initpath, "w")
            if os.path.exists(".license.py"):
                lic = open(".license.py")
                contents = lic.read()
                f.write(contents)
            else:
                f.write("#\n#")
            f.close()

        print()  # Add an empty line for cleaner output


# ---------------------------------------------------------------------------- #
#
class CMakeTest(TestCommand):
    """
    A custom test runner to execute both Python unittest tests and C++ Catch-
    lib tests.
    """

    cmake_version = get_integer_version('2.7.12')
    cmake_min_version = get_integer_version('2.8.12')


    #--------------------------------------------------------------------------#
    def ctest_version_error(self):
        """
        Raise exception about CMake
        """
        ma = 'Error finding/putting ctest in path. Either no CMake found or no cmake module.'
        mb = 'This error can commonly be resolved with "{}"'.format("pip install -U pip cmake")
        mc = 'CMake version found: {}'.format(get_string_version(CMakeTest.ctest_version))
        md = "CMake must be installed to build the following extensions: " + \
        ", ".join(e.name for e in self.extensions)
        mt = '\n\n\t{}\n\t{}\n\t{}\n\t{}\n\n'.format(ma, mb, mc, md)
        raise RuntimeError(mt)


    #--------------------------------------------------------------------------#
    def init_ctest(self):
        """
        Ensure ctest is in PATH
        """
        try:
            out = subprocess.check_output(['ctest', '--version'])
            CMakeBuild.cmake_version = get_integer_version(
                re.search(r'version\s*([\d.]+)', out.decode()).group(1))
        except OSError:
            # if fail, try the module
            import cmake
            try:
                if not cmake.CMAKE_BIN_DIR in sys.path:
                    sys.path.append(cmake.CMAKE_BIN_DIR)
                if platform.system() != "Windows":
                    curr_path = os.environ['PATH']
                    if not cmake.CMAKE_BIN_DIR in curr_path:
                        os.environ['PATH'] = "{}:{}".format(curr_path, cmake.CMAKE_BIN_DIR)

                CMakeTest.ctest_version = cmake.sys.version.split(' ')[0]
            except:
                self.ctest_version_error()


    #--------------------------------------------------------------------------#
    def distutils_dir_name(self, dname):
        """Returns the name of a distutils build directory"""
        dir_name = "{dirname}.{platform}-{version[0]}.{version[1]}"
        return dir_name.format(dirname=dname,
                               platform=sysconfig.get_platform(),
                               version=sys.version_info)


    #--------------------------------------------------------------------------#
    def run(self):
        self.init_ctest()
        print("\nRunning CMake/CTest tests...\n")
        # Run catch tests
        subprocess.check_call(['ctest', '--output-on-failure' ],
                cwd=os.path.join('build', self.distutils_dir_name('temp')))


# ---------------------------------------------------------------------------- #
#
class CMakeInstallEggInfo(install_egg_info):

    dirs = {}
    files = []

    #--------------------------------------------------------------------------#
    def run(self):
        install_egg_info.run(self)

        for f in CMakeInstallEggInfo.files:
            print ('Adding "{}"...'.format(f))
            self.outputs.append(f)

        # read the install manifest from CMake
        for tmpdir, libdir in CMakeInstallEggInfo.dirs.items():
            for manifest in [ 'install_manifest.txt',
                              'install_manifest_development.txt',
                              'install_manifest_python.txt' ]:
                fname = os.path.join(tmpdir, manifest)
                if not os.path.exists(fname):
                    continue
                f = open(fname, 'r')
                if libdir[len(libdir)-1] != '/':
                    libdir += '/'
                for l in f.read().splitlines():
                    b = l.replace(libdir, '')
                    f = os.path.join(self.install_dir, b)
                    print ('Adding "{}"...'.format(f))
                    self.outputs.append(f)


# ---------------------------------------------------------------------------- #
#
def get_long_description():
    long_descript = ''
    try:
        long_descript = open('README.rst').read()
    except:
        long_descript = ''
    return long_descript


# ---------------------------------------------------------------------------- #
#
def get_short_description():
    part_a = "Python wrappers for generating CTest and submitting to CDash"
    part_b = "without a CMake build system"
    return "{} {}".format(part_a, part_b)


# ---------------------------------------------------------------------------- #
#
def get_keywords():
    return [ 'cmake', 'ctest', 'pybind11' ]


# ---------------------------------------------------------------------------- #
#
def get_classifiers():
    return [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Topic :: Utilities',
        'Natural Language :: English',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: Implementation :: CPython',
    ]


# ---------------------------------------------------------------------------- #
#
def get_name():
    return 'Jonathan R. Madsen'


# ---------------------------------------------------------------------------- #
#
def get_email():
    return 'jonrobm.programming@gmail.com'


# ---------------------------------------------------------------------------- #
# calls the setup and declare package
#
setup(name='pyctest',
    version=get_project_version(),
    author=get_name(),
    author_email=get_email(),
    maintainer=get_name(),
    maintainer_email=get_email(),
    contact=get_name(),
    contact_email=get_email(),
    description=get_short_description(),
    long_description=get_long_description(),
    url='https://github.com/jrmadsen/pyctest.git',
    license='MIT',
    # add extension module
    ext_modules=[CMakeExtension('pyctest')],
    # add custom build_ext command
    cmdclass=dict(build_ext=CMakeBuild,
                  test=CMakeTest,
                  install_egg_info=CMakeInstallEggInfo),
    zip_safe=False,
    # extra
    install_requires=[ ],
    setup_requires=[ ],
    packages=[ ],
    keywords=get_keywords(),
    classifiers=get_classifiers(),
    python_requires='>=2.6',
    data_files=[ ],
)

