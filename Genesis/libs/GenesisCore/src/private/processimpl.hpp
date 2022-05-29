#pragma once

#include "process.hpp"

namespace Genesis
{
namespace Core
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

} // namespace Core
} // namespace Genesis