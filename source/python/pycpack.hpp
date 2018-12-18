//  MIT License
//
//  Copyright (c) 2018, The Regents of the University of California,
//  through Lawrence Berkeley National Laboratory (subject to receipt of any
//  required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#ifndef pycpack_hpp_
#define pycpack_hpp_

//============================================================================//

#include "cmCPackGenerator.h"
#include "cmCPackGeneratorFactory.h"
#include "cmCPackLog.h"
#include "cmDocumentation.h"
#include "cmDocumentationEntry.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cmake.h"
#include "cmsys/CommandLineArguments.hxx"
#include "cmsys/Encoding.hxx"

#if defined(_WIN32) && defined(CMAKE_BUILD_WITH_CMAKE)
#    include "cmsys/ConsoleBuf.hxx"
#endif

//============================================================================//

#include <atomic>
#include <chrono>
#include <cstdint>
#include <ctype.h>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <stddef.h>
#include <string.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>

//============================================================================//

#include <pybind11/cast.h>
#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

//============================================================================//

#include "cmCPackGenerator.h"
#include "cmCPackGeneratorFactory.h"
#include "cmCPackLog.h"
#include "cmDocumentation.h"
#include "cmDocumentationEntry.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cmake.h"

//============================================================================//

namespace py = pybind11;
using namespace py::literals;
using namespace std::placeholders;  // for _1, _2, _3...

//============================================================================//

namespace pycp
{
//----------------------------------------------------------------------------//
// forward declarations
struct cpackDefinitions
{
    typedef std::map<std::string, std::string> MapType;
    MapType                                    Map;
    cmCPackLog*                                Log;
};

//----------------------------------------------------------------------------//
int
cpackUnknownArgument(const char* /*unused*/, void* /*unused*/);

//----------------------------------------------------------------------------//
int
cpackDefinitionArgument(const char* argument, const char* cValue,
                        void* call_data);

//----------------------------------------------------------------------------//
int
cpack_main_driver(int ac, char const* const* av);

//----------------------------------------------------------------------------//

class pycpack
{
};

//----------------------------------------------------------------------------//

class cpackWrapper
{
public:
    cpackWrapper(pycpack* obj)
    : m_cpack(obj)
    {
    }

    ~cpackWrapper() {}

             operator pycpack*() { return m_cpack; }
    pycpack* get() const { return m_cpack; }

protected:
    pycpack* m_cpack;
};

//----------------------------------------------------------------------------//

}  // namespace pycp

//----------------------------------------------------------------------------//

#endif
