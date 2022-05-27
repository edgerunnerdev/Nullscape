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
void Log::LogInternal(const std::wstring& text, Log::Level level)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::wstring prefix;
    if (level == Log::Level::Info)
    {

        prefix = L"[INFO] ";
    }
    else if (level == Log::Level::Warning)
    {

        prefix = L"[WARNING] ";
    }
    else if (level == Log::Level::Error)
    {

        prefix = L"[ERROR] ";
    }

    std::wostringstream msg;
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

void TTYLogger::Log(const std::wstring& text, Log::Level type)
{
    std::wcout << text << std::endl;
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

void FileLogger::Log(const std::wstring& text, Log::Level type)
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

void MessageBoxLogger::Log(const std::wstring& text, Log::Level type)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string convertedText = converter.to_bytes(text);

    if (type == Log::Level::Warning)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", convertedText.c_str(), nullptr);
    }
    else if (type == Log::Level::Error)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", convertedText.c_str(), nullptr);
    }
}

//////////////////////////////////////////////////////////////////////////
// VisualStudioLogger
//////////////////////////////////////////////////////////////////////////

void VisualStudioLogger::Log(const std::wstring& text, Log::Level type)
{
#if _MSC_VER
    std::wostringstream ss;
    ss << text << std::endl;
    OutputDebugStringW(ss.str().c_str());
#endif
}

} // namespace Core
} // namespace Genesis
