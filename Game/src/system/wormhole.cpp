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

#include <random>

#include "system/wormhole.hpp"

#include "ui2.hpp"

namespace Nullscape
{

Wormhole::Wormhole(SystemRandomEngine& systemRandomEngine, const glm::vec2& coordinates, int depth)
    : SignalSource(systemRandomEngine)
    , m_Coordinates(coordinates)
    , m_Depth(depth)
    , m_SignalType("Wormhole")
    , m_SignalName("Unstable wormhole")
{
    Wavelength onenm(1.0_pm);
    std::normal_distribution<float> wavelengthDist(5.0f, 2.0f);
    std::uniform_real_distribution<float> radiosityDist(1.0f, 2.0f);
    for (int i = 0; i < 18; ++i)
    {
        double w = wavelengthDist(m_LocalRandomEngine) * onenm.ToDouble();
        float r = radiosityDist(m_LocalRandomEngine);
        m_SignalData.Add(w, r);
    }
}

void Wormhole::CanvasRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset)
{
    ImVec2 center = UI2::ToCanvasCoordinates(canvasTopLeft, canvasBottomRight, canvasOffset, GetSignalCoordinates());
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    pDrawList->AddCircleFilled(center, 12.0f, IM_COL32(200, 0, 200, 255));
}

float Wormhole::GetSignalDifficulty() const
{
    return static_cast<float>(m_Depth) * 1000.0f;
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

} // namespace Nullscape