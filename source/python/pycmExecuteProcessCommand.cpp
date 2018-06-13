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

#include "pycmExecuteProcessCommand.hpp"

#include "cmsys/Process.h"
#include <ctype.h>
#include <sstream>
#include <stdio.h>

#include "cmAlgorithms.h"
#include "cmMakefile.h"
#include "cmProcessOutput.h"
#include "cmSystemTools.h"

class cmExecutionStatus;

//============================================================================//

static bool pycmExecuteProcessCommandIsWhitespace(char c)
{
    return (isspace(static_cast<int>(c)) || c == '\n' || c == '\r');
}

//============================================================================//

void pycmExecuteProcessCommandFixText(std::vector<char>& output,
                                    bool strip_trailing_whitespace)
{
    // Remove \0 characters and the \r part of \r\n pairs.
    unsigned int in_index = 0;
    unsigned int out_index = 0;
    while (in_index < output.size())
    {
        char c = output[in_index++];
        if ((c != '\r' ||
             !(in_index < output.size() && output[in_index] == '\n')) &&
            c != '\0')
        {
            output[out_index++] = c;
        }
    }

    // Remove trailing whitespace if requested.
    if (strip_trailing_whitespace)
    {
        while (out_index > 0 &&
               pycmExecuteProcessCommandIsWhitespace(output[out_index - 1]))
        {
            --out_index;
        }
    }

    // Shrink the vector to the size needed.
    output.resize(out_index);

    // Put a terminator on the text string.
    output.push_back('\0');
}

//============================================================================//

void pycmExecuteProcessCommandAppend(std::vector<char>& output, const char* data,
                                   int length)
{
#if defined(__APPLE__)
    // HACK on Apple to work around bug with inserting at the
    // end of an empty vector.  This resulted in random failures
    // that were hard to reproduce.
    if (output.empty() && length > 0)
    {
        output.push_back(data[0]);
        ++data;
        --length;
    }
#endif
    output.insert(output.end(), data, data + length);
}

//============================================================================//

