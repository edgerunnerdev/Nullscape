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
#include "process.hpp"
#include "private/processimpl.hpp"

#if defined(TARGET_PLATFORM_LINUX)
#include "private/processlinux.hpp"
#elif defined(TARGET_PLATFORM_WINDOWS)
#include "private/processwindows.hpp"
#endif

namespace Genesis
{
namespace Core
{

Process::Process(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback, ProcessOnOutputCallback outputCallback)
{
#if defined(TARGET_PLATFORM_LINUX)
    m_pImpl = std::make_unique<ProcessLinux>(executable, arguments, completionCallback, outputCallback);
#elif defined(TARGET_PLATFORM_WINDOWS)
    m_pImpl = std::make_unique<ProcessWindows>(executable, arguments, completionCallback, outputCallback);
#else
    static_assert(false); // NOT IMPLEMENTED
#endif
}

Process::~Process() {}

void Process::Run()
{
    m_pImpl->Run();
}

void Process::Wait()
{
    m_pImpl->Wait();
}

uint32_t Process::GetExitCode() const 
{
    return m_pImpl->GetExitCode();
}

} // namespace Core
} // namespace Genesis