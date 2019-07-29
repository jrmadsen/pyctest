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

#ifndef pyctest_hpp_
#define pyctest_hpp_

//============================================================================//
// base operating system

#if defined(_WIN32) || defined(_WIN64)
#    if !defined(_WINDOWS)
#        define _WINDOWS
#    endif
//----------------------------------------------------------------------------//

#elif defined(__APPLE__) || defined(__MACH__)
#    if !defined(_MACOS)
#        define _MACOS
#    endif
#    if !defined(_UNIX)
#        define _UNIX
#    endif
//----------------------------------------------------------------------------//

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#    if !defined(_LINUX)
#        define _LINUX
#    endif
#    if !defined(_UNIX)
#        define _UNIX
#    endif
//----------------------------------------------------------------------------//

#elif defined(__unix__) || defined(__unix) || defined(unix) || defined(_)
#    if !defined(_UNIX)
#        define _UNIX
#    endif
#endif

#if !defined(DEFAULT_UMASK)
#    define DEFAULT_UMASK 0777
#endif

//============================================================================//

// C library
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
// I/O
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <sstream>
#include <string>
// general
#include <chrono>
#include <exception>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
// container
#include <deque>
#include <map>
#include <set>
#include <vector>
// threading
#include <atomic>
#include <future>
#include <mutex>
#include <thread>

#if defined(_UNIX)
#    include <errno.h>
#    include <stdio.h>
#    include <string.h>
#    include <sys/stat.h>
#    include <sys/types.h>
#elif defined(_WINDOWS)
#    include <direct.h>
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#endif

//============================================================================//

// use this function to get rid of "unused parameter" warnings
template <typename _Tp, typename... _Args>
void
consume_parameters(_Tp, _Args...)
{
}

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

#include "CTest/cmCTestBuildCommand.h"
#include "CTest/cmCTestConfigureCommand.h"
#include "CTest/cmCTestCoverageCommand.h"
#include "CTest/cmCTestLaunch.h"
#include "CTest/cmCTestMemCheckCommand.h"
#include "CTest/cmCTestScriptHandler.h"
#include "CTest/cmCTestStartCommand.h"
#include "CTest/cmCTestSubmitCommand.h"
#include "CTest/cmCTestUpdateCommand.h"
#include "cmCTest.h"
#include "cmDocumentation.h"
#include "cmGeneratorExpression.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmOutputConverter.h"
#include "cmProperty.h"
#include "cmPropertyMap.h"
#include "cmScriptGenerator.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmStateTypes.h"
#include "cmSystemTools.h"
#include "cmTest.h"
#include "cmTestGenerator.h"
#include "cmake.h"
#include "cmsys/Encoding.hxx"

#if defined(_WIN32)
#    include "cmsys/ConsoleBuf.hxx"
#endif

//============================================================================//

namespace py = pybind11;
using namespace py::literals;
using namespace std::placeholders;  // for _1, _2, _3...

//============================================================================//

// define helper macros
#define pyobj_cast(_var, _type, _pyobject) _type* _var = _pyobject.cast<_type*>()
#define FORWARD_GET(ret, func) \
    ret func() const { return m_obj->func(); }
#define FORWARD_SET(func, param) \
    void func(param val) { m_obj->func(val); }
#define FORWARD_FUN_0(ret, func) \
    ret func() { return m_obj->func(); }
#define FORWARD_FUN_1(ret, func, param) \
    ret func(param val) { return m_obj->func(val); }

//============================================================================//

inline int
makedir(std::string _dir, int umask = DEFAULT_UMASK)
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
class pycmCTest;
class pycmTest;
class pycmTestGenerator;
class pycmVariable;
class pycmCTestStartCommand;
class pycmMakefile;
class pycmGlobalGenerator;
class pycmStateSnapshot;
//----------------------------------------------------------------------------//
int
ctest_main_driver(int argc, char const* const* argv);
//----------------------------------------------------------------------------//
// typedefs
typedef std::string                     string_t;
typedef std::vector<string_t>           strvec_t;
typedef std::vector<pycmTest*>          test_list_t;
typedef std::vector<pycmVariable*>      test_variable_list_t;
typedef std::vector<pycmTestGenerator*> test_generator_list_t;
//----------------------------------------------------------------------------//

