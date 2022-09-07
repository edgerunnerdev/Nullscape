// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#include "platform.hpp"

#if defined(TARGET_PLATFORM_LINUX)

#include "log.hpp"
#include "private/processlinux.hpp"
#include "stringhelpers.hpp"

#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>

namespace Genesis
{

ProcessLinux::ProcessLinux(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback, ProcessOnOutputCallback outputCallback)
    : ProcessImpl(executable, arguments, completionCallback, outputCallback)
    , m_Pid(-1)
    , m_Status(0)
{
}

ProcessLinux::~ProcessLinux()
{
}

void ProcessLinux::Run()
{
    m_Pid = fork();
    if (m_Pid == -1)
    {
        Log::Error() << "fork() failed.";
    }
    else
    {
        std::string executableName = GetExecutable().filename();
            char** ppArguments = new char*[2];

        // Executable name
        ppArguments[0] = new char[executableName.size() + 1];
        memcpy(ppArguments[0], executableName.data(), executableName.size());
        ppArguments[0][executableName.size()] = '\0';

        // Arguments
        ppArguments[1] = new char[GetArguments().size() + 1];
        memcpy(ppArguments[1], GetArguments().data(), GetArguments().size());
        ppArguments[1][GetArguments().size()] = '\0';

        if (execv(GetExecutable().c_str(), ppArguments) == -1)
        {
            Log::Error() << "execv() failed: " << strerror(errno); 
        }
    }
}

void ProcessLinux::Wait()
{
    waitpid(m_Pid, &m_Status, 0);
    if (OnCompletion != nullptr)
    {
        OnCompletion(GetExitCode());
    }
}

uint32_t ProcessLinux::GetExitCode() const
{
    return WEXITSTATUS(m_Status);
}

bool ProcessLinux::HasExited() const
{
    return WIFEXITED(m_Status);
}

} // namespace Genesis

#endif // TARGET_PLATFORM_WINDOWS