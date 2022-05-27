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

#include <algorithm>

#ifdef _WIN32
#include "windows.h"
#endif

#include "log.hpp"

#include <SDL.h>

namespace Genesis
{
namespace Core
{

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

std::mutex Log::m_Mutex;
Log::LogTargetList Log::m_Targets;

void Log::AddLogTarget(LogTargetSharedPtr pLogTarget)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Targets.push_back(pLogTarget);
}

void Log::RemoveLogTarget(LogTargetSharedPtr pLogTarget)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Targets.remove(pLogTarget);
}

Log::Stream Log::Info()
{
    return Log::Stream(Log::Level::Info);
}

Log::Stream Log::Warning()
{
    return Log::Stream(Log::Level::Warning);
}

Log::Stream Log::Error()
{
    return Log::Stream(Log::Level::Error);
}

// Internal logging function. Should only be called by Log::Stream's destructor.
void Log::LogInternal(const std::string& text, Log::Level level)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::string prefix;
    if (level == Log::Level::Info)
    {

        prefix = "[INFO] ";
    }
    else if (level == Log::Level::Warning)
    {

        prefix = "[WARNING] ";
    }
    else if (level == Log::Level::Error)
    {

        prefix = "[ERROR] ";
    }

    std::ostringstream msg;
    msg << prefix << text;
    for (auto& pTarget : m_Targets)
    {
        pTarget->Log(msg.str(), level);
    }
}

//////////////////////////////////////////////////////////////////////////
// Log::Stream
//////////////////////////////////////////////////////////////////////////

Log::Stream::Stream(Log::Level level)
{
    m_Level = level;
}

Log::Stream::~Stream()
{
    Log::LogInternal(m_Collector.str(), m_Level);
}

//////////////////////////////////////////////////////////////////////////
// TTYLogger
// Prints the message to the console.
//////////////////////////////////////////////////////////////////////////

void TTYLogger::Log(const std::string& text, Log::Level type)
{
    std::cout << text << std::endl;
}

//////////////////////////////////////////////////////////////////////////
// FileLogger
//////////////////////////////////////////////////////////////////////////

FileLogger::FileLogger(const std::filesystem::path& filePath)
{
    m_File.open(filePath, std::fstream::out | std::fstream::trunc);
}

FileLogger::~FileLogger()
{
    if (m_File.is_open())
    {
        m_File.close();
    }
}

void FileLogger::Log(const std::string& text, Log::Level type)
{
    if (m_File.is_open())
    {
        m_File << text << std::endl;
        m_File.flush();
    }
}

//////////////////////////////////////////////////////////////////////////
// MessageBoxLogger
//////////////////////////////////////////////////////////////////////////

void MessageBoxLogger::Log(const std::string& text, Log::Level type)
{
    if (type == Log::Level::Warning)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", text.c_str(), nullptr);
    }
    else if (type == Log::Level::Error)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", text.c_str(), nullptr);
    }
}

//////////////////////////////////////////////////////////////////////////
// VisualStudioLogger
//////////////////////////////////////////////////////////////////////////

void VisualStudioLogger::Log(const std::string& text, Log::Level type)
{
#if _MSC_VER
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wideText = converter.from_bytes(text);
    std::wostringstream ss;
    ss << wideText << std::endl;
    OutputDebugStringW(ss.str().c_str());
#endif
}

} // namespace Core
} // namespace Genesis
