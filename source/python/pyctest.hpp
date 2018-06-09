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

#ifndef pyctest_hpp_
#define pyctest_hpp_

//============================================================================//
// base operating system

#if defined(_WIN32) || defined(_WIN64)
#   if !defined(_WINDOWS)
#       define _WINDOWS
#   endif
//----------------------------------------------------------------------------//

#elif defined(__APPLE__) || defined(__MACH__)
#   if !defined(_MACOS)
#       define _MACOS
#   endif
#   if !defined(_UNIX)
#       define _UNIX
#   endif
//----------------------------------------------------------------------------//

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#   if !defined(_LINUX)
#       define _LINUX
#   endif
#   if !defined(_UNIX)
#       define _UNIX
#   endif
//----------------------------------------------------------------------------//

#elif defined(__unix__) || defined(__unix) || defined(unix) || defined(_)
#   if !defined(_UNIX)
#       define _UNIX
#   endif
#endif

#if !defined(DEFAULT_UMASK)
#   define DEFAULT_UMASK 0777
#endif

//============================================================================//

// C library
#include <stdint.h>
#include <stdlib.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
// I/O
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iosfwd>
#include <fstream>
// general
#include <exception>
#include <stdexcept>
#include <functional>
#include <limits>
#include <memory>
#include <chrono>
// container
#include <deque>
#include <set>
#include <vector>
#include <map>
// threading
#include <future>
#include <atomic>
#include <mutex>
#include <thread>

#if defined(_UNIX)
#   include <stdio.h>
#   include <string.h>
#   include <errno.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#elif defined(_WINDOWS)
#   include <direct.h>
#endif

//============================================================================//

// use this function to get rid of "unused parameter" warnings
template <typename _Tp, typename... _Args>
void consume_parameters(_Tp, _Args...)
{ }

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

#include "cmake.h"
#include "cmTest.h"
#include "cmTestGenerator.h"
#include "cmCTest.h"
#include "cmPropertyMap.h"
#include "cmSystemTools.h"
#include "cmScriptGenerator.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmLocalGenerator.h"
#include "cmOutputConverter.h"
#include "cmProperty.h"
#include "cmPropertyMap.h"
#include "cmStateTypes.h"
#include "cmSystemTools.h"
#include "cmTest.h"
#include "CTest/cmCTestLaunch.h"
#include "CTest/cmCTestScriptHandler.h"
#include "cmCTest.h"
#include "cmDocumentation.h"
#include "cmSystemTools.h"
#include "cmsys/Encoding.hxx"
#if defined(_WIN32)
#  include "cmsys/ConsoleBuf.hxx"
#endif

//============================================================================//

namespace py = pybind11;
using namespace py::literals;
using namespace std::placeholders;  // for _1, _2, _3...

//============================================================================//

// define helper macros
#define pyobj_cast(_var, _type, _pyobject) _type * _var = _pyobject.cast< _type * >()

//============================================================================//

inline int makedir(std::string _dir, int umask = DEFAULT_UMASK)
{
#if defined(_UNIX)
    while(_dir.find("\\\\") != std::string::npos)
        _dir.replace(_dir.find("\\\\"), 2, "/");
    while(_dir.find("\\") != std::string::npos)
        _dir.replace(_dir.find("\\"), 1, "/");

    if(mkdir(_dir.c_str(), umask) != 0)
    {
        std::stringstream _sdir;
        _sdir << "mkdir -p " << _dir;
        return system(_sdir.str().c_str());
    }
#elif defined(_WINDOWS)
    consume_parameters<int>(umask);
    while(_dir.find("/") != std::string::npos)
        _dir.replace(_dir.find("/"), 1, "\\\\");

    if(_mkdir(_dir.c_str()) != 0)
    {
        std::stringstream _sdir;
        _sdir << "dir " << _dir;
        return system(_sdir.str().c_str());
    }
#endif
    return 0;
}

//============================================================================//
//
//  pyct namespace -- [pyct]est
//
//============================================================================//

namespace pyct
{

//----------------------------------------------------------------------------//
// forward declarations
class pycmTest;
class pycmTestGenerator;
int ctest_main_driver(int argc, char const* const* argv);
//----------------------------------------------------------------------------//
// typedefs
typedef std::vector<std::string> strvec_t;
typedef std::vector<pycmTest*> test_list_t;
typedef std::vector<pycmTestGenerator*> test_generator_list_t;
//----------------------------------------------------------------------------//
class pycmTest
{
public:
    pycmTest(std::string name = "", strvec_t cmd = strvec_t())
    : m_name(name), m_command(cmd)
    {
        if(m_name.length() > 0)
            std::cout << "Created test \"" << m_name << "\"..." << std::endl;
        else
            std::cout << "Created test \"" << this << "\"..." << std::endl;
    }

    ~pycmTest()
    {
        if(m_name.length() > 0)
            std::cout << "Deleted test \"" << m_name << "\"..." << std::endl;
        else
            std::cout << "Deleted test \"" << this << "\"..." << std::endl;
    }

    // name
    void SetName(const std::string& name) { m_name = name; }
    std::string GetName() const { return m_name; }

