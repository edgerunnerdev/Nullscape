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

class Planet : public AstronomicalObject
{
public:
    Planet(SystemRandomEngine& randomEngine, const glm::vec2& coordinates);
    Planet(SystemRandomEngine& randomEngine, OrbitUniquePtr pOrbit, float theta);
    virtual ~Planet() override;
    virtual void DebugRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset) override;
    virtual void UpdateDebugUI() override;

    // SignalSource
    virtual float GetSignalDifficulty() const override;
    virtual const std::string& GetSignalType() const override;
    virtual const std::string& GetSignalName() const override;

private:
    std::string m_SignalType;
    std::string m_SignalName;
};
GENESIS_DECLARE_SMART_PTR(Planet);

} // namespace Hyperscape