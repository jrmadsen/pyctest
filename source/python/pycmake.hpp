//  MIT License
//  
//  Copyright (c) 2018, The Regents of the University of California, 
//  through Lawrence Berkeley National Laboratory (subject to receipt of any
//  required approvals from the U.S. Dept. of Energy).  All rights reserved.
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#ifndef pycmake_hpp_
#define pycmake_hpp_

//============================================================================//

#include "cmAlgorithms.h"
#include "cmDocumentationEntry.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmState.h"
#include "cmStateTypes.h"
#include "cmSystemTools.h"
#include "cmake.h"
#include "cmcmd.h"

#include "cmDocumentation.h"
#include "cmDynamicLoader.h"

#include "cm_uv.h"

#include "cmsys/Encoding.hxx"
#if defined(_WIN32)
#  include "cmsys/ConsoleBuf.hxx"
#endif

//============================================================================//

#include <ctype.h>
#include <string.h>
#include <future>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <memory>

//============================================================================//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <pybind11/iostream.h>
#include <pybind11/eval.h>
#include <pybind11/embed.h>
#include <pybind11/cast.h>
#include <pybind11/pytypes.h>
#include <pybind11/numpy.h>

//============================================================================//

#include "cmState.h"
#include "cmake.h"
#include "cmMakefile.h"
#include "cmDefinitions.h"
#include "cmTest.h"
#include "cmTestGenerator.h"

//============================================================================//

namespace py = pybind11;
using namespace py::literals;
using namespace std::placeholders;  // for _1, _2, _3...

//============================================================================//

namespace pycm
{

int do_command(int ac, char const* const* av);
cmMakefile* cmakemainGetMakefile(void* clientdata);
std::string cmakemainGetStack(void* clientdata);
void cmakemainMessageCallback(const char* m, const char*,
                              bool&, void* clientdata);
void cmakemainProgressCallback(const char* m, float prog,
                               void* clientdata);
int cmake_main(int ac, char const* const* av);
int do_cmake(int ac, char const* const* av);
int do_build(int ac, char const* const* av);
int do_open(int ac, char const* const* av);

cmake* cmake_instance()
{
    static cmake* _instance = nullptr;
    if(_instance)
        _instance = new cmake(cmake::Role::RoleProject);
    return _instance;
}

typedef std::unique_ptr<cmake, py::nodelete> unique_cm_t;

}

//----------------------------------------------------------------------------//

#endif