    // command
    void SetCommand(std::vector<std::string> const& command)
    {
        m_command = command;
    }
    std::vector<std::string> const& GetCommand() const { return m_command; }

    // properties
    void SetProperty(const std::string& prop, const char* value)
    {
        m_properties.SetProperty(prop, value);
    }
    void AppendProperty(const std::string& prop, const char* value,
                        bool asString = false)
    {
        m_properties.AppendProperty(prop, value, asString);
    }
    const char* GetProperty(const std::string& prop) const
    {
        return m_properties.GetPropertyValue(prop);
    }
    bool GetPropertyAsBool(const std::string& prop) const
    {
        return cmSystemTools::IsOn(this->GetProperty(prop));
    }
    cmPropertyMap& GetProperties() { return m_properties; }

private:
    cmPropertyMap m_properties;
    std::string m_name;
    std::vector<std::string> m_command;
};
//----------------------------------------------------------------------------//
class pycmTestWrapper
{
public:
    pycmTestWrapper(std::string name = "", strvec_t cmd = strvec_t())
    : m_test(new pycmTest(name, cmd))
    { }

    pycmTestWrapper(pycmTest* test)
    : m_test(test)
    { }

    ~pycmTestWrapper()
    { }

    operator pycmTest*() { return m_test; }

    pycmTest* get() const { return m_test; }

protected:
    pycmTest* m_test;
};
//----------------------------------------------------------------------------//
class pycmTestGenerator : public cmScriptGenerator
{
public:
    pycmTestGenerator(pycmTest* test);
    ~pycmTestGenerator();

    /** Test if this generator installs the test for a given configuration.  */
    bool TestsForConfig(const std::string& config);

    pycmTest* GetTest() const;

protected:
    void GenerateScriptConfigs(std::ostream& os, Indent indent);
    void GenerateScriptActions(std::ostream& os, Indent indent);
    void GenerateScriptForConfig(std::ostream& os, const std::string& config,
                                 Indent indent);
    void GenerateScriptNoConfig(std::ostream& os, Indent indent);
    bool NeedsScriptNoConfig() const;
    void GenerateOldStyle(std::ostream& os, Indent indent);

    pycmTest* m_test;
    bool m_test_generated;
};
//----------------------------------------------------------------------------//
test_list_t* get_test_list()
{
    typedef std::shared_ptr<test_list_t> ptr_t;
    static ptr_t _instance = ptr_t(new test_list_t());
    return _instance.get();
}
//----------------------------------------------------------------------------//
void set_name(py::object self, std::string name)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetName(name);
}
//----------------------------------------------------------------------------//
std::string get_name(py::object self)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return _self->get()->GetName();
}
//----------------------------------------------------------------------------//
void set_command(py::object self, py::list cmd)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetCommand(cmd.cast<strvec_t>());
}
//----------------------------------------------------------------------------//
py::list get_command(py::object self)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    strvec_t cxxcmd = _self->get()->GetCommand();
    py::list pycmd;
    for(const auto& itr : cxxcmd)
        pycmd.append(itr);
    return pycmd;
}
//----------------------------------------------------------------------------//
void set_property(py::object self, std::string prop, std::string value)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetProperty(prop, value.c_str());
}
//----------------------------------------------------------------------------//
std::string get_property(py::object self, std::string prop)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return std::string(_self->get()->GetProperty(prop));
}
//----------------------------------------------------------------------------//
bool get_property_as_bool(py::object self, std::string prop)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return _self->get()->GetPropertyAsBool(prop);
}
//----------------------------------------------------------------------------//
void generate(std::string dir = "")
{
    std::string fname = "CTestTestfile.cmake";

    if(dir.length() > 0)
    {
        const std::string double_backslash = "\\\\";
        const std::string single_baskslash = "\\";
        auto replace_all = [=] (std::string& var, const std::string& val)
        {
            while(var.find(val) != std::string::npos)
                var.replace(var.find(val), val.length(), "/");
        };
        replace_all(dir, double_backslash);
        replace_all(dir, single_baskslash);

        if(dir.find_last_of("/") == dir.length()-1)
            dir = dir.substr(0, dir.length()-1);

        makedir(dir);

        fname = dir + "/" + fname;
    }

    auto test_list = get_test_list();
    if(!test_list || (test_list && test_list->size() == 0))
    {
        std::cerr << __FUNCTION__ << ":: Warning! No tests to generate!!!"
                  << std::endl;
    }
    else
    {
        std::ofstream ofs(fname.c_str());
        test_generator_list_t generator_list;
        if(ofs)
        {
            std::cout << "Writing CTest test file: \"" << fname << "\"..."
                      << std::endl;
            strvec_t configs(0, "");
            for(auto itr : *test_list)
                generator_list.push_back(new pycmTestGenerator(itr));

            for(auto& itr : generator_list)
            {
                std::cout << "Generating test \"" << itr->GetTest()->GetName()
                          << "\"..." << std::endl;
                itr->Generate(ofs, "", configs);
            }
        }
        else
            std::cerr << __FUNCTION__ << ":: Error opening " << fname
                      << "!!!" << std::endl;
    }

}
//----------------------------------------------------------------------------//

} // namespace pycm

//============================================================================//

// undefine helper macros
#undef pyobj_cast

//============================================================================//

#endif

