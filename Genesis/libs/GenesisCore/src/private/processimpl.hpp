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

#include "process.hpp"

namespace Genesis
{

class ProcessImpl
{
public:
    ProcessImpl(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback = nullptr, ProcessOnOutputCallback outputCallback = nullptr) 
    {
        m_Executable = executable;
        m_Arguments = arguments;
        OnCompletion = completionCallback;
        OnOutput = outputCallback;
    }

    virtual ~ProcessImpl() {}

    virtual void Run() = 0;
    virtual void Wait() = 0;
    virtual uint32_t GetExitCode() const = 0;
    virtual bool HasExited() const = 0;

    ProcessOnCompletionCallback OnCompletion;
    ProcessOnOutputCallback OnOutput;

    const std::filesystem::path& GetExecutable() const { return m_Executable; }
    const std::string& GetArguments() const { return m_Arguments; }

private:
    std::filesystem::path m_Executable;
    std::string m_Arguments;
};

} // namespace Genesis