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

#include "system/astronomicalobject/star.hpp"

#include <sstream>

#include <genesis.h>

#include "system/astronomicalobject/orbit.hpp"
#include "ui2.hpp"

namespace Hyperscape
{

Star::Star(SystemRandomEngine& randomEngine, const glm::vec2& coordinates)
    : AstronomicalObject(randomEngine, "Star", coordinates)
    , m_Type(Type::MainSequenceStar)
{
    GenerateProperties();
}

Star::Star(SystemRandomEngine& randomEngine, OrbitUniquePtr pOrbit, float theta)
    : AstronomicalObject(randomEngine, "Star", std::move(pOrbit), theta)
    , m_Type(Type::MainSequenceStar)
{
    GenerateProperties();
}

Star::~Star() {}

void Star::CanvasRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset)
{
    AstronomicalObject::CanvasRender(canvasTopLeft, canvasBottomRight, canvasOffset);

    ImVec2 center = UI2::ToCanvasCoordinates(canvasTopLeft, canvasBottomRight, canvasOffset, GetSignalCoordinates());
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    pDrawList->AddCircleFilled(center, 16.0f, IM_COL32(200, 200, 0, 255));
}

void Star::UpdateDebugUI() 
{
    ImGui::Text("Type: %s", magic_enum::enum_name(m_Type).data());
}

Star::Type Star::GetType() const
{
    return m_Type;
}

float Star::GetSignalDifficulty() const 
{ 
    return 0.0f; 
}

const std::string& Star::GetSignalType() const 
{ 
    return m_SignalType;
}

const std::string& Star::GetSignalName() const 
{
    return m_SignalName;
}

void Star::GenerateProperties()
{
    GenerateType();
}

void Star::GenerateType()
{
    struct StarTypeProbability
    {
        Type type;
        float probability;
    };

    // clang-format off
    static const std::vector<StarTypeProbability> starTypeProbabilities = {
        {Type::MainSequenceStar, 77.00f},
        {Type::GiantStar,         0.25f},
        {Type::ProtoStar,         2.40f}, 
        {Type::CarbonStar,        0.08f},
        {Type::WolfRayetStar,     0.05f},
        {Type::BlackHole,         0.41f},
        {Type::NeutronStar,       4.00f},
        {Type::WhiteDwarf,        0.36f},
        {Type::BrownDwarf,       15.45f}
    };
    // clang-format on

    std::uniform_real_distribution<float> typeDistribution(0.0f, 100.0f);
    const float value = typeDistribution(GetRandomEngine());
    float accumulated = 0.0f;
    for (auto& starTypeProbability : starTypeProbabilities)
    {
        accumulated += starTypeProbability.probability;
        if (value <= accumulated)
        {
            m_Type = starTypeProbability.type;
            break;
        }
    }

    // clang-format off
    static const std::vector<std::string> starTypeNames = {
        "Main sequence star",
        "Giant star",
        "Proto star", 
        "Carbon star",
        "Wolf-Rayet star",
        "Black hole",
        "Neutron star",
        "White dwarf",
        "Brown dwarf"
    };
    // clang-format on

    SDL_assert(starTypeProbabilities.size() == starTypeNames.size());
    m_SignalType = starTypeNames[static_cast<size_t>(m_Type)];

    m_SignalName = "Star";
}

} // namespace Hyperscape