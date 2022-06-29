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

#include "system/astronomicalobject/astronomicalobject.hpp"

namespace Hyperscape
{

class Star : public AstronomicalObject
{
public:
    enum class Type
    {
        MainSequenceStar,
        GiantStar,
        ProtoStar,
        CarbonStar,
        WolfRayetStar,
        BlackHole,
        NeutronStar,
        WhiteDwarf,
        BrownDwarf
    };

    Star(SystemRandomEngine& randomEngine, const glm::vec2& coordinates);
    Star(SystemRandomEngine& randomEngine, OrbitUniquePtr pOrbit, float theta);
    virtual ~Star() override;
    virtual void DebugRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight) override;
    virtual void UpdateDebugUI() override;

    Type GetType() const;

private:
    void GenerateProperties(SystemRandomEngine& randomEngine);
    void GenerateType(SystemRandomEngine& randomEngine);

    Type m_Type;
};
GENESIS_DECLARE_SMART_PTR(Star);

} // namespace Hyperscape