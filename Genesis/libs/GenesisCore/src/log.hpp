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

#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <locale>
#include <memory>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>

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
        Error,

        Count
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

        #ifdef _WIN32
        Stream& operator<<(const std::filesystem::path& value)
        {
            // Cleanup all the slashes and display them in Windows' standard format ('\').
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            m_Collector << std::regex_replace(converter.to_bytes(value), std::regex("(\\\\|/)"), "\\");
            return *this;
        }
        #endif

        Stream& operator<<(const std::wstring& value)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            m_Collector << converter.to_bytes(value);
            return *this;
        }

    private:
        Level m_Level;
        std::ostringstream m_Collector;
    };

    static Stream Info();
    static Stream Warning();
    static Stream Error();

    static void AddLogTarget(LogTargetSharedPtr pLogTarget);
    static void RemoveLogTarget(LogTargetSharedPtr pLogTarget);

private:
    using LogTargetList = std::list<LogTargetSharedPtr>;

    static void LogInternal(const std::string& text, Log::Level level);

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
    virtual void Log(const std::string& text, Log::Level level) = 0;

protected:
    static const std::string& GetPrefix(Log::Level level);
};

//////////////////////////////////////////////////////////////////////////
// TTYLogger
// Prints the message to the console.
//////////////////////////////////////////////////////////////////////////

class TTYLogger : public ILogTarget
{
public:
    virtual void Log(const std::string& text, Log::Level type) override;
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
    virtual void Log(const std::string& text, Log::Level type) override;

private:
    std::ofstream m_File;
};

//////////////////////////////////////////////////////////////////////////
// MessageBoxLogger
// Creates a message box whenever the log message is above LogLevel::Info.
//////////////////////////////////////////////////////////////////////////

class MessageBoxLogger : public ILogTarget
{
public:
    virtual void Log(const std::string& text, Log::Level type) override;
};

//////////////////////////////////////////////////////////////////////////
// VisualStudioLogger
// All the output from the logger goes to the Visual Studio output window.
//////////////////////////////////////////////////////////////////////////

class VisualStudioLogger : public ILogTarget
{
public:
    virtual void Log(const std::string& text, Log::Level type) override;
};

} // namespace Core
} // namespace Genesis
