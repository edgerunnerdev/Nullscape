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

namespace Genesis
{

ProcessLinux::ProcessLinux(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback, ProcessOnOutputCallback outputCallback)
    : ProcessImpl(executable, arguments, completionCallback, outputCallback)
{
}

ProcessLinux::~ProcessLinux()
{
}

void ProcessLinux::Run()
{
}

void ProcessLinux::Wait()
{
}

void ProcessLinux::WaitOrTimerCallback() 
{
}

uint32_t ProcessLinux::GetExitCode() const
{
    return -1;
}

bool ProcessLinux::HasExited() const
{
    return true;
}

} // namespace Genesis

#endif // TARGET_PLATFORM_WINDOWS