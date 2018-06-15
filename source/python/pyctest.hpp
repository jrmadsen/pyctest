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
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
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

#include "pycmExecuteProcessCommand.hpp"

namespace pyct
{

//----------------------------------------------------------------------------//
// forward declarations
class pycmTest;
class pycmTestGenerator;
class pycmVariable;
int ctest_main_driver(int argc, char const* const* argv);
//----------------------------------------------------------------------------//
// typedefs
typedef std::string                     string_t;
typedef std::vector<string_t>           strvec_t;
typedef std::vector<pycmTest*>          test_list_t;
typedef std::vector<pycmVariable*>      test_variable_list_t;
typedef std::vector<pycmTestGenerator*> test_generator_list_t;
//----------------------------------------------------------------------------//
template <typename _Tp>
class pycmWrapper
{
public:
    pycmWrapper(_Tp* _type) : m_type(_type) {}
    ~pycmWrapper() { }
    operator _Tp*() { return m_type; }
    _Tp* get() const { return m_type; }

protected:
    _Tp*    m_type;
};
//----------------------------------------------------------------------------//
class pycmTest
{
public:
    pycmTest(string_t name = "", strvec_t cmd = strvec_t())
    : m_name(name), m_command(cmd)
    { }

    ~pycmTest()
    { }

    // name
    void SetName(const string_t& name) { m_name = name; }
    string_t GetName() const { return m_name; }

    // command
    void SetCommand(std::vector<string_t> const& command)
    {
        m_command = command;
    }
    std::vector<string_t> const& GetCommand() const { return m_command; }

    // properties
    void SetProperty(const string_t& prop, const char* value)
    {
        m_properties.SetProperty(prop, value);
    }
    void AppendProperty(const string_t& prop, const char* value,
                        bool asString = false)
    {
        m_properties.AppendProperty(prop, value, asString);
    }
    const char* GetProperty(const string_t& prop) const
    {
        return m_properties.GetPropertyValue(prop);
    }
    bool GetPropertyAsBool(const string_t& prop) const
    {
        return cmSystemTools::IsOn(this->GetProperty(prop));
    }
    cmPropertyMap& GetProperties() { return m_properties; }

private:
    cmPropertyMap m_properties;
    string_t m_name;
    std::vector<string_t> m_command;
};
//----------------------------------------------------------------------------//
class pycmTestWrapper : public pycmWrapper<pycmTest>
{
public:
    typedef pycmWrapper<pycmTest>   wrapper_t;

public:
    pycmTestWrapper(pycmTest* test)
    : wrapper_t(test)
    { }
};
//----------------------------------------------------------------------------//
class pycmTestGenerator : public cmScriptGenerator
{
public:
    pycmTestGenerator(pycmTest* test);
    ~pycmTestGenerator();

    /** Test if this generator installs the test for a given configuration.  */
    bool TestsForConfig(const string_t& config);

    pycmTest* GetTest() const;

protected:
    void GenerateScriptConfigs(std::ostream& os, Indent indent);
    void GenerateScriptActions(std::ostream& os, Indent indent);
    void GenerateScriptForConfig(std::ostream& os, const string_t& config,
                                 Indent indent);
    void GenerateScriptNoConfig(std::ostream& os, Indent indent);
    bool NeedsScriptNoConfig() const;
    void GenerateOldStyle(std::ostream& os, Indent indent);

    pycmTest* m_test;
    bool m_test_generated;
};
//----------------------------------------------------------------------------//
class pycmVariable
{
public:
    enum class cache_t
    {
        NONE,
        BOOL,
        FILEPATH,
        PATH,
        STRING,
        INTERNAL
    };

public:
    pycmVariable(const string_t& var, const string_t& val)
    : m_var(var), m_val(val), m_cache(cache_t::NONE), m_doc(""), m_force(false)
    { }

    pycmVariable(const string_t& var, const string_t& val, cache_t type,
                 const string_t& doc, bool force)
    : m_var(var), m_val(val), m_cache(type), m_doc(doc), m_force(force)
    { }

    const string_t& variable() const { return m_var; }
    const string_t& value() const { return m_val; }
    const cache_t& cache_type() const { return m_cache; }
    const string_t& doc_string() const { return m_doc; }
    const bool& force() const { return m_force; }

    friend std::ostream& operator<<(std::ostream& os, const pycmVariable& var)
    {
        os << var.as_string();
        return os;
    }

