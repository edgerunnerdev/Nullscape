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

#pragma once

#include "system/astronomicalobject/astronomicalobject.hpp"

namespace Nullscape
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
    virtual void CanvasRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset) override;
    virtual void UpdateDebugUI() override;

    Type GetType() const;

    // SignalSource
    virtual float GetSignalDifficulty() const override;
    virtual const std::string& GetSignalType() const override;
    virtual const std::string& GetSignalName() const override;

private:
    void GenerateProperties();
    void GenerateType();

    Type m_Type;
    std::string m_SignalName;
    std::string m_SignalType;
};
GENESIS_DECLARE_SMART_PTR(Star);

} // namespace Nullscape