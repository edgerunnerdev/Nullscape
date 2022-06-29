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

#include "system/astronomicalobject/orbit.hpp"
#include <genesis.h>

namespace Hyperscape
{

Star::Star(SystemRandomEngine& randomEngine, const glm::vec2& coordinates)
    : AstronomicalObject(randomEngine, "Star", coordinates)
    , m_Type(Type::MainSequenceStar)
{
    GenerateProperties(randomEngine);
}

Star::Star(SystemRandomEngine& randomEngine, OrbitUniquePtr pOrbit, float theta)
    : AstronomicalObject(randomEngine, "Star", std::move(pOrbit), theta)
    , m_Type(Type::MainSequenceStar)
{
    GenerateProperties(randomEngine);
}

Star::~Star() {}

void Star::DebugRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight)
{
    AstronomicalObject::DebugRender(canvasTopLeft, canvasBottomRight);

    ImVec2 size(canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y);
    glm::vec2 normalizedCoordinates = GetCoordinates() / 2.0f + glm::vec2(0.5f);
    ImVec2 center(canvasTopLeft.x + size.x * normalizedCoordinates.x, canvasTopLeft.y + size.y * normalizedCoordinates.y);

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

void Star::GenerateProperties(SystemRandomEngine& randomEngine)
{
    GenerateType(randomEngine);
}

void Star::GenerateType(SystemRandomEngine& randomEngine)
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
    const float value = typeDistribution(randomEngine);
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
}

} // namespace Hyperscape