//============================================================================//
//
//  Base wrapper to avoid Python GC
//
//============================================================================//
template <typename _Tp> class pycmWrapper
{
public:
    pycmWrapper(_Tp* _type)
    : m_obj(_type)
    {
    }
    ~pycmWrapper() {}

    operator _Tp*() { return m_obj; }
    operator const _Tp*() const { return m_obj; }

    _Tp*&      get() { return m_obj; }
    const _Tp* get() const { return m_obj; }

protected:
    _Tp* m_obj;
};
//----------------------------------------------------------------------------//
template <typename _Cp, typename _Tp> class pycmSingletonWrapper : public pycmWrapper<_Cp>
{
public:
    pycmSingletonWrapper(_Cp* _type)
    : pycmWrapper<_Cp>(_type)
    {
    }
    ~pycmSingletonWrapper() {}

    static _Tp* GetInstance()
    {
        static _Tp* _instance = new _Tp();
        return _instance;
    }
};
//============================================================================//

//============================================================================//
//
//  Python versions of CMake/CTest classes
//
//============================================================================//
class pycmCTest : public cmCTest
{
public:
    pycmCTest()
    : cmCTest()
    {
    }

    static pycmCTest* GetInstance()
    {
        static pycmCTest* _instance = new pycmCTest();
        return _instance;
    }
};
//----------------------------------------------------------------------------//
class pycmTest
{
public:
    pycmTest(string_t name = "", strvec_t cmd = strvec_t())
    : m_name(name)
    , m_command(cmd)
    {
    }

    ~pycmTest() {}

    // name
    void     SetName(const string_t& name) { m_name = name; }
    string_t GetName() const { return m_name; }

    // command
    void SetCommand(std::vector<string_t> const& command) { m_command = command; }
    std::vector<string_t> const& GetCommand() const { return m_command; }

    // properties
    void SetProperty(const string_t& prop, const char* value)
    {
        m_properties.SetProperty(prop, value);
    }
    void AppendProperty(const string_t& prop, const char* value, bool asString = false)
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
    cmPropertyMap         m_properties;
    string_t              m_name;
    std::vector<string_t> m_command;
};
//----------------------------------------------------------------------------//
class pycmCTestStartCommand : public pycmWrapper<cmCTestStartCommand>
{
public:
    typedef pycmWrapper<cmCTestStartCommand> wrap_type;

public:
    pycmCTestStartCommand(cmCTestStartCommand* obj)
    : wrap_type(obj)
    {
    }
};
//----------------------------------------------------------------------------//
class pycmCTestWrapper : public pycmWrapper<pycmCTest>
{
public:
    typedef pycmWrapper<pycmCTest>             wrap_type;
    typedef std::map<std::string, std::string> string_map;

public:
    pycmCTestWrapper(pycmCTest* test)
    : wrap_type(test)
    {
    }

    FORWARD_FUN_0(void, EmptyCTestConfiguration)
    FORWARD_FUN_0(int, ProcessSteps)

    FORWARD_FUN_1(int, TestDirectory, bool)
    FORWARD_FUN_1(bool, CTestFileExists, const string_t&)

    void SetNotesFiles(const string_t& notes) { m_obj->SetNotesFiles(notes.c_str()); }
    void SetSpecificTrack(const string_t& track)
    {
        m_obj->SetSpecificTrack(track.c_str());
    }
    string_t GetSpecificTrack() { return m_obj->GetSpecificTrack(); }
    void     Initialize(pycmCTestStartCommand* start)
    {
        m_obj->InitializeFromCommand(start->get());
    }

    FORWARD_GET(bool, GetTomorrowTag)
    FORWARD_GET(const string_t&, GetConfigType)
    FORWARD_GET(cmDuration, GetTimeOut)
    FORWARD_GET(int, GetParallelLevel)
    FORWARD_GET(int, GetTestModel)
    FORWARD_GET(string_t, GetCurrentTag)
    FORWARD_GET(string_t, GetBinaryDir)
    FORWARD_GET(cmDuration, GetGlobalTimeout)
    FORWARD_GET(unsigned long, GetTestLoad)
    FORWARD_GET(string_t, CurrentTime)
    FORWARD_GET(cmDuration, GetRemainingTimeAllowed)
    FORWARD_GET(std::string, GetScheduleType)
    FORWARD_GET(int, GetMaxTestNameWidth)
    FORWARD_GET(int, GetDartVersion)
    FORWARD_GET(int, GetDropSiteCDash)
    FORWARD_GET(const string_map&, GetDefinitions)
    FORWARD_GET(string_t, GetTestModelString)

