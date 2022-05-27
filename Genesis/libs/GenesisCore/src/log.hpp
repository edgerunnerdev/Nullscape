/*
MIT License

Copyright (c) 2022 Pedro Nunes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <locale>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace Genesis
{
namespace Core
{

class ILogTarget;

//////////////////////////////////////////////////////////////////////////
// Log
// Contains any number of ILogTargets, which are responsible for actually
// logging the message in various ways.
// This class is thread safe.
//////////////////////////////////////////////////////////////////////////

using LogTargetSharedPtr = std::shared_ptr<ILogTarget>;

class Log
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

    class Stream
    {
    public:
        Stream(Level level);
        ~Stream();

        template <typename T> Stream& operator<<(T const& value)
        {
            m_Collector << value;
            return *this;
        }

        Stream& operator<<(std::string& value)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            m_Collector << converter.from_bytes(value);
            return *this;
        }

        Stream& operator<<(const char* value)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            m_Collector << converter.from_bytes(value);
            return *this;
        }

        Stream& operator<<(const std::filesystem::path& value)
        {
            m_Collector << value.wstring();
            return *this;
        }

    private:
        Level m_Level;
        std::wostringstream m_Collector;
    };

    static Stream Info();
    static Stream Warning();
    static Stream Error();

    static void AddLogTarget(LogTargetSharedPtr pLogTarget);
    static void RemoveLogTarget(LogTargetSharedPtr pLogTarget);

private:
    using LogTargetList = std::list<LogTargetSharedPtr>;

    static void LogInternal(const std::wstring& text, Log::Level level);

    static std::mutex m_Mutex;
    static LogTargetList m_Targets;
};

//////////////////////////////////////////////////////////////////////////
// ILogTarget
// Any ILogTarget must implement Log().
//////////////////////////////////////////////////////////////////////////

class ILogTarget
{
public:
    virtual ~ILogTarget() {}
    virtual void Log(const std::wstring& text, Log::Level level) = 0;
};

//////////////////////////////////////////////////////////////////////////
// TTYLogger
// Prints the message to the console.
//////////////////////////////////////////////////////////////////////////

class TTYLogger : public ILogTarget
{
public:
    virtual void Log(const std::wstring& text, Log::Level type) override;
};

//////////////////////////////////////////////////////////////////////////
// FileLogger
// Dumps the logging into file given in "filename". It is flushed
// after every entry.
//////////////////////////////////////////////////////////////////////////

 class FileLogger : public ILogTarget
{
 public:
    FileLogger(const std::filesystem::path& filePath);
    virtual ~FileLogger() override;
    virtual void Log(const std::wstring& text, Log::Level type) override;

 private:
    std::wofstream m_File;
};

//////////////////////////////////////////////////////////////////////////
// MessageBoxLogger
// Creates a message box whenever the log message is above LogLevel::Info.
//////////////////////////////////////////////////////////////////////////

 class MessageBoxLogger : public ILogTarget
{
 public:
    virtual void Log(const std::wstring& text, Log::Level type) override;
};

//////////////////////////////////////////////////////////////////////////
// VisualStudioLogger
// All the output from the logger goes to the Visual Studio output window.
//////////////////////////////////////////////////////////////////////////

class VisualStudioLogger : public ILogTarget
{
public:
    virtual void Log(const std::wstring& text, Log::Level type) override;
};

} // namespace Core
} // namespace Genesis
