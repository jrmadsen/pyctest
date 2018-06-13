// MIT License
//
// Copyright (c) 2018, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef pycmExecuteProcessCommand_hpp_
#define pycmExecuteProcessCommand_hpp_

#include "cmConfigure.h"

#include <string>
#include <vector>
#include <sstream>

#include "cmCommand.h"
#include "cmExecutionStatus.h"
#include "cmListFileCache.h"
#include "cmAlgorithms.h"
#include "cmMakefile.h"
#include "cmProcessOutput.h"
#include "cmSystemTools.h"
#include "cmsys/Process.h"

//============================================================================//

namespace pyct
{

//
// CMake invocation format
//
// execute_process(COMMAND <cmd1> [args1...]]
//                 [COMMAND <cmd2> [args2...] [...]]
//                 [WORKING_DIRECTORY <directory>]
//                 [TIMEOUT <seconds>]
//                 [RESULT_VARIABLE <variable>]
//                 [RESULTS_VARIABLE <variable>]
//                 [OUTPUT_VARIABLE <variable>]
//                 [ERROR_VARIABLE <variable>]
//                 [INPUT_FILE <file>]
//                 [OUTPUT_FILE <file>]
//                 [ERROR_FILE <file>]
//                 [OUTPUT_QUIET]
//                 [ERROR_QUIET]
//                 [OUTPUT_STRIP_TRAILING_WHITESPACE]
//                 [ERROR_STRIP_TRAILING_WHITESPACE]
//                 [ENCODING <name>])
//

//============================================================================//

class pycmExecuteProcessCommand : public cmCommand
{
public:
    typedef std::string                     string_t;
    typedef std::stringstream               sstream_t;
    typedef std::vector<string_t>           strvec_t;
    typedef std::vector<strvec_t>           cmdvec_t;
    typedef std::vector<const char*>        charvec_t;
    typedef std::pair<string_t, string_t>   strpair_t;
    typedef cmProcessOutput::Encoding       encoding_t;

public:
    pycmExecuteProcessCommand();
    pycmExecuteProcessCommand(strvec_t);

    bool operator()();
    bool operator()(strvec_t const& args)
    {
        m_args_list.push_back(args);
        return (*this)();
    }

    cmCommand* Clone() override { return new pycmExecuteProcessCommand; }
    bool InvokeInitialPass(const std::vector<cmListFileArgument>& args,
                           cmExecutionStatus& status) override
    {
        strvec_t expandedArgs;
        for(const auto& itr : args)
            expandedArgs.push_back(itr.Value);
        return InitialPass(expandedArgs, status);
    }

    bool InitialPass(strvec_t const& args, cmExecutionStatus&) override
    {
        m_args_list.push_back(args);
        return (*this)();
    }

    //------------------------------------------------------------------------//
    //      variables that can be get and set
    //------------------------------------------------------------------------//
    #define STANDARD_GET_SET(type, func, var) \
    const type & func () const              { return var ; } \
    void         func ( type val )  { var = val; }

    STANDARD_GET_SET(string_t,      working_directory,  m_working_directory)
    STANDARD_GET_SET(string_t,      timeout,            m_timeout)
    STANDARD_GET_SET(string_t,      input_file,         m_inp_file)
    STANDARD_GET_SET(string_t,      output_file,        m_out_file)
    STANDARD_GET_SET(string_t,      error_file,         m_err_file)
    STANDARD_GET_SET(bool,          output_quiet,       m_out_quiet)
    STANDARD_GET_SET(bool,          error_quiet,        m_err_quiet)
    STANDARD_GET_SET(bool,          strip_output,       m_out_strip)
    STANDARD_GET_SET(bool,          strip_error,        m_err_strip)
    STANDARD_GET_SET(encoding_t,    encoding,           m_encoding)

    #undef STANDARD_GET_SET
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    //      variables that can be get
    //------------------------------------------------------------------------//
    #define STANDARD_GET(type, func, var) \
    const type & func () const              { return var ; }

    STANDARD_GET(string_t, result,  m_result)
    STANDARD_GET(string_t, results, m_results)
    STANDARD_GET(string_t, output,  m_out)
    STANDARD_GET(string_t, error,   m_err)

    #undef STANDARD_GET
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    void add_command(strvec_t arr) { m_args_list.push_back(arr); }
    string_t command_string() const
    {
        sstream_t ss;
        for(unsigned j = 0; j < m_args_list.size(); ++j)
        {
            ss << j << " : \"";
            for(unsigned i = 0; i < m_args_list.at(j).size(); ++i)
            {
                ss << m_args_list.at(j).at(i);
                if(i+1 < m_args_list.at(j).size())
                    ss << " ";
            }
            if(j+1 < m_args_list.size())
                ss << std::endl;
        }
        return ss.str();
    }

protected:
    //------------------------------------------------------------------------//
    // GET variables
    //------------------------------------------------------------------------//
    // command list
    cmdvec_t    m_args_list;
    // variables set during execution
    string_t    m_result;
    string_t    m_results;
    string_t    m_out;
    string_t    m_err;

protected:
    //------------------------------------------------------------------------//
    // GET/SET variables
    //------------------------------------------------------------------------//
    // extra
    string_t    m_working_directory;
    string_t    m_timeout;
    // file
    string_t    m_inp_file;
    string_t    m_out_file;
    string_t    m_err_file;
    // quiet
    bool        m_out_quiet;
    bool        m_err_quiet;
    // strip
    bool        m_out_strip;
    bool        m_err_strip;
    // encoding
    encoding_t  m_encoding;

};

//============================================================================//

} // namespace pyct

//============================================================================//

#endif
