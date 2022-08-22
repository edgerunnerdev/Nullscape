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

#pragma once

#include "platform.hpp"

#if defined(TARGET_PLATFORM_WINDOWS)

#include "private/processimpl.hpp"
#include "process.hpp"

#include <atomic>
#include <Windows.h>

namespace Genesis
{

class ProcessWindows : public ProcessImpl
{
public:
    ProcessWindows(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback = nullptr, ProcessOnOutputCallback outputCallback = nullptr);
    ~ProcessWindows();

    virtual void Run() override;
    virtual void Wait() override;
    virtual uint32_t GetExitCode() const override;
    virtual bool HasExited() const override;

    void WaitOrTimerCallback();

private:
    PROCESS_INFORMATION m_ProcessInformation;
    bool m_HandlesValid;
    bool m_HasExited;
    DWORD m_ExitCode;
    HANDLE m_WaitHandle;
    std::atomic_bool m_ProcessExited;
};

} // namespace Genesis

#endif // TARGET_PLATFORM_WINDOWS