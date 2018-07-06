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

//============================================================================//

// define helper macros
#define pyobj_cast(_var, _type, _pyobject) _type * _var = _pyobject.cast< _type * >()

//============================================================================//

typedef std::stringstream                   sstream_t;
typedef std::string                         string_t;
typedef std::vector<string_t>               strvec_t;
typedef std::vector<char*>                  charvec_t;
typedef pyct::pycmExecuteProcessCommand     execProcCmd_t;

//============================================================================//

static const char* cmDocumentationName[][2] =
{
    { nullptr, "  ctest - Testing driver provided by CMake." },
    { nullptr, nullptr }
};

//============================================================================//

static const char* cmDocumentationUsage[][2] =
{
    { nullptr, "  ctest [options]" },
    { nullptr, nullptr }
};

//============================================================================//

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

pycmTestGenerator::pycmTestGenerator(pycmTest* test)
: cmScriptGenerator("CTEST_CONFIGURATION_TYPE", std::vector<string_t>()),
  m_test(test),
  m_test_generated(false)
{ }

//============================================================================//

pycmTestGenerator::~pycmTestGenerator()
{ }

//============================================================================//

bool pycmTestGenerator::TestsForConfig(const string_t& config)
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
                                              const string_t&,
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
    std::vector<string_t> const& command = m_test->GetCommand();

    string_t exe = command[0];
    cmSystemTools::ConvertToUnixSlashes(exe);
    fout << indent;
    fout << "add_test(";
    fout << m_test->GetName() << " \"" << exe << "\"";

    for (std::vector<string_t>::const_iterator argit = command.begin() + 1;
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

// this is a test driver program for cmCTest.
int ctest_main_driver(int argc, char const* const* argv)
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
    std::vector<string_t> args;
    args.reserve(argc);
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    // run ctest
    string_t output;
    int res = inst.Run(args, &output);
    cmCTestLog(&inst, OUTPUT, output);

    return res;
}

//============================================================================//

} // namespace pycm

//============================================================================//