    FORWARD_SET(SetTimeOut, cmDuration)
    FORWARD_SET(SetParallelLevel, int)
    FORWARD_SET(SetTestModel, int)
    FORWARD_SET(SetTestLoad, unsigned long)
    FORWARD_SET(SetStopTime, string_t const&)
    FORWARD_SET(SetScheduleType, string_t const&)
    FORWARD_SET(SetMaxTestNameWidth, int)
    FORWARD_SET(SetSuppressUpdatingCTestConfiguration, bool)
    FORWARD_SET(AddCTestConfigurationOverwrite, const string_t&)
    FORWARD_SET(SetRunCurrentScript, bool)
};
//----------------------------------------------------------------------------//
class pycmTestWrapper : public pycmWrapper<pycmTest>
{
public:
    typedef pycmWrapper<pycmTest> wrap_type;

public:
    pycmTestWrapper(pycmTest* test)
    : wrap_type(test)
    {
    }
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
    void GenerateScriptForConfig(std::ostream& os, const string_t& config, Indent indent);
    void GenerateScriptNoConfig(std::ostream& os, Indent indent);
    bool NeedsScriptNoConfig() const;
    void GenerateOldStyle(std::ostream& os, Indent indent);

    bool      m_test_generated;
    pycmTest* m_test;
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
    : m_var(var)
    , m_val(val)
    , m_cache(cache_t::NONE)
    , m_doc("")
    , m_force(false)
    {
    }

    pycmVariable(const string_t& var,
                 const string_t& val,
                 cache_t         type,
                 const string_t& doc,
                 bool            force)
    : m_var(var)
    , m_val(val)
    , m_cache(type)
    , m_doc(doc)
    , m_force(force)
    {
    }

    const string_t& variable() const { return m_var; }
    const string_t& value() const { return m_val; }
    const cache_t&  cache_type() const { return m_cache; }
    const string_t& doc_string() const { return m_doc; }
    const bool&     force() const { return m_force; }

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

        auto string_to_int = [](string_t _str2) {
            std::stringstream ss;
            ss << _str2;
            int val;
            ss >> val;
            return val;
        };

        auto string_is_bool = [=](string_t _str) {
            if(_str.length() == 0)
                return NON_BOOL;
            for(auto& itr : _str)
                itr = static_cast<char>(tolower(itr));

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
        string_t quote = (string_is_bool(m_val) == NON_BOOL) ? "\"" : "";

        ss << "set(" << m_var << " " << quote << m_val << quote;

        if(m_cache == cache_t::NONE)
            ss << ")";
        else
        {
            ss << " ";
            switch(m_cache)
            {
                case cache_t::BOOL: ss << "BOOL"; break;
                case cache_t::FILEPATH: ss << "FILEPATH"; break;
                case cache_t::PATH: ss << "PATH"; break;
                case cache_t::STRING: ss << "STRING"; break;
                case cache_t::INTERNAL: ss << "INTERNAL"; break;
                case cache_t::NONE: break;
            }
            ss << " " << quote << m_doc << quote;
            if(m_force)
                ss << " FORCE";
            ss << ")";
        }
        return ss.str();
    }

protected:
    string_t m_var;
    string_t m_val;
    cache_t  m_cache;
    string_t m_doc;
    bool     m_force;
};
//----------------------------------------------------------------------------//
class pycmVariableWrapper : public pycmWrapper<pycmVariable>
{
public:
    typedef pycmWrapper<pycmVariable> wrap_type;

public:
    pycmVariableWrapper(pycmVariable* _variable)
    : wrap_type(_variable)
    {
    }
};
//----------------------------------------------------------------------------//
class pycmStateSnapshot : public pycmWrapper<cmStateSnapshot>
{
public:
    typedef cmStateSnapshot        base_type;
    typedef pycmStateSnapshot      this_type;
    typedef pycmWrapper<base_type> wrap_type;

public:
    pycmStateSnapshot(base_type* _obj)
    : wrap_type(_obj)
    {
    }

