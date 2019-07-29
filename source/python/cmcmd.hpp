/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef _cmcmd_hpp
#define _cmcmd_hpp

#if defined(_WIN32)
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#endif

#include "cmConfigure.h"  // IWYU pragma: keep
#include "cmCryptoHash.h"

#include <string>
#include <vector>

class cmcmd
{
public:
    /**
     * Execute commands during the build process. Supports options such
     * as echo, remove file etc.
     */
    static int ExecuteCMakeCommand(std::vector<std::string>&);

protected:
    static int  HandleCoCompileCommands(std::vector<std::string>& args);
    static int  HashSumFile(std::vector<std::string>& args, cmCryptoHash::Algo algo);
    static int  SymlinkLibrary(std::vector<std::string>& args);
    static int  SymlinkExecutable(std::vector<std::string>& args);
    static bool SymlinkInternal(std::string const& file, std::string const& link);
    static int  ExecuteEchoColor(std::vector<std::string>& args);
    static int  ExecuteLinkScript(std::vector<std::string>& args);
    static int  WindowsCEEnvironment(const char* version, const std::string& name);
    static int  VisualStudioLink(std::vector<std::string> const& args, int type);
};

#endif
