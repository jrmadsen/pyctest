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

#include "pycmConfigureFileCommand.hpp"

#include <sstream>
#include <vector>

#include "cmAlgorithms.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmNewLineStyle.h"
#include "cmSystemTools.h"
#include "cmake.h"
#include "cmsys/FStream.hxx"
#include "cmsys/RegularExpression.hxx"

class cmExecutionStatus;

//============================================================================//

namespace pyct
{
//============================================================================//

// cmConfigureFileCommand
bool
pycmConfigureFileCommand::InitialPass(std::vector<std::string> const& args,
                                      cmExecutionStatus&)
{
    if(args.size() < 2)
    {
        this->SetError("called with incorrect number of arguments, expected 2");
        return false;
    }

    std::string const& inFile = args[0];
    if(!cmSystemTools::FileIsFullPath(inFile))
        this->SetError("Full path to input file required");

    this->InputFile += inFile;

    // If the input location is a directory, error out.
    if(cmSystemTools::FileIsDirectory(this->InputFile))
    {
        std::ostringstream e;
        /* clang-format off */
        e << "input location\n"
          << "  " << this->InputFile << "\n"
          << "is a directory but a file was expected.";
        /* clang-format on */
        this->SetError(e.str());
        return false;
    }

    std::string const& outFile = args[1];
    if(!cmSystemTools::FileIsFullPath(outFile))
    {
        this->OutputFile = this->Makefile->GetCurrentBinaryDirectory();
        this->OutputFile += "/";
    }
    this->OutputFile += outFile;

    // If the output location is already a directory put the file in it.
    if(cmSystemTools::FileIsDirectory(this->OutputFile))
    {
        this->OutputFile += "/";
        this->OutputFile += cmSystemTools::GetFilenameName(inFile);
    }

    if(!this->Makefile->CanIWriteThisFile(this->OutputFile))
    {
        std::string e = "attempted to configure a file: " + this->OutputFile +
                        " into a source directory.";
        this->SetError(e);
        cmSystemTools::SetFatalErrorOccured();
        return false;
    }
    std::string errorMessage;
    if(!this->NewLineStyle.ReadFromArguments(args, errorMessage))
    {
        this->SetError(errorMessage);
        return false;
    }
    this->CopyOnly     = false;
    this->EscapeQuotes = false;

    std::string unknown_args;
    this->AtOnly = false;
    for(unsigned int i = 2; i < args.size(); ++i)
    {
        if(args[i] == "COPYONLY")
        {
            this->CopyOnly = true;
            if(this->NewLineStyle.IsValid())
            {
                this->SetError("COPYONLY could not be used in combination "
                               "with NEWLINE_STYLE");
                return false;
            }
        }
        else if(args[i] == "ESCAPE_QUOTES")
        {
            this->EscapeQuotes = true;
        }
        else if(args[i] == "@ONLY")
        {
            this->AtOnly = true;
        }
        else if(args[i] == "IMMEDIATE")
        {
            /* Ignore legacy option.  */
        }
        else if(args[i] == "NEWLINE_STYLE" || args[i] == "LF" ||
                args[i] == "UNIX" || args[i] == "CRLF" || args[i] == "WIN32" ||
                args[i] == "DOS")
        {
            /* Options handled by NewLineStyle member above.  */
        }
        else
        {
            unknown_args += " ";
            unknown_args += args[i];
            unknown_args += "\n";
        }
    }
    if(!unknown_args.empty())
    {
        std::string msg = "configure_file called with unknown argument(s):\n";
        msg += unknown_args;
        this->Makefile->IssueMessage(cmake::AUTHOR_WARNING, msg);
    }

    if(!this->ConfigureFile())
    {
        this->SetError("Problem configuring file");
        return false;
    }

    return true;
}

//============================================================================//

int
pycmConfigureFileCommand::ConfigureFile()
{
    this->cmDefineRegex.compile("#([ \t]*)cmakedefine[ \t]+([A-Za-z_0-9]*)");
    this->cmDefine01Regex.compile(
        "#([ \t]*)cmakedefine01[ \t]+([A-Za-z_0-9]*)");
    this->cmAtVarRegex.compile("(@[A-Za-z_0-9/.+-]+@)");
    this->cmNamedCurly.compile("^[A-Za-z0-9/_.+-]+{");

    return this->Makefile->ConfigureFile(
        this->InputFile.c_str(), this->OutputFile.c_str(), this->CopyOnly,
        this->AtOnly, this->EscapeQuotes, this->NewLineStyle);
}

//============================================================================//

const char*
pycmConfigureFileCommand::GetDefinition(const std::string& key)
{
    return (m_definitions.find(key) != m_definitions.end())
               ? m_definitions.find(key)->second.c_str()
               : nullptr;
}

//============================================================================//

void
pycmConfigureFileCommand::ConfigureString(const std::string& input,
                                          std::string& output, bool atOnly,
                                          bool escapeQuotes) const
{
    // Split input to handle one line at a time.
    std::string::const_iterator lineStart = input.begin();
    while(lineStart != input.end())
    {
        // Find the end of this line.
        std::string::const_iterator lineEnd = lineStart;
        while(lineEnd != input.end() && *lineEnd != '\n')
            ++lineEnd;

        // Copy the line.
        std::string line(lineStart, lineEnd);

        // Skip the newline character.
        bool haveNewline = (lineEnd != input.end());
        if(haveNewline)
            ++lineEnd;

        // Replace #cmakedefine instances.
        if(this->cmDefineRegex.find(line))
        {
            const char* def = this->GetDefinition(this->cmDefineRegex.match(2));
            if(!cmSystemTools::IsOff(def))
            {
                const std::string indentation = this->cmDefineRegex.match(1);
                cmSystemTools::ReplaceString(line,
                                             "#" + indentation + "cmakedefine",
                                             "#" + indentation + "define");
                output += line;
            }
            else
            {
                output += "/* #undef ";
                output += this->cmDefineRegex.match(2);
                output += " */";
            }
        }
        else if(this->cmDefine01Regex.find(line))
        {
            const std::string indentation = this->cmDefine01Regex.match(1);
            const char*       def =
                this->GetDefinition(this->cmDefine01Regex.match(2));
            cmSystemTools::ReplaceString(line,
                                         "#" + indentation + "cmakedefine01",
                                         "#" + indentation + "define");
            output += line;
            if(!cmSystemTools::IsOff(def))
                output += " 1";
            else
                output += " 0";
        }
        else
            output += line;

        if(haveNewline)
            output += "\n";

        // Move to the next line.
        lineStart = lineEnd;
    }

    // Perform variable replacements.
    this->ExpandVariablesInString(output, escapeQuotes, true, atOnly, nullptr,
                                  -1, true, true);
}

int
pycmConfigureFileCommand::ConfigureFile(const char* infile, const char* outfile,
                                        bool copyonly, bool atOnly,
                                        bool           escapeQuotes,
                                        cmNewLineStyle newLine)
{
    int res = 1;
    if(!cmSystemTools::FileExists(infile))
    {
        cmSystemTools::Error("File ", infile, " does not exist.");
        return 0;
    }
    std::string soutfile = outfile;
    std::string sinfile  = infile;
    cmSystemTools::ConvertToUnixSlashes(soutfile);

    mode_t perm = 0;
    cmSystemTools::GetPermissions(sinfile, perm);
    std::string::size_type pos = soutfile.rfind('/');
    if(pos != std::string::npos)
    {
        std::string path = soutfile.substr(0, pos);
        cmSystemTools::MakeDirectory(path);
    }

    if(copyonly)
    {
        if(!cmSystemTools::CopyFileIfDifferent(sinfile.c_str(),
                                               soutfile.c_str()))
            return 0;
    }
    else
    {
        std::string        newLineCharacters;
        std::ios::openmode omode = std::ios::out | std::ios::trunc;
        if(newLine.IsValid())
        {
            newLineCharacters = newLine.GetCharacters();
            omode |= std::ios::binary;
        }
        else
        {
            newLineCharacters = "\n";
        }
        std::string tempOutputFile = soutfile;
        tempOutputFile += ".tmp";
        cmsys::ofstream fout(tempOutputFile.c_str(), omode);
        if(!fout)
        {
            cmSystemTools::Error(
                "Could not open file for write in copy operation ",
                tempOutputFile.c_str());
            cmSystemTools::ReportLastSystemError("");
            return 0;
        }
        cmsys::ifstream fin(sinfile.c_str());
        if(!fin)
        {
            cmSystemTools::Error(
                "Could not open file for read in copy operation ",
                sinfile.c_str());
            return 0;
        }

        cmsys::FStream::BOM bom = cmsys::FStream::ReadBOM(fin);
        if(bom != cmsys::FStream::BOM_None && bom != cmsys::FStream::BOM_UTF8)
        {
            std::ostringstream e;
            e << "File starts with a Byte-Order-Mark that is not UTF-8:\n  "
              << sinfile;
            this->IssueMessage(cmake::FATAL_ERROR, e.str());
            return 0;
        }
        // rewind to copy BOM to output file
        fin.seekg(0);

        // now copy input to output and expand variables in the
        // input file at the same time
        std::string inLine;
        std::string outLine;
        while(cmSystemTools::GetLineFromStream(fin, inLine))
        {
            outLine.clear();
            this->ConfigureString(inLine, outLine, atOnly, escapeQuotes);
            fout << outLine << newLineCharacters;
        }
        // close the files before attempting to copy
        fin.close();
        fout.close();
        if(!cmSystemTools::CopyFileIfDifferent(tempOutputFile.c_str(),
                                               soutfile.c_str()))
        {
            res = 0;
        }
        else
        {
            cmSystemTools::SetPermissions(soutfile, perm);
        }
        cmSystemTools::RemoveFile(tempOutputFile);
    }
    return res;
}

//============================================================================//

}  // namespace pyct

//============================================================================//