    string_t GetDefinition(const string_t& name) const
    {
        return *m_obj->GetDefinition(name);
    }
    bool IsInitialized(const string_t& name) const { return m_obj->IsInitialized(name); }
    void SetDefinition(const string_t& name, const string_t& value)
    {
        m_obj->SetDefinition(name, value);
    }
    void RemoveDefinition(const string_t& name) { m_obj->RemoveDefinition(name); }
    std::vector<string_t> UnusedKeys() const { return m_obj->UnusedKeys(); }
    std::vector<string_t> ClosureKeys() const { return m_obj->ClosureKeys(); }
    bool                  RaiseScope(const string_t& var, const string_t& varDef)
    {
        return m_obj->RaiseScope(var, varDef.c_str());
    }
    void     SetListFile(const string_t& listfile) { m_obj->SetListFile(listfile); }
    string_t GetExecutionListFile() const { return m_obj->GetExecutionListFile(); }
    bool     IsValid() const { return m_obj->IsValid(); }
    void     SetProjectName(const string_t& name) { m_obj->SetProjectName(name); }
    string_t GetProjectName() const { return m_obj->GetProjectName(); }
    void     SetDirectoryDefinitions() { m_obj->SetDirectoryDefinitions(); }
    void     SetDefaultDefinitions() { m_obj->SetDefaultDefinitions(); }
    cmState* GetState() const { return m_obj->GetState(); }

    // std::vector<cmStateSnapshot> GetChildren();
    // cmStateSnapshot GetBuildsystemDirectoryParent() const;
    // cmStateSnapshot GetCallStackParent() const;
    // cmStateSnapshot GetCallStackBottom() const;
    // cmStateEnums::SnapshotType GetType() const;
    // void SetPolicy(cmPolicies::PolicyID id, cmPolicies::PolicyStatus status);
    // cmPolicies::PolicyStatus GetPolicy(cmPolicies::PolicyID id,
    //                                   bool parent_scope = false) const;
    // bool HasDefinedPolicyCMP0011();
    // void PushPolicy(cmPolicies::PolicyMap const& entry, bool weak);
    // bool PopPolicy();
    // bool CanPopPolicyScope();

    // cmStateDirectory GetDirectory() const;
    // void InitializeFromParent_ForSubdirsCommand();
    // struct StrictWeakOrder
    //{
    //  bool operator()(const cmStateSnapshot& lhs,
    //                  const cmStateSnapshot& rhs) const;
    //};
};
//----------------------------------------------------------------------------//
class pycmState : public pycmWrapper<cmState>
{
public:
    typedef cmState                base_type;
    typedef pycmState              this_type;
    typedef pycmWrapper<base_type> wrap_type;

public:
    pycmState(base_type* _obj)
    : wrap_type(_obj)
    {
    }

    bool LoadCache(const std::string&     path,
                   bool                   internal,
                   std::set<std::string>& excludes,
                   std::set<std::string>& includes)
    {
        return m_obj->LoadCache(path, internal, excludes, includes);
    }

    bool DeleteCache(const std::string& path) { return m_obj->DeleteCache(path); }

    std::vector<std::string> GetCacheEntryKeys() const
    {
        return m_obj->GetCacheEntryKeys();
    }
    string_t GetCacheEntryValue(const string_t& key) const
    {
        return m_obj->GetCacheEntryValue(key);
    }
    string_t GetInitializedCacheValue(const string_t& key) const
    {
        return *m_obj->GetInitializedCacheValue(key);
    }
    void SetCacheEntryValue(const string_t& key, const string_t& value)
    {
        m_obj->SetCacheEntryValue(key, value);
    }
    void SetCacheValue(const string_t& key, const string_t& value)
    {
        m_obj->SetCacheEntryValue(key, value);
    }

    void RemoveCacheEntry(const string_t& key) { return m_obj->RemoveCacheEntry(key); }

    void SetCacheEntryProperty(const string_t& key,
                               const string_t& propertyName,
                               const string_t& value)
    {
        m_obj->SetCacheEntryBoolProperty(key, propertyName, value.c_str());
    }

    void SetCacheEntryBoolProperty(const string_t& key,
                                   const string_t& propertyName,
                                   bool            value)
    {
        m_obj->SetCacheEntryBoolProperty(key, propertyName, value);
    }

    std::vector<std::string> GetCacheEntryPropertyList(const string_t& key)
    {
        return m_obj->GetCacheEntryPropertyList(key);
    }

    string_t GetCacheEntryProperty(const string_t& key, const string_t& propertyName)
    {
        return m_obj->GetCacheEntryProperty(key, propertyName);
    }

    bool GetCacheEntryPropertyAsBool(const string_t& key, const string_t& propertyName)
    {
        return m_obj->GetCacheEntryPropertyAsBool(key, propertyName);
    }

    void AppendCacheEntryProperty(const string_t&    key,
                                  const std::string& property,
                                  const std::string& value,
                                  bool               asString = false)
    {
        m_obj->AppendCacheEntryProperty(key, property, value, asString);
    }