namespace pyct
{

//============================================================================//

pycmExecuteProcessCommand::pycmExecuteProcessCommand()
: m_working_directory(""),
  m_timeout(""),
  m_inp_file(""),
  m_out_file(""),
  m_err_file(""),
  m_out_quiet(true),
  m_err_quiet(false),
  m_out_strip(false),
  m_err_strip(true),
  m_encoding(cmProcessOutput::None)
{ }

//============================================================================//

pycmExecuteProcessCommand::pycmExecuteProcessCommand(strvec_t args)
: m_working_directory(""),
  m_timeout(""),
  m_inp_file(""),
  m_out_file(""),
  m_err_file(""),
  m_out_quiet(true),
  m_err_quiet(false),
  m_out_strip(false),
  m_err_strip(true),
  m_encoding(cmProcessOutput::None)
{
    m_args_list.push_back(args);
}

//============================================================================//

// pycmExecuteProcessCommand
bool pycmExecuteProcessCommand::operator()()
{
    //------------------------------------------------------------------------//
    auto strvec_to_charvec = [] (const strvec_t& _array)
    {
        charvec_t _carray;
        for(const auto& itr : _array)
            _carray.push_back(itr.c_str());
        return _carray;
    };
    //------------------------------------------------------------------------//
    auto vchar_to_string = [] (const std::vector<char>& _array)
    {
        string_t _str = string_t(&*_array.begin());
        assert(_str.length() == _array.size());
        return _str;
    };
    //------------------------------------------------------------------------//
    auto pchar_to_string = [] (char* _array, int nsize)
    {
        sstream_t _ss;
        for(int i = 0; i < nsize; ++i)
        {
            if(_array[i] == '\0')
                break;
            _ss << _array[i];
        }
        return _ss.str();
    };
    //------------------------------------------------------------------------//

    if(m_args_list.empty())
    {
        this->SetError("called with incorrect number of arguments");
        return false;
    }

    if(m_args_list.at(0).empty())
    {
        this->SetError("called with incorrect number of arguments");
        return false;
    }

    std::vector<std::vector<const char*>> cmds;
    for(const auto& itr : m_args_list)
        cmds.push_back(strvec_to_charvec(itr));

    bool& output_quiet = m_out_quiet;
    bool& error_quiet = m_err_quiet;
    bool& output_strip_trailing_whitespace = m_out_strip;
    bool& error_strip_trailing_whitespace = m_err_strip;
    std::string& timeout_string = m_timeout;
    std::string& input_file = m_inp_file;
    std::string& output_file = m_out_file;
    std::string& error_file = m_err_file;
    std::string& working_directory = m_working_directory;
    encoding_t& encoding = m_encoding;


    // Check for commands given.
    if (cmds.empty())
    {
        this->SetError(" called with no COMMAND argument.");
        return false;
    }

    // create command
    for (auto& cmd : cmds)
    {
        if (cmd.empty())
        {
            this->SetError(" given COMMAND argument with no value.");
            return false;
        }
        // Add the null terminating pointer to the command argument list.
        cmd.push_back(nullptr);
    }

    // Parse the timeout string.
    double timeout = -1;
    if (!timeout_string.empty())
    {
        if (sscanf(timeout_string.c_str(), "%lg", &timeout) != 1)
        {
            this->SetError(" called with TIMEOUT value that could not be parsed.");
            return false;
        }
    }

    // Create a process instance.
    cmsysProcess* cp = cmsysProcess_New();

    // Set the command sequence.
    for (auto const& cmd : cmds)
        cmsysProcess_AddCommand(cp, &*cmd.begin());

    // Set the process working directory.
    if (!working_directory.empty())
        cmsysProcess_SetWorkingDirectory(cp, working_directory.c_str());

    // Always hide the process window.
    cmsysProcess_SetOption(cp, cmsysProcess_Option_HideWindow, 1);

    // Check the output variables.
    bool merge_output = false;

    // if input file, pipe in stdin
    if (!input_file.empty())
        cmsysProcess_SetPipeFile(cp, cmsysProcess_Pipe_STDIN, input_file.c_str());

    // if no output file specified, use stdout
    if (!output_file.empty())
        cmsysProcess_SetPipeFile(cp, cmsysProcess_Pipe_STDOUT,
                                 output_file.c_str());

    // if no error file specified, use stderr
    if (!error_file.empty())
    {
        if (error_file == output_file)
            merge_output = true;
        else
            cmsysProcess_SetPipeFile(cp, cmsysProcess_Pipe_STDERR,
                                     error_file.c_str());
    }

    // only used when output_file == error_file
    if (merge_output)
        cmsysProcess_SetOption(cp, cmsysProcess_Option_MergeOutput, 1);

    // Set the timeout if any.
    if (timeout >= 0)
        cmsysProcess_SetTimeout(cp, timeout);

    // Start the process.
    cmsysProcess_Execute(cp);

    // Read the process output.
    std::vector<char> tempOutput;
    std::vector<char> tempError;
    int length;
    char* data;
    int p;
    cmProcessOutput processOutput(encoding);
    std::string strdata;

    while ((p = cmsysProcess_WaitForData(cp, &data, &length, nullptr), p))
    {
        if(p == cmsysProcess_Pipe_STDOUT)
        {
            processOutput.DecodeText(data, length, strdata, 1);
            // echo to stdout
            if (!output_quiet && !strdata.empty())
                cmSystemTools::Stdout(strdata.c_str(), strdata.size());
            pycmExecuteProcessCommandAppend(tempOutput, data, length);
        }

        if(p == cmsysProcess_Pipe_STDERR)
        {
            processOutput.DecodeText(data, length, strdata, 2);
            // echo to stderr
            if (!error_quiet && !strdata.empty())
                cmSystemTools::Stderr(strdata.c_str(), strdata.size());
            pycmExecuteProcessCommandAppend(tempError, data, length);
        }
    }

    // All output has been read.  Wait for the process to exit.
    cmsysProcess_WaitForExit(cp, nullptr);
    processOutput.DecodeText(tempOutput, tempOutput);
    processOutput.DecodeText(tempError, tempError);

    // Fix the text in the output strings.
    pycmExecuteProcessCommandFixText(tempOutput, output_strip_trailing_whitespace);
    pycmExecuteProcessCommandFixText(tempError, error_strip_trailing_whitespace);

    // Store the output obtained.
    if (!tempOutput.empty())
        m_out = vchar_to_string(tempOutput);

    if (!tempError.empty())
        m_err = vchar_to_string(tempError);

    // Store the result of running the process.
    switch (cmsysProcess_GetState(cp))
    {
        case cmsysProcess_State_Exited:
        {
            int v = cmsysProcess_GetExitValue(cp);
            char buf[16];
            sprintf(buf, "%d", v);
            m_result = pchar_to_string(buf, 16);
        }
            break;
        case cmsysProcess_State_Exception:
            m_result = string_t(cmsysProcess_GetExceptionString(cp));
            break;
        case cmsysProcess_State_Error:
            m_result = string_t(cmsysProcess_GetErrorString(cp));
            break;
        case cmsysProcess_State_Expired:
            m_result = string_t("Process terminated due to timeout");
            break;
    }

    // Store the result of running the processes.
    switch (cmsysProcess_GetState(cp))
    {
        case cmsysProcess_State_Exited:
        {
            std::vector<std::string> res;
            for (size_t i = 0; i < cmds.size(); ++i)
            {
                switch (cmsysProcess_GetStateByIndex(cp, static_cast<int>(i)))
                {
                    case kwsysProcess_StateByIndex_Exited:
                    {
                        int exitCode =
                                cmsysProcess_GetExitValueByIndex(cp, static_cast<int>(i));
                        char buf[16];
                        sprintf(buf, "%d", exitCode);
                        res.push_back(buf);
                    }
                        break;
                    case kwsysProcess_StateByIndex_Exception:
                        res.push_back(cmsysProcess_GetExceptionStringByIndex(
                                          cp, static_cast<int>(i)));
                        break;
                    case kwsysProcess_StateByIndex_Error:
                    default:
                        res.push_back("Error getting the child return code");
                        break;
                }
            }
            m_results = cmJoin(res, ";");
        } break;
        case cmsysProcess_State_Exception:
            m_results = cmsysProcess_GetExceptionString(cp);
            break;
        case cmsysProcess_State_Error:
            m_results = cmsysProcess_GetErrorString(cp);
            break;
        case cmsysProcess_State_Expired:
            m_results = "Process terminated due to timeout";
            break;
    }

    // Delete the process instance.
    cmsysProcess_Delete(cp);

    return true;
}

//============================================================================//

} // namespace pyct

//============================================================================//
