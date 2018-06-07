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

#include "pyctest.hpp"

// define helper macros
#define pyobj_cast(_var, _type, _pyobject) _type * _var = _pyobject.cast< _type * >()

//============================================================================//

namespace pyct
{

//============================================================================//

namespace test
{

//============================================================================//

pycmTestGenerator::pycmTestGenerator(pycmTest* test)
: cmScriptGenerator("CTEST_CONFIGURATION_TYPE", std::vector<std::string>()),
  m_test(test),
  m_test_generated(false)
{ }

//============================================================================//

pycmTestGenerator::~pycmTestGenerator()
{ }

//============================================================================//

bool pycmTestGenerator::TestsForConfig(const std::string& config)
{
    return this->GeneratesForConfig(config);
}

//============================================================================//

pycmTest* pycmTestGenerator::GetTest() const
{
    return m_test;
}

//============================================================================//

void pycmTestGenerator::GenerateScriptConfigs(std::ostream& os, Indent indent)
{
    // Create the tests.
    this->cmScriptGenerator::GenerateScriptConfigs(os, indent);
}

//============================================================================//

void pycmTestGenerator::GenerateScriptActions(std::ostream& os, Indent indent)
{
    if (this->ActionsPerConfig)
    {
        // This is the per-config generation in a single-configuration
        // build generator case.  The superclass will call our per-config
        // method.
        this->cmScriptGenerator::GenerateScriptActions(os, indent);
    }
    else
    {
        // This is an old-style test, so there is only one config.
        // assert(m_test->GetOldStyle());
        this->GenerateOldStyle(os, indent);
    }
}

//============================================================================//

void pycmTestGenerator::GenerateScriptForConfig(std::ostream& os,
                                              const std::string&,
                                              Indent indent)
{
    GenerateOldStyle(os, indent);
}

//============================================================================//

void pycmTestGenerator::GenerateScriptNoConfig(std::ostream& os, Indent indent)
{
    os << indent << "add_test(" << m_test->GetName() << " NOT_AVAILABLE)\n";
}

//============================================================================//

bool pycmTestGenerator::NeedsScriptNoConfig() const
{
    return (m_test_generated &&    // test generated for at least one config
            this->ActionsPerConfig && // test is config-aware
            this->Configurations.empty() &&      // test runs in all configs
            !this->ConfigurationTypes->empty()); // config-dependent command
}

//============================================================================//

void pycmTestGenerator::GenerateOldStyle(std::ostream& fout, Indent indent)
{
    m_test_generated = true;

    // Get the test command line to be executed.
    std::vector<std::string> const& command = m_test->GetCommand();

    std::string exe = command[0];
    cmSystemTools::ConvertToUnixSlashes(exe);
    fout << indent;
    fout << "add_test(";
    fout << m_test->GetName() << " \"" << exe << "\"";

    for (std::vector<std::string>::const_iterator argit = command.begin() + 1;
         argit != command.end(); ++argit) {
        // Just double-quote all arguments so they are re-parsed
        // correctly by the test system.
        fout << " \"";
        for (char c : *argit) {
            // Escape quotes within arguments.  We should escape
            // backslashes too but we cannot because it makes the result
            // inconsistent with previous behavior of this command.
            if (c == '"') {
                fout << '\\';
            }
            fout << c;
        }
        fout << "\"";
    }
    fout << ")" << std::endl;

    // Output properties for the test.
    cmPropertyMap& pm = m_test->GetProperties();
    if (!pm.empty()) {
        fout << indent << "set_tests_properties(" << m_test->GetName()
             << " PROPERTIES ";
        for (auto const& i : pm) {
            fout << " " << i.first << " "
                 << cmOutputConverter::EscapeForCMake(i.second.GetValue());
        }
        fout << ")" << std::endl;
    }
}

//============================================================================//

} // namespace test

//============================================================================//

} // namespace pycm

//============================================================================//

PYBIND11_MODULE(pyctest, ct)
{
    py::add_ostream_redirect(ct, "ostream_redirect");

    //------------------------------------------------------------------------//
    // create a new test and add to test list
    auto test_init = [=] ()
    {
        auto obj = new pyct::test::pycmTest();
        pyct::test::get_test_list()->push_back(obj);
        return new pyct::test::pycmTestWrapper(obj);
    };
    //------------------------------------------------------------------------//
    auto test_add = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::test::pycmTestWrapper, obj);
        pyct::test::test_list_t* test_list = pyct::test::get_test_list();
        if(test_list)
        {
            auto itr = std::find(test_list->begin(), test_list->end(), _obj->get());
            if(itr == test_list->end())
                test_list->push_back(_obj->get());
        }
    };
    //------------------------------------------------------------------------//
    auto test_remove = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::test::pycmTestWrapper, obj);
        pyct::test::test_list_t* test_list = pyct::test::get_test_list();
        if(test_list)
        {
            auto itr = std::find(test_list->begin(), test_list->end(), _obj->get());
            if(itr != test_list->end())
                test_list->erase(itr);
        }
    };
    //------------------------------------------------------------------------//
    auto test_find = [=] (std::string test_name)
    {
        pyct::test::test_list_t* test_list = pyct::test::get_test_list();
        pyct::test::pycmTestWrapper* test = nullptr;
        if(test_list)
        {
            for(auto itr : *test_list)
                if(itr->GetName() == test_name)
                    return new pyct::test::pycmTestWrapper(itr);
        }
        return test;
    };
    //------------------------------------------------------------------------//

    py::class_<pyct::test::pycmTestWrapper> _test(ct, "test");

    ct.def("add_test", test_add, "Add a test");
    ct.def("remove_test", test_remove, "Remove a test");
    ct.def("find_test", test_find, "Find a test by name");
    ct.def("generate", &pyct::test::generate, "Generate a CTestTestfile.cmake",
           py::arg("output_dir") = "");

    _test.def(py::init(test_init), "Test for CTest");
    _test.def("SetName", &pyct::test::set_name, "Set test name");
    _test.def("GetName", &pyct::test::get_name, "Get test name");
    _test.def("SetCommand", &pyct::test::set_command, "Set the command for the test");
    _test.def("GetCommand", &pyct::test::get_command, "Get the command for the test");
    _test.def("SetProperty", &pyct::test::set_property, "Set a test property");
    _test.def("GetProperty", &pyct::test::get_property, "Get a test property");
    _test.def("GetPropertyAsBool", &pyct::test::get_property_as_bool, "Get property as boolean");

}

//============================================================================//