    void RemoveCacheEntryProperty(const string_t& key, const string_t& propertyName)
    {
        m_obj->RemoveCacheEntryProperty(key, propertyName);
    }

    void SetLanguageEnabled(const string_t& l) { m_obj->SetLanguageEnabled(l); }
    bool GetLanguageEnabled(const string_t& l) const
    {
        return m_obj->GetLanguageEnabled(l);
    }
    std::vector<std::string> GetEnabledLanguages() const
    {
        return m_obj->GetEnabledLanguages();
    }
    void SetEnabledLanguages(std::vector<std::string> const& langs)
    {
        m_obj->SetEnabledLanguages(langs);
    }
    void ClearEnabledLanguages() { m_obj->ClearEnabledLanguages(); }
    void RemoveUserDefinedCommands() { m_obj->RemoveUserDefinedCommands(); }
    std::vector<std::string> GetCommandNames() const { return m_obj->GetCommandNames(); }

    void SetGlobalProperty(const std::string& prop, const string_t& value)
    {
        m_obj->SetGlobalProperty(prop, value.c_str());
    }
    void AppendGlobalProperty(const std::string& prop,
                              const string_t&    value,
                              bool               asString = false)
    {
        m_obj->AppendGlobalProperty(prop, value.c_str(), asString);
    }
    string_t GetGlobalProperty(const std::string& prop)
    {
        return m_obj->GetGlobalProperty(prop);
    }
    bool GetGlobalPropertyAsBool(const std::string& prop)
    {
        return m_obj->GetGlobalPropertyAsBool(prop);
    }

    const string_t& GetSourceDirectory() const { return m_obj->GetSourceDirectory(); }
    void            SetSourceDirectory(const string_t& sourceDirectory)
    {
        m_obj->SetSourceDirectory(sourceDirectory);
    }
    const string_t& GetBinaryDirectory() const { return m_obj->GetBinaryDirectory(); }
    void            SetBinaryDirectory(const string_t& binaryDirectory)
    {
        m_obj->SetBinaryDirectory(binaryDirectory);
    }

    unsigned int GetCacheMajorVersion() const { return m_obj->GetCacheMajorVersion(); }
    unsigned int GetCacheMinorVersion() const { return m_obj->GetCacheMinorVersion(); }

    // bool SaveCache(const std::string& path, cmMessenger* messenger);
    // cmStateEnums::CacheEntryType GetCacheEntryType(const string_t& key)
    // const; cmStateSnapshot Reset();
    // Define a property
    // void DefineProperty(const std::string& name, cmProperty::ScopeType scope,
    //                    const string_t& ShortDescription,
    //                    const string_t& FullDescription, bool chain = false);
    // get property definition
    // cmPropertyDefinition const* GetPropertyDefinition(
    //  const std::string& name, cmProperty::ScopeType scope) const;
    // Is a property defined?
    // bool IsPropertyDefined(const std::string& name,
    //                       cmProperty::ScopeType scope) const;
    // bool IsPropertyChained(const std::string& name,
    //                       cmProperty::ScopeType scope) const;
    // bool GetIsInTryCompile() const;
    // void SetIsInTryCompile(bool b);
    // bool GetIsGeneratorMultiConfig() const;
    // void SetIsGeneratorMultiConfig(bool b);
    // Returns a command from its name, case insensitive, or nullptr
    // cmCommand* GetCommand(const string_t& name) const;
    // Returns a command from its name, or nullptr
    // cmCommand* GetCommandByExactName(const string_t& name) const;
    // void AddBuiltinCommand(const string_t& name, cmCommand* command);
    // void AddDisallowedCommand(const string_t& name, cmCommand* command,
    //                          cmPolicies::PolicyID policy, const string_t&
    //                          message);
    // void AddUnexpectedCommand(const string_t& name, const string_t& error);
    // void AddScriptedCommand(const string_t& name, cmCommand* command);
    // void SetWindowsShell(bool windowsShell);
    // bool UseWindowsShell() const;
    // void SetWindowsVSIDE(bool windowsVSIDE);
    // bool UseWindowsVSIDE() const;
    // void SetWatcomWMake(bool watcomWMake);
    // bool UseWatcomWMake() const;
    // void SetMinGWMake(bool minGWMake);
    // bool UseMinGWMake() const;
    // void SetNMake(bool nMake);
    // bool UseNMake() const;
    // void SetMSYSShell(bool mSYSShell);
    // bool UseMSYSShell() const;
};
//----------------------------------------------------------------------------//

