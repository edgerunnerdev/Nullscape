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

#include "system/wormhole.hpp"

namespace Hyperscape
{

Wormhole::Wormhole(SystemRandomEngine& systemRandomEngine, const glm::vec2& coordinates, int depth)
    : SignalSource(systemRandomEngine)
    , m_Coordinates(coordinates)
    , m_Depth(depth)
    , m_SignalType("Wormhole")
    , m_SignalName("Unstable wormhole")
{

}

float Wormhole::GetSignalDifficulty() const
{
    return 1.0f + static_cast<float>(m_Depth);
}

const std::string& Wormhole::GetSignalType() const 
{
    return m_SignalType;
}
    
const std::string& Wormhole::GetSignalName() const
{
    return m_SignalName;
}
    
const glm::vec2& Wormhole::GetSignalCoordinates() const
{
    return m_Coordinates;
}

} // namespace Hyperscape