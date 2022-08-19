// Copyright 2022 Pedro Nunes
//
// This file is part of Nullscape.
//
// Nullscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Nullscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nullscape. If not, see <http://www.gnu.org/licenses/>.

#include "system/signalsource.hpp"

#include <sstream>

namespace Nullscape
{

SignalSource::SignalSource(SystemRandomEngine& systemRandomEngine)
    : m_LocalRandomEngine(systemRandomEngine)
{
    GenerateSignalId();
}

void SignalSource::GenerateSignalId()
{
    auto fnLetter = [this]() -> char
    {
        std::uniform_int_distribution<int> distLetters(0, 26);
        return ('A' + distLetters(m_LocalRandomEngine));
    };

    auto fnNumber = [this]() -> char
    {
        std::uniform_int_distribution<int> distNumbers(0, 9);
        return ('0' + distNumbers(m_LocalRandomEngine));
    };

    std::stringstream id;
    id << fnLetter() << fnLetter() << "-" << fnNumber() << fnNumber() << fnNumber();
    m_SignalId = id.str();
}

} // namespace Nullscape