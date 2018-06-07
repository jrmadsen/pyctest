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

static const char* cmDocumentationName[][2] =
{
    { nullptr, "  ctest - Testing driver provided by CMake." },
    { nullptr, nullptr }
};

static const char* cmDocumentationUsage[][2] =
{
    { nullptr, "  ctest [options]" },
    { nullptr, nullptr }
};

static const char* cmDocumentationOptions[][2] =
{
    { "-C <cfg>, --build-config <cfg>", "Choose configuration to test." },
    { "-V,--verbose", "Enable verbose output from tests." },
    { "-VV,--extra-verbose", "Enable more verbose output from tests." },
    { "--debug", "Displaying more verbose internals of CTest." },
    { "--output-on-failure",
      "Output anything outputted by the test program "
      "if the test should fail." },
    { "--test-output-size-passed <size>",
      "Limit the output for passed tests "
      "to <size> bytes" },
    { "--test-output-size-failed <size>",
      "Limit the output for failed tests "
      "to <size> bytes" },
    { "-F", "Enable failover." },
    { "-j <jobs>, --parallel <jobs>",
      "Run the tests in parallel using the "
      "given number of jobs." },
    { "-Q,--quiet", "Make ctest quiet." },
    { "-O <file>, --output-log <file>", "Output to log file" },
    { "-N,--show-only", "Disable actual execution of tests." },
    { "-L <regex>, --label-regex <regex>",
      "Run tests with labels matching "
      "regular expression." },
    { "-R <regex>, --tests-regex <regex>",
      "Run tests matching regular "
      "expression." },
    { "-E <regex>, --exclude-regex <regex>",
      "Exclude tests matching regular "
      "expression." },
    { "-LE <regex>, --label-exclude <regex>",
      "Exclude tests with labels "
      "matching regular expression." },
    { "-FA <regex>, --fixture-exclude-any <regex>",
      "Do not automatically "
      "add any tests for "
      "fixtures matching "
      "regular expression." },
    { "-FS <regex>, --fixture-exclude-setup <regex>",
      "Do not automatically "
      "add setup tests for "
      "fixtures matching "
      "regular expression." },
    { "-FC <regex>, --fixture-exclude-cleanup <regex>",
      "Do not automatically "
      "add cleanup tests for "
      "fixtures matching "
      "regular expression." },
    { "-D <dashboard>, --dashboard <dashboard>", "Execute dashboard test" },
    { "-D <var>:<type>=<value>", "Define a variable for script mode" },
    { "-M <model>, --test-model <model>", "Sets the model for a dashboard" },
    { "-T <action>, --test-action <action>",
      "Sets the dashboard action to "
      "perform" },
    { "--track <track>", "Specify the track to submit dashboard to" },
    { "-S <script>, --script <script>",
      "Execute a dashboard for a "
      "configuration" },
    { "-SP <script>, --script-new-process <script>",
      "Execute a dashboard for a "
      "configuration" },
    { "-A <file>, --add-notes <file>", "Add a notes file with submission" },
    { "-I [Start,End,Stride,test#,test#|Test file], --tests-information",
      "Run a specific number of tests by number." },
    { "-U, --union", "Take the Union of -I and -R" },
    { "--rerun-failed", "Run only the tests that failed previously" },
    { "--repeat-until-fail <n>",
      "Require each test to run <n> "
      "times without failing in order to pass" },
    { "--max-width <width>", "Set the max width for a test name to output" },
    { "--interactive-debug-mode [0|1]", "Set the interactive mode to 0 or 1." },
    { "--no-label-summary", "Disable timing summary information for labels." },
    { "--no-subproject-summary",
      "Disable timing summary information for "
      "subprojects." },
    { "--build-and-test", "Configure, build and run a test." },
    { "--build-target", "Specify a specific target to build." },
    { "--build-nocmake", "Run the build without running cmake first." },
    { "--build-run-dir", "Specify directory to run programs from." },
    { "--build-two-config", "Run CMake twice" },
    { "--build-exe-dir", "Specify the directory for the executable." },
    { "--build-generator", "Specify the generator to use." },
    { "--build-generator-platform", "Specify the generator-specific platform." },
    { "--build-generator-toolset", "Specify the generator-specific toolset." },
    { "--build-project", "Specify the name of the project to build." },
    { "--build-makeprogram", "Specify the make program to use." },
    { "--build-noclean", "Skip the make clean step." },
    { "--build-config-sample",
      "A sample executable to use to determine the configuration" },
    { "--build-options", "Add extra options to the build step." },

    { "--test-command", "The test to run with the --build-and-test option." },
    { "--test-timeout", "The time limit in seconds, internal use only." },
    { "--test-load", "CPU load threshold for starting new parallel tests." },
    { "--tomorrow-tag", "Nightly or experimental starts with next day tag." },
    { "--overwrite", "Overwrite CTest configuration option." },
    { "--extra-submit <file>[;<file>]", "Submit extra files to the dashboard." },
    { "--force-new-ctest-process",
      "Run child CTest instances as new processes" },
    { "--schedule-random", "Use a random order for scheduling tests" },
    { "--submit-index",
      "Submit individual dashboard tests with specific index" },
    { "--timeout <seconds>", "Set a global timeout on all tests." },
    { "--stop-time <time>",
      "Set a time at which all tests should stop running." },
    { "--http1.0", "Submit using HTTP 1.0." },
    { "--no-compress-output", "Do not compress test output when submitting." },
    { "--print-labels", "Print all available test labels." },
    { nullptr, nullptr }
};


