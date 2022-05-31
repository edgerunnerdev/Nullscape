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

#include <string>
#include <vector>

namespace Genesis
{
namespace Core
{

std::string ToLower(const std::string& text);
int CountLines(const std::string& text);
std::vector<std::string> Split(const std::string& text, char delim);
std::wstring ToWString(const std::string& text);

bool StringStartsWith(const std::string& text, const std::string& startsWith);
bool StringStartsWith(const std::wstring& text, const std::wstring& startsWith);
bool StringEndsWith(const std::string& text, const std::string& endsWith);
bool StringEndsWith(const std::wstring& text, const std::wstring& endsWith);

} // namespace Core
} // namespace Genesis
