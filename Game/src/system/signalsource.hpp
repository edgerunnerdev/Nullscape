// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <string>

#include "system/system.fwd.hpp"


namespace Hyperscape
{

class SignalSource
{
public:
    SignalSource(SystemRandomEngine& systemRandomEngine);

    virtual float GetSignalDifficulty() const = 0;
    virtual const std::string& GetSignalType() const = 0;
    virtual const std::string& GetSignalName() const = 0;
    virtual const glm::vec2& GetSignalCoordinates() const = 0;

    const std::string& GetSignalId() const;

private:
    void GenerateSignalId(SystemRandomEngine& systemRandomEngine);

    std::string m_SignalId;
};

inline const std::string& SignalSource::GetSignalId() const { return m_SignalId; }

} // namespace Hyperscape