PYBIND11_MODULE(pyctest, ct)
{
    py::add_ostream_redirect(ct, "ostream_redirect");

    //------------------------------------------------------------------------//
    //
    //      Initializers
    //
    //------------------------------------------------------------------------//
    // create a new test and add to test list
    auto test_init = [=] ()
    {
        auto obj = new pyct::pycmTest();
        pyct::get_test_list()->push_back(obj);
        return new pyct::pycmTestWrapper(obj);
    };
    //------------------------------------------------------------------------//
    // create a new test and add to test list
    auto var_init = [=] (string_t var, string_t val,
                    pyct::pycmVariable::cache_t cache,
                    string_t doc, bool force)
    {
        auto obj = new pyct::pycmVariable(var, val, cache, doc, force);
        if(var.length() == 0)
            std::cerr << "Warning! Variable name must be longer than zero. "
                      << "This variable will not be added to output."
                      << std::endl;
        else
            pyct::get_test_variables()->push_back(obj);
        return new pyct::pycmVariableWrapper(obj);
    };
    //------------------------------------------------------------------------//
    auto proc_init = [=] (py::list cmd)
    {
        pyct::strvec_t _cmd;
        for(const auto& itr : cmd)
            _cmd.push_back(itr.cast<string_t>());
        return new pyct::pycmExecuteProcessCommand(_cmd);
    };
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    //
    //      Helpers
    //
    //------------------------------------------------------------------------//
    // attributes that should be filled by user
    std::vector<string_t> blank_attr =
    {
        "CHECKOUT_COMMAND",
        "UPDATE_COMMAND",
        "CONFIGURE_COMMAND",
        "BUILD_COMMAND",
        "COVERAGE_COMMAND",
        "MEMORYCHECK_COMMAND",
        "GENERATOR",
        "GENERATOR_PLATFORM",
        "MODEL",
        "SOURCE_DIRECTORY",
        "BINARY_DIRECTORY",
        "TRIGGER",
        "CUSTOM_MAXIMUM_NUMBER_OF_ERRORS",
        "CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS",
        "CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE",
        "CUSTOM_COVERAGE_EXCLUDE"
    };
    //------------------------------------------------------------------------//
    auto upperstr = [] (string_t tmp)
    {
        for(auto& itr : tmp)
            itr = toupper(itr);
        return tmp;
    };

    //------------------------------------------------------------------------//
    //
    //      Binding functions
    //
    //------------------------------------------------------------------------//
    auto test_add = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::pycmTestWrapper, obj);
        pyct::test_list_t* test_list = pyct::get_test_list();
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
        pyobj_cast(_obj, pyct::pycmTestWrapper, obj);
        pyct::test_list_t* test_list = pyct::get_test_list();
        if(test_list)
        {
            auto itr = std::find(test_list->begin(), test_list->end(), _obj->get());
            if(itr != test_list->end())
                test_list->erase(itr);
        }
    };
    //------------------------------------------------------------------------//
    auto test_find = [=] (string_t test_name)
    {
        pyct::test_list_t* test_list = pyct::get_test_list();
        pyct::pycmTestWrapper* test = nullptr;
        if(test_list)
        {
            for(auto itr : *test_list)
                if(itr->GetName() == test_name)
                    return new pyct::pycmTestWrapper(itr);
        }
        return test;
    };
    //------------------------------------------------------------------------//
    auto proc_exec = [=] (py::object obj, py::list args)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        bool ret = true;
        if(args.size() > 0)
        {
            pyct::strvec_t _args;
            for(const auto& itr : args)
                _args.push_back(itr.cast<string_t>());
            ret = (*_obj)(_args);
        }
        else
            ret = (*_obj)();

        if(!ret)
        {
            std::stringstream ss;
            ss << "Error running command!\n" << std::endl;
            ss << _obj->command_string() << std::endl;
            ss << "Result code: " << _obj->result() << std::endl;
            ss << "Output: " << _obj->output() << std::endl;
            ss << "Error: " << _obj->error() << std::endl;
            throw std::runtime_error(ss.str().c_str());
        }
    };
    //------------------------------------------------------------------------//
    auto proc_cmd = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        return _obj->command_string();
    };
    //------------------------------------------------------------------------//
    auto proc_out = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        return _obj->output();
    };
    //------------------------------------------------------------------------//
    auto proc_err = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        return _obj->error();
    };
    //------------------------------------------------------------------------//
    auto proc_ret = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        return _obj->result();
    };
    //------------------------------------------------------------------------//
    auto proc_rets = [=] (py::object obj)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        return _obj->results();
    };
    //------------------------------------------------------------------------//
    auto proc_cmd_add = [=] (py::object obj, py::list args)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        pyct::strvec_t _args;
        for(const auto& itr : args)
            _args.push_back(itr.cast<string_t>());
        _obj->add_command(_args);
    };
    //------------------------------------------------------------------------//
    auto proc_dir_set = [=] (py::object obj, string_t val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->working_directory(val);
    };
    //------------------------------------------------------------------------//
    auto proc_timeout_set = [=] (py::object obj, string_t val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->timeout(val);
    };
    //------------------------------------------------------------------------//
    auto proc_inpf_set = [=] (py::object obj, string_t val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->input_file(val);
    };
    //------------------------------------------------------------------------//
    auto proc_outf_set = [=] (py::object obj, string_t val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->output_file(val);
    };
    //------------------------------------------------------------------------//
    auto proc_errf_set = [=] (py::object obj, string_t val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->error_file(val);
    };
    //------------------------------------------------------------------------//
    auto proc_out_quiet_set = [=] (py::object obj, bool val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->output_quiet(val);
    };
    //------------------------------------------------------------------------//
    auto proc_err_quiet_set = [=] (py::object obj, bool val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->error_quiet(val);
    };
    //------------------------------------------------------------------------//
    auto proc_out_strip_set = [=] (py::object obj, bool val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->strip_output(val);
    };
    //------------------------------------------------------------------------//
    auto proc_err_strip_set = [=] (py::object obj, bool val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->strip_error(val);
    };
    //------------------------------------------------------------------------//
    auto proc_encoding_set = [=] (py::object obj, execProcCmd_t::encoding_t val)
    {
        pyobj_cast(_obj, pyct::pycmExecuteProcessCommand, obj);
        _obj->encoding(val);
    };
    //------------------------------------------------------------------------//
    auto exe_path = [=] ()
    {
        string_t _pyctest_file = ct.attr("__file__").cast<string_t>();
        auto locals = py::dict("_pyctest_file"_a = _pyctest_file);
        py::exec(R"(
                 import os
                 _ctest_path = os.path.join(os.path.dirname(_pyctest_file),
                                            os.path.join("bin", "ctest"))
                 if not os.path.exists(_ctest_path):
                     print("Warning! Executable not found @ '{}'".format(_ctest_path))
                 )",
                 py::globals(), locals);
        return locals["_ctest_path"].cast<string_t>();
    };
    //------------------------------------------------------------------------//
    auto str2char_convert = [] (const string_t& _str)
    {
        char* pc = new char[_str.size()+1];
        std::strcpy(pc, _str.c_str());
        pc[_str.size()] = '\0';
        return pc;
    };
    //------------------------------------------------------------------------//
    auto run = [=] (std::vector<string_t> pargs, string_t working_dir)
    {
        charvec_t cargs;
        // convert list elements to char*
        for(auto itr : pargs)
            cargs.push_back(str2char_convert(itr));

        // structures passed
        int argc = pargs.size() + 1;
        char** argv = new char*[argc];

        // ctest executable
        auto _exe = exe_path();
        argv[0] = str2char_convert(_exe);

        // fill argv
        for(unsigned i = 1; i < argc; ++i)
            argv[i] = cargs[i-1];

        // change working directory
        auto locals = py::dict("working_dir"_a = working_dir);
        py::exec(R"(
                 import os

                 origwd = os.getcwd()
                 if len(working_dir) > 0:
                     if not os.path.exists(working_dir):
                         os.makedirs(working_dir)
                     os.chdir(working_dir)
                 )",
                 py::globals(), locals);

        string_t origwd = locals["origwd"].cast<string_t>();
        int ret = pyct::ctest_main_driver(argc, argv);

        locals = py::dict("working_dir"_a = origwd);
        py::exec(R"(
                 import os

                 os.chdir(working_dir)
                 )",
                 py::globals(), locals);

        if(ret > 0)
            std::cerr << "Error! Non-zero exit code: " << ret << std::endl;
    };
    //------------------------------------------------------------------------//
    auto copy_cdash = [=] (string_t dir)
    {
        string_t _pyctest_file = ct.attr("__file__").cast<string_t>();
        auto locals = py::dict("_pyctest_file"_a = _pyctest_file,
                               "_dir"_a = dir);
        py::exec(R"(
                 import os
                 from shutil import copyfile

                 _cdash_path = os.path.join(os.path.dirname(_pyctest_file),
                                            "cdash")
                 _dir = os.path.realpath(_dir)
                 if not os.path.exists(_dir):
                     os.makedirs(_dir)
                 if not os.path.exists(_cdash_path):
                     print("Warning! CDash directory not found @ '{}'".format(_cdash_path))
                 else:
                     for f in [ "Build", "Coverage", "Glob", "Init", "MemCheck",
                                "Stages", "Submit", "Test", "Utilities"]:
                         fsrc = os.path.join(_cdash_path, "{}.cmake".format(f))
                         fdst = os.path.join(_dir, "{}.cmake".format(f))
                         copyfile(fsrc, fdst)
                 )",
                 py::globals(), locals);
    };
    //------------------------------------------------------------------------//
    auto generate_ctest_config = [=] (string_t dir)
    {
        string_t fname = "CTestConfig.cmake";
        pyct::configure_filepath(dir, fname);

        std::stringstream ssfs;
        for(const auto& itr : pyct::get_config_attributes())
        {
            string_t attr_var = "CTEST_" + itr;
            string_t attr_val = ct.attr(itr.c_str()).cast<string_t>();
            pyct::pycmVariable _pyvar(attr_var, attr_val);
            if(attr_val.length() == 0)
                ssfs << "# ";
            ssfs << _pyvar << std::endl;
        }

        std::ofstream ofs(fname.c_str());
        if(!ofs)
        {
            std::cerr << "pyct::generate_ctest_config -- Error opening " << fname
                      << "!!!" << std::endl;
            std::cout << ssfs.str() << std::endl;
        }
        else
        {
            ofs << ssfs.str() << std::endl;
        }
        ofs.close();

    };
    //------------------------------------------------------------------------//
    auto generate_custom_config = [=] (string_t dir)
    {
        string_t fname = "CTestCustom.cmake";
        pyct::configure_filepath(dir, fname);

        //--------------------------------------------------------------------//
        auto generate_attr = [=] (sstream_t& ssfs, string_t itr, bool required)
        {
            string_t attr_var = "CTEST_" + itr;
            string_t attr_val = "";
            try { attr_val = ct.attr(itr.c_str()).cast<string_t>(); }
            catch (...)
            {
                sstream_t msg;
                msg << itr << " (i.e. " << attr_var << ") is not set.";
                if(required)
                    throw std::runtime_error(msg.str());
                else
                    std::cerr << "Warning! " << msg.str() << std::endl;
            }
            pyct::pycmVariable _pyvar(attr_var, attr_val);
            if(attr_val.length() == 0)
                ssfs << "# ";
            ssfs << _pyvar << std::endl;
        };
        //--------------------------------------------------------------------//

        std::stringstream ssfs;
        for(const auto& itr : pyct::get_reqired_attributes())
            generate_attr(ssfs, itr, true);

        for(const auto& itr : pyct::get_custom_attributes())
            generate_attr(ssfs, itr, false);

        for(const auto& itr : *pyct::get_test_variables())
            ssfs << *itr << std::endl;

        std::ofstream ofs(fname.c_str());
        if(!ofs)
        {
            std::cerr << "pyct::generate_custom_config -- Error opening " << fname
                      << "!!!" << std::endl;
            std::cout << ssfs.str() << std::endl;
        }
        else
        {
            ofs << ssfs.str() << std::endl;
        }
        ofs.close();
    };
    //------------------------------------------------------------------------//
    auto generate_config = [=] (string_t dir)
    {
        generate_ctest_config(dir);
        generate_custom_config(dir);
        copy_cdash(dir);
    };
    //------------------------------------------------------------------------//
    auto copy_files = [=] (py::list files,
                           std::string from_dir,
                           std::string target_dir)
    {
        auto locals = py::dict("files"_a = files,
                               "from_dir"_a = from_dir,
                               "target_dir"_a = target_dir);
        locals["pyctest"] = ct;
        py::exec(R"(
                 import os
                 import shutil

                 if len(from_dir) == 0:
                     from_dir = os.getcwd()

                 if len(target_dir) == 0:
                     target_dir = pyctest.BINARY_DIRECTORY

                 if len(target_dir) == 0:
                     raise Exception("Please provide a target directory or set pyctest.pyctest.BINARY_DIRECTORY")

                 def files_append(fname, files):
                     if not fname in files:
                         files.append(fname)

                 files_append("PyCTestPreInit.cmake", files)
                 files_append("PyCTestPostInit.cmake", files)

                 def _copy_file(_file, _from_dir, _target_dir):
                     fsrc = os.path.join(_from_dir, '{}'.format(_file))
                     fdst = os.path.join(_target_dir, '{}'.format(_file))
                     if not os.path.exists(_target_dir):
                         os.makedirs(_dir)
                     if os.path.exists(fsrc) and fsrc != fdst:
                         print("Copying file '{}' to '{}'...".format(fsrc, fdst))
                         shutil.copyfile(fsrc, fdst)
                         shutil.copymode(fsrc, fdst)

                 for f in files:
                     _copy_file(f, from_dir, target_dir)
                 )",
                 py::globals(), locals);
    };
    //------------------------------------------------------------------------//
    auto git_checkout = [=] (std::string repo_url,
                             std::string source_dir,
                             bool update)
    {
        auto locals = py::dict("repo_url"_a = repo_url,
                               "source_dir"_a = source_dir,
                               "update"_a = update);
        locals["pyctest"] = ct;
        py::exec(R"(
                 import os

                 def _perform_update(_source_dir):
                     # try to pull latest version. not a fatal error if fails
                     try:
                         import git
                         repo = git.Repo(_source_dir)
                         repo.remotes['origin'].pull()
                     except:
                         # execute a pull command
                         cmd = pyctest.command(["git", "pull"])
                         cmd.SetWorkingDirectory(_source_dir)
                         cmd.SetOutputQuiet(False)
                         cmd.SetErrorQuiet(False)
                         cmd.Execute()

                 _git_dir = os.path.join(source_dir, ".git")
                 print("Checking for '{}'...".format(_git_dir))
                 # do we want to checkout or try to update?
                 if os.path.exists(_git_dir):
                     if update:
                         _perform_update(source_dir)
                 else:
                     # execute a checkout command
                     cmd = pyctest.command(["git", "clone", repo_url, source_dir])
                     cmd.SetWorkingDirectory(os.getcwd())
                     cmd.SetOutputQuiet(False)
                     cmd.SetErrorQuiet(False)
                     cmd.Execute()
                 )",
                 py::globals(), locals);
    };
    //------------------------------------------------------------------------//
    auto add_note = [=] (string_t dir, string_t file, bool clobber)
    {
        string_t fname = "CTestNotes.cmake";
        pyct::configure_filepath(dir, fname);

        std::ofstream ofs;
        if(clobber)
            ofs.open(fname.c_str());
        else
            ofs.open(fname.c_str(),
                     std::ios_base::out | std::ios_base::app);

        if(ofs)
        {
            std::stringstream ssfs;
            string_t quote = "\"";
            ssfs << "\nlist(APPEND CTEST_NOTES_FILES " << quote << file << quote
                 << ")" << std::endl;
            ofs << ssfs.str() << std::endl;
        }
    };
    //------------------------------------------------------------------------//
    auto add_presubmit_command = [=] (string_t dir, py::list cmd, bool clobber)
    {
        string_t fname = "CTestPreSubmitScript.cmake";
        pyct::configure_filepath(dir, fname);

        std::ofstream ofs;
        if(clobber)
            ofs.open(fname.c_str());
        else
            ofs.open(fname.c_str(),
                     std::ios_base::out | std::ios_base::app);

        if(ofs)
        {
            std::stringstream ssfs;
            ssfs << "\nexecute_process(COMMAND ";
            string_t quote = "\"";
            for(const auto& itr : cmd)
                ssfs << quote << itr << quote << " ";
            ssfs << "\n";
            ssfs << "WORKING_DIRECTORY " << dir
                 << ")";
            ofs << ssfs.str() << std::endl;
        }
    };
    //------------------------------------------------------------------------//

    py::class_<pyct::pycmTestWrapper>           _test   (ct,    "test",
                                                         "CTest test object -- works like add_test(...)");
    py::class_<pyct::pycmVariableWrapper>       _var    (ct,    "set",
                                                         "Set a variable -- works like set(...)");
    py::class_<pyct::pycmExecuteProcessCommand> _cmd    (ct,    "command",
                                                         "Run a command -- works like execute_process(...)");
    py::enum_<pyct::pycmVariable::cache_t>      _cache  (ct, "cache", py::arithmetic(),
                                                         "Cache types");
    py::enum_<execProcCmd_t::encoding_t>        _encode (ct, "encoding", py::arithmetic(),
                                                         "Encoding types");

    _cache.value    ("NONE",        pyct::pycmVariable::cache_t::NONE)
            .value  ("BOOL",        pyct::pycmVariable::cache_t::BOOL)
            .value  ("FILEPATH",    pyct::pycmVariable::cache_t::FILEPATH)
            .value  ("PATH",        pyct::pycmVariable::cache_t::PATH)
            .value  ("STRING",      pyct::pycmVariable::cache_t::STRING)
            .value  ("INTERNAL",    pyct::pycmVariable::cache_t::INTERNAL);

    _encode.value   ("None",        cmProcessOutput::Encoding::None)
            .value  ("Auto",        cmProcessOutput::Encoding::Auto)
            .value  ("UTF8",        cmProcessOutput::Encoding::UTF8)
            .value  ("ANSI",        cmProcessOutput::Encoding::ANSI)
            .value  ("OEM",         cmProcessOutput::Encoding::OEM);

    ct.attr("PROJECT_NAME") = "";
    ct.attr("NIGHTLY_START_TIME") = "01:00:00 UTC";
    ct.attr("DROP_METHOD") = "https";
    ct.attr("DROP_SITE") = "cdash.nersc.gov";
    ct.attr("DROP_LOCATION") = "/submit.php?project=${CTEST_PROJECT_NAME}";
    ct.attr("CDASH_VERSION") = "1.6";
    ct.attr("CDASH_QUERY_VERSION") = "TRUE";
    ct.attr("TIMEOUT") = "7200";

    for(const auto& itr : blank_attr)
        ct.attr(upperstr(itr).c_str()) = "";

    ct.def("add_test", test_add, "Add a test");
    ct.def("remove_test", test_remove, "Remove a test");
    ct.def("find_test", test_find, "Find a test by name");
    ct.def("generate_test_file", &pyct::generate_test_file,
           "Generate a CTestTestfile.cmake",
           py::arg("output_dir") = "");
    ct.def("copy_files", copy_files,
           "Helper method to copy files over to binary dir",
           py::arg("files") = py::list(),
           py::arg("from_dir") = "",
           py::arg("target_dir") = "");
    ct.def("git_checkout", git_checkout,
           "Perform git checkout a code and optionally update if already exists",
           py::arg("repo_url"),
           py::arg("source_dir"),
           py::arg("update") = true);
    ct.def("generate_config", generate_config,
           "Generate PyCTestConfig.cmake file",
           py::arg("output_dir") = "");
    ct.def("add_presubmit_command", add_presubmit_command,
           "Add a command to be executed before submission",
           py::arg("dir") = "", py::arg("cmd") = py::list(),
           py::arg("clobber") = false);
    ct.def("add_note", add_note,
           "Add a note to the dashboard",
           py::arg("dir") = "", py::arg("file") = "",
           py::arg("clobber") = false);
    ct.def("exe_path", exe_path, "Path to ctest executable");
    ct.def("run", run, "Run CTest",
           py::arg("args") = py::list(),
           py::arg("working_directory") = "");

    _test.def(py::init(test_init), "Test for CTest");
    _test.def("SetName", &pyct::set_name, "Set test name");
    _test.def("GetName", &pyct::get_name, "Get test name");
    _test.def("SetCommand", &pyct::set_command, "Set the command for the test");
    _test.def("GetCommand", &pyct::get_command, "Get the command for the test");
    _test.def("SetProperty", &pyct::set_property, "Set a test property");
    _test.def("GetProperty", &pyct::get_property, "Get a test property");
    _test.def("GetPropertyAsBool", &pyct::get_property_as_bool, "Get property as boolean");

    _var.def(py::init(var_init), "Set a variable in CTestInit.cmake",
             py::arg("variable") = "",
             py::arg("value") = "",
             py::arg("cache") = pyct::pycmVariable::cache_t::NONE,
             py::arg("doc") = "",
             py::arg("force") = false);

    _cmd.def(py::init(proc_init), "Object to execute a process",
             py::arg("args") = py::list());
    _cmd.def("Exec", proc_exec, "Execute (i.e. run)",
             py::arg("args") = py::list());
    _cmd.def("Execute", proc_exec, "Execute (i.e. run)",
             py::arg("args") = py::list());
    _cmd.def("Command", proc_cmd, "Get the argument list");
    _cmd.def("Output", proc_out, "Get the output string");
    _cmd.def("Error", proc_err, "Get the error string");
    _cmd.def("Result", proc_ret, "Get the result (return code) string");
    _cmd.def("Results", proc_rets, "Get the results");
    _cmd.def("AddCommand", proc_cmd_add, "Add a command");

    _cmd.def("SetWorkingDirectory", proc_dir_set, "Set the working directory");
    _cmd.def("SetTimeout", proc_timeout_set, "Set the process timeout");
    _cmd.def("SetInputFile", proc_inpf_set, "Set the input file");
    _cmd.def("SetOutputFile", proc_outf_set, "Set the output file");
    _cmd.def("SetErrorFile", proc_errf_set, "Set the error file");
    _cmd.def("SetOutputQuiet", proc_out_quiet_set, "Suppress output");
    _cmd.def("SetErrorQuiet", proc_err_quiet_set, "Suppress error");
    _cmd.def("SetOutputStripTrailingWhitespace", proc_out_strip_set, "Strip trailing whitespace from output");
    _cmd.def("SetErrorStripTrailingWhitespace", proc_err_strip_set, "Strip trailing whitespace from error");
    _cmd.def("SetEncoding", proc_encoding_set, "Set the process encoding");

    //------------------------------------------------------------------------//
    auto get_git_branch = [=] (string_t dir)
    {
        auto locals = py::dict("_dir"_a = dir);
        locals["pyctest"] = ct;
        auto globals = py::globals();
        py::exec(R"(
                 import os
                 from shutil import copyfile

                 if _dir is None:
                     _dir = os.getcwd()
                 cmd = pyctest.command(["git", "rev-parse", "--abbrev-ref", "HEAD"])
                 cmd.SetWorkingDirectory(_dir)
                 cmd.SetOutputStripTrailingWhitespace(True)
                 cmd.Execute()
                 _branch = cmd.Output()
                 )",
                 globals, locals);
        string_t _branch = locals["_branch"].cast<string_t>();
        return _branch;
    };

    ct.def("GetGitBranch", get_git_branch, "Get the branch name of a git repo",
           py::arg("dir") = py::none());
}

//============================================================================//