//============================================================================//
//
//  Generic functions
//
//============================================================================//

test_list_t*
get_test_list()
{
    typedef std::shared_ptr<test_list_t> ptr_t;
    static ptr_t                         _instance = ptr_t(new test_list_t());
    return _instance.get();
}
//----------------------------------------------------------------------------//
test_variable_list_t*
get_test_variables()
{
    typedef std::shared_ptr<test_variable_list_t> ptr_t;
    static ptr_t _instance = ptr_t(new test_variable_list_t());
    return _instance.get();
}
//----------------------------------------------------------------------------//
strvec_t&
get_config_attributes()
{
    static strvec_t _instance = {
        "PROJECT_NAME",  "NIGHTLY_START_TIME", "DROP_METHOD",         "DROP_SITE",
        "DROP_LOCATION", "CDASH_VERSION",      "CDASH_QUERY_VERSION",
    };
    return _instance;
}
//----------------------------------------------------------------------------//
strvec_t&
get_required_attributes()
{
    static strvec_t _instance = {
        "BUILD_NAME", "SOURCE_DIRECTORY", "BINARY_DIRECTORY", "MODEL", "SITE",
    };
    return _instance;
}
//----------------------------------------------------------------------------//
strvec_t&
get_custom_attributes()
{
    static strvec_t _instance = { "CHECKOUT_COMMAND",
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
                                  "CUSTOM_COVERAGE_EXCLUDE" };
    return _instance;
}
//----------------------------------------------------------------------------//
void
set_name(py::object self, string_t name)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetName(name);
}
//----------------------------------------------------------------------------//
string_t
get_name(py::object self)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return _self->get()->GetName();
}
//----------------------------------------------------------------------------//
void
set_command(py::object self, py::list cmd)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetCommand(cmd.cast<strvec_t>());
}
//----------------------------------------------------------------------------//
py::list
get_command(py::object self)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    strvec_t cxxcmd = _self->get()->GetCommand();
    py::list pycmd;
    for(const auto& itr : cxxcmd)
        pycmd.append(itr);
    return pycmd;
}
//----------------------------------------------------------------------------//
void
set_property(py::object self, string_t prop, string_t value)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    _self->get()->SetProperty(prop, value.c_str());
}
//----------------------------------------------------------------------------//
string_t
get_property(py::object self, string_t prop)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return string_t(_self->get()->GetProperty(prop));
}
//----------------------------------------------------------------------------//
bool
get_property_as_bool(py::object self, string_t prop)
{
    pyobj_cast(_self, pycmTestWrapper, self);
    return _self->get()->GetPropertyAsBool(prop);
}
//----------------------------------------------------------------------------//
void
configure_filepath(string_t& dir, string_t& fname)
{
    if(dir.length() > 0)
    {
        const string_t double_backslash = "\\\\";
        const string_t single_baskslash = "\\";
        auto           replace_all      = [=](string_t& var, const string_t& val) {
            while(var.find(val) != string_t::npos)
                var.replace(var.find(val), val.length(), "/");
        };
        replace_all(dir, double_backslash);
        replace_all(dir, single_baskslash);

        if(dir.find_last_of("/") == dir.length() - 1)
            dir = dir.substr(0, dir.length() - 1);

        makedir(dir);

        fname = dir + "/" + fname;
    }
}
//----------------------------------------------------------------------------//
void
generate_test_file(string_t dir = "")
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
        std::ofstream         ofs(fname.c_str());
        test_generator_list_t generator_list;
        if(ofs)
        {
            std::cout << "Writing CTest test file: \"" << fname << "\"..." << std::endl;
            strvec_t configs(0, "");
            for(auto itr : *test_list)
                generator_list.push_back(new pycmTestGenerator(itr));

            for(auto& itr : generator_list)
            {
                std::cout << "Generating test \"" << itr->GetTest()->GetName() << "\"..."
                          << std::endl;
                itr->Generate(ofs, "", configs);
            }
        }
        else
            std::cerr << __FUNCTION__ << ":: Error opening " << fname << "!!!"
                      << std::endl;
    }
}
//----------------------------------------------------------------------------//

}  // namespace pycm

//============================================================================//

//============================================================================//

// undefine helper macros
#undef pyobj_cast
#undef FORWARD_GET
#undef FORWARD_SET
#undef FORWARD_FUN_0
#undef FORWARD_FUN_1

//============================================================================//

#endif
