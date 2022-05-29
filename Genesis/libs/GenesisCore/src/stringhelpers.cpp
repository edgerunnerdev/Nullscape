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

#include "stringhelpers.hpp"

#include <algorithm>
#include <codecvt>
#include <locale>
#include <sstream>

namespace Genesis
{
namespace Core
{

std::string ToLower(const std::string& str)
{
    std::string loweredStr(str);
    std::transform(loweredStr.begin(), loweredStr.end(), loweredStr.begin(),
                   [](char c) -> char
                   {
                       return static_cast<char>(std::tolower(c));
                   });
    return loweredStr;
}

int CountLines(const std::string& text)
{
    if (text.empty())
        return 0;

    int lines = 1;
    int l = static_cast<int>(text.length());
    for (int i = 0; i < l; ++i)
    {
        if (text[i] == '\n')
        {

            lines++;
        }
    }
    return lines;
}

bool StringEndsWith(const std::string& text, const std::string& substring)
{
    const size_t textLength = text.length();
    const size_t substringLength = substring.length();
    return (textLength >= substringLength && text.substr(textLength - substringLength) == substring);
}

std::vector<std::string> Split(const std::string& text, char delim)
{
    std::stringstream ss(text);
    std::vector<std::string> elems;
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::wstring ToWString(const std::string& text)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(text);
}

} // namespace Core
} // namespace Genesis
