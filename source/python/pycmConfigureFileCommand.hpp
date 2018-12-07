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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef pycmConfigureFileCommand_hpp
#define pycmConfigureFileCommand_hpp

#include "cmConfigure.h"  // IWYU pragma: keep
#include "cmsys/RegularExpression.hxx"

#include "cmCommand.h"
#include "cmNewLineStyle.h"
#include <map>
#include <string>
#include <vector>

//============================================================================//

class cmExecutionStatus;

namespace pyct
{
//
// CMake invocation format
//
// configure_file(<INPUT_FILE>
//                <OUTPUT_FILE>
//                [COPYONLY]
//                [@ONLY])
//

class pycmConfigureFileCommand : public cmCommand
{
public:
    cmCommand* Clone() override { return new cmConfigureFileCommand; }

    /**
     * This is called when the command is first encountered in
     * the input file.
     */
    bool InitialPass(std::vector<std::string> const& args,
                     cmExecutionStatus&) override;

    void AddDefinition(const std::string& key, const std::string& val)
    {
        m_definitions[key] = val;
    }

private:
    int  ConfigureFile();
    int  ConfigureFile(const char* infile, const char* outfile, bool copyonly,
                       bool atOnly, bool escapeQuotes, cmNewLineStyle newLine);
    void ConfigureString(const std::string& input, std::string& output,
                         bool atOnly, bool escapeQuotes) const;
    const char* GetDefinition(const std::string& key);

    cmNewLineStyle NewLineStyle;

    std::string                        InputFile;
    std::string                        OutputFile;
    bool                               CopyOnly     = false;
    bool                               EscapeQuotes = false;
    bool                               AtOnly       = true;
    mutable cmsys::RegularExpression   cmDefineRegex;
    mutable cmsys::RegularExpression   cmDefine01Regex;
    mutable cmsys::RegularExpression   cmAtVarRegex;
    mutable cmsys::RegularExpression   cmNamedCurly;
    std::map<std::string, std::string> m_definitions;
};

#endif
