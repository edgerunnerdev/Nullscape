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

#if defined(TARGET_PLATFORM_WINDOWS)

#include "log.hpp"
#include "private/processwindows.hpp"
#include "stringhelpers.hpp"

#include <sstream>

namespace Genesis
{

void CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    ProcessWindows* pProcess = reinterpret_cast<ProcessWindows*>(lpParameter);
    pProcess->WaitOrTimerCallback();
}

ProcessWindows::ProcessWindows(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback, ProcessOnOutputCallback outputCallback)
    : ProcessImpl(executable, arguments, completionCallback, outputCallback)
{
    ZeroMemory(&m_ProcessInformation, sizeof(m_ProcessInformation));
    m_HandlesValid = false;
    m_HasExited = false;
    m_ExitCode = 0;
    m_ProcessExited = false;
}

ProcessWindows::~ProcessWindows()
{
    if (m_HandlesValid)
    {
        CloseHandle(m_ProcessInformation.hProcess);
        CloseHandle(m_ProcessInformation.hThread);
        UnregisterWait(m_WaitHandle);
    }
}

void ProcessWindows::Run()
{
    STARTUPINFO startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));

    std::wstringstream ss;
    ss << GetExecutable() << " " << ToWString(GetArguments());
    if (!CreateProcessW(nullptr, ss.str().data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &m_ProcessInformation))
    {
        Log::Error() << "CreateProcess failed: " << GetLastError();
        return;
    }
    else
    {
        RegisterWaitForSingleObject(&m_WaitHandle, m_ProcessInformation.hProcess, &Genesis::WaitOrTimerCallback, this, INFINITE, WT_EXECUTEONLYONCE);
        m_HandlesValid = true;
    }
}

void ProcessWindows::Wait()
{
    DWORD result = WaitForSingleObject(m_ProcessInformation.hProcess, INFINITE);
    while (m_ProcessExited == false) {}

    GetExitCodeProcess(m_ProcessInformation.hProcess, &m_ExitCode);
    CloseHandle(m_ProcessInformation.hProcess);
    CloseHandle(m_ProcessInformation.hThread);
    UnregisterWait(m_WaitHandle);
    m_HandlesValid = false;

    if (OnCompletion != nullptr)
    {
        OnCompletion(GetExitCode());
    }
}

void ProcessWindows::WaitOrTimerCallback() 
{
    m_ProcessExited = true;
}

uint32_t ProcessWindows::GetExitCode() const
{
    return m_ExitCode;
}

bool ProcessWindows::HasExited() const
{
    return m_HasExited;
}

} // namespace Genesis

#endif // TARGET_PLATFORM_WINDOWS