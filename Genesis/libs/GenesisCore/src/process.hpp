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

#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace Genesis
{

class Process;
using ProcessSharedPtr = std::shared_ptr<Process>;
using ProcessOnCompletionCallback = std::function<void(uint32_t)>;
using ProcessOnOutputCallback = std::function<void(const std::string&)>;

class ProcessImpl;

class Process
{
public:
    Process(const std::filesystem::path& executable, const std::string& arguments, ProcessOnCompletionCallback completionCallback = nullptr, ProcessOnOutputCallback outputCallback = nullptr);
    ~Process();

    void Run();
    void Wait();
    uint32_t GetExitCode() const;
    bool HasExited() const;

private:
    std::unique_ptr<ProcessImpl> m_pImpl;
};

} // namespace Genesis