    string_t as_string() const
    {
        enum strbool
        {
            NON_BOOL,
            BOOL_TRUE,
            BOOL_FALSE
        };

        auto string_to_int = [] (string_t _str2)
        {
            std::stringstream ss;
            ss << _str2;
            int val;
            ss >> val;
            return val;
        };

        auto string_is_bool = [=] (string_t _str)
        {
            if(_str.length() == 0)
                return NON_BOOL;
            for(auto& itr : _str)
                itr = tolower(itr);

            if(_str.find("0123456789") != string_t::npos && _str.length() == 1)
            {
                if(_str.find_first_not_of("0123456789") == string_t::npos)
                    return (string_to_int(_str) == 0) ? BOOL_FALSE : BOOL_TRUE;
            }
            else if(_str == "on" || _str == "true")
                return BOOL_TRUE;
            else if(_str == "off" || _str == "false")
                return BOOL_FALSE;
            return NON_BOOL;
        };
        std::stringstream ss;
        // don't quote ON/OFF/TRUE/FALSE/{0..9}
        string_t quote = (string_is_bool(m_val) == NON_BOOL)
                         ? "\"" : "" ;

        ss << "set(" << m_var << " " << quote << m_val << quote;

        if(m_cache == cache_t::NONE)
            ss << ")";
        else
        {
            ss << " ";
            switch (m_cache)
            {
                case cache_t::BOOL:
                    ss << "BOOL";
                    break;
                case cache_t::FILEPATH:
                    ss << "FILEPATH";
                    break;
                case cache_t::PATH:
                    ss << "PATH";
                    break;
                case cache_t::STRING:
                    ss << "STRING";
                    break;
                case cache_t::INTERNAL:
                    ss << "INTERNAL";
                    break;
                case cache_t::NONE:
                default:
                    break;
            }
            ss << " " << quote << m_doc << quote;
            if(m_force)
                ss << " FORCE";
            ss << ")";
        }
        return ss.str();
    }

protected:
    string_t    m_var;
    string_t    m_val;
    cache_t     m_cache;
    string_t    m_doc;
    bool        m_force;
};
//----------------------------------------------------------------------------//
class pycmVariableWrapper: public pycmWrapper<pycmVariable>
{
public:
    typedef pycmWrapper<pycmVariable>   wrapper_t;

public:
    pycmVariableWrapper(pycmVariable* _variable)
    : wrapper_t(_variable)
    { }
};
//----------------------------------------------------------------------------//
test_list_t* get_test_list()
{
    typedef std::shared_ptr<test_list_t> ptr_t;
    static ptr_t _instance = ptr_t(new test_list_t());
    return _instance.get();
}
//----------------------------------------------------------------------------//
test_variable_list_t* get_test_variables()
{
    typedef std::shared_ptr<test_variable_list_t> ptr_t;
    static ptr_t _instance = ptr_t(new test_variable_list_t());
    return _instance.get();
}
//----------------------------------------------------------------------------//
strvec_t& get_config_attributes()
{
    static strvec_t _instance =
    {
        "PROJECT_NAME",
        "NIGHTLY_START_TIME",
        "DROP_METHOD",
        "DROP_SITE",
        "DROP_LOCATION",
        "CDASH_VERSION",
        "CDASH_QUERY_VERSION",
    };
    return _instance;
}
//----------------------------------------------------------------------------//
strvec_t& get_reqired_attributes()
{
    static strvec_t _instance =
    {
        "BUILD_NAME",
        "SOURCE_DIRECTORY",
        "BINARY_DIRECTORY",
        "MODEL",
        "SITE",
    };
    return _instance;
}
//----------------------------------------------------------------------------//
strvec_t& get_custom_attributes()
{
    static strvec_t _instance =
    {
        "CHECKOUT_COMMAND",
        "UPDATE_COMMAND",
        "CONFIGURE_COMMAND",
        "BUILD_COMMAND",
        "COVERAGE_COMMAND",
        "MEMORYCHECK_COMMAND",
        "GENERATOR",
        "GENERATOR_PLATFORM",
        "TIMEOUT",
        "TRIGGER",
        "CUSTOM_MAXIMUM_NUMBER_OF_ERRORS",
        "CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS",
        "CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE",
        "CUSTOM_COVERAGE_EXCLUDE"
    };
    return _instance;
}
//----------------------------------------------------------------------------//
void set_name(py::object self, string_t name)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetName(name);
}
//----------------------------------------------------------------------------//
string_t get_name(py::object self)
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
void set_property(py::object self, string_t prop, string_t value)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetProperty(prop, value.c_str());
}
//----------------------------------------------------------------------------//
string_t get_property(py::object self, string_t prop)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return string_t(_self->get()->GetProperty(prop));
}
//----------------------------------------------------------------------------//
bool get_property_as_bool(py::object self, string_t prop)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return _self->get()->GetPropertyAsBool(prop);
}
//----------------------------------------------------------------------------//
void configure_filepath(string_t& dir, string_t& fname)
{
    if(dir.length() > 0)
    {
        const string_t double_backslash = "\\\\";
        const string_t single_baskslash = "\\";
        auto replace_all = [=] (string_t& var, const string_t& val)
        {
            while(var.find(val) != string_t::npos)
                var.replace(var.find(val), val.length(), "/");
        };
        replace_all(dir, double_backslash);
        replace_all(dir, single_baskslash);

        if(dir.find_last_of("/") == dir.length()-1)
            dir = dir.substr(0, dir.length()-1);

        makedir(dir);

        fname = dir + "/" + fname;
    }
}
//----------------------------------------------------------------------------//
void generate_test_file(string_t dir = "")
{
    string_t fname = "CTestTestfile.cmake";
    configure_filepath(dir, fname);

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


//============================================================================//

// undefine helper macros
#undef pyobj_cast

//============================================================================//

#endif

