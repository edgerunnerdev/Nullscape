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

#if defined(TARGET_PLATFORM_LINUX)

#include <unistd.h>

#include "private/processimpl.hpp"
#include "process.hpp"

namespace Genesis
{

class ProcessLinux : public ProcessImpl
{
public:
    ProcessLinux(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback = nullptr, ProcessOnOutputCallback outputCallback = nullptr);
    ~ProcessLinux();

    virtual void Run() override;
    virtual void Wait() override;
    virtual uint32_t GetExitCode() const override;
    virtual bool HasExited() const override;

    void WaitOrTimerCallback();

private:
    pid_t m_Pid;
    int m_Status;
};

} // namespace Genesis

#endif // TARGET_PLATFORM_LINUX