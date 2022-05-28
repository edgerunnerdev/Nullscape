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

#include "stringhelpers.hpp"

#include <algorithm>
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

} // namespace Core
} // namespace Genesis