//============================================================================//

namespace pyct
{

//============================================================================//

namespace test
{

// this is a test driver program for cmCTest.
int main_driver(int argc, char const* const* argv)
{
#if defined(_WIN32)
    // Replace streambuf so we can output Unicode to console
    cmsys::ConsoleBuf::Manager consoleOut(std::cout);
    consoleOut.SetUTF8Pipes();
    cmsys::ConsoleBuf::Manager consoleErr(std::cerr, true);
    consoleErr.SetUTF8Pipes();
#endif

    cmsys::Encoding::CommandLineArguments encoding_args =
            cmsys::Encoding::CommandLineArguments::Main(argc, argv);
    argc = encoding_args.argc();
    argv = encoding_args.argv();

    cmSystemTools::DoNotInheritStdPipes();
    cmSystemTools::EnableMSVCDebugHook();
    cmSystemTools::InitializeLibUV();
    cmSystemTools::FindCMakeResources(argv[0]);

    // Dispatch 'ctest --launch' mode directly.
    if (argc >= 2 && strcmp(argv[1], "--launch") == 0)
    {
        return cmCTestLaunch::Main(argc, argv);
    }

    cmCTest inst;

    if (cmSystemTools::GetCurrentWorkingDirectory().empty())
    {
        cmCTestLog(&inst, ERROR_MESSAGE,
                   "Current working directory cannot be established."
                   << std::endl);
        return 1;
    }

    // If there is a testing input file, check for documentation options
    // only if there are actually arguments.  We want running without
    // arguments to run tests.
    if (argc > 1 ||
        !(cmSystemTools::FileExists("CTestTestfile.cmake") ||
          cmSystemTools::FileExists("DartTestfile.txt")))
    {
        if (argc == 1)
        {
            cmCTestLog(&inst, ERROR_MESSAGE,
                       "*********************************"
                       << std::endl
                       << "No test configuration file found!" << std::endl
                       << "*********************************" << std::endl);
        }
        cmDocumentation doc;
        doc.addCTestStandardDocSections();
        if (doc.CheckOptions(argc, argv))
        {
            // Construct and print requested documentation.
            cmCTestScriptHandler* ch =
                    static_cast<cmCTestScriptHandler*>(inst.GetHandler("script"));
            ch->CreateCMake();

            doc.SetShowGenerators(false);
            doc.SetName("ctest");
            doc.SetSection("Name", cmDocumentationName);
            doc.SetSection("Usage", cmDocumentationUsage);
            doc.PrependSection("Options", cmDocumentationOptions);
            return doc.PrintRequestedDocumentation(std::cout) ? 0 : 1;
        }
    }

    // copy the args to a vector
    std::vector<std::string> args;
    args.reserve(argc);
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    // run ctest
    std::string output;
    int res = inst.Run(args, &output);
    cmCTestLog(&inst, OUTPUT, output);

    return res;
}

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


