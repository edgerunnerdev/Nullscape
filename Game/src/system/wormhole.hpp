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

#include "system/signalsource.hpp"


namespace Nullscape
{

class Wormhole :  public SignalSource
{
public:
    Wormhole(SystemRandomEngine& systemRandomEngine, const glm::vec2& coordinates, int depth);

    virtual void CanvasRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset) override;
    virtual float GetSignalDifficulty() const override;
    virtual const std::string& GetSignalType() const override;
    virtual const std::string& GetSignalName() const override;
    virtual const glm::vec2& GetSignalCoordinates() const override;

private:
    glm::vec2 m_Coordinates;
    int m_Depth;
    std::string m_SignalType;
    std::string m_SignalName;
};

} // namespace Nullscape