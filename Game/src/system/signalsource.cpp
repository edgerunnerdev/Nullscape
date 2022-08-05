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

#include <sstream>

#include "system/signalsource.hpp"

namespace Hyperscape
{

SignalSource::SignalSource(SystemRandomEngine& systemRandomEngine)
{
	GenerateSignalId(systemRandomEngine);
}

void SignalSource::GenerateSignalId(SystemRandomEngine& systemRandomEngine)
{
	LocalRandomEngine localRandomEngine(systemRandomEngine());

	auto fnLetter = [&localRandomEngine]() -> char { 
		std::uniform_int_distribution<int> distLetters(0, 26);
		return ('A' + distLetters(localRandomEngine)); 
	};

	auto fnNumber = [&localRandomEngine]() -> char {
        std::uniform_int_distribution<int> distNumbers(0, 9);
        return ('0' + distNumbers(localRandomEngine));
    };

	std::stringstream id;
	id << fnLetter() << fnLetter() << "-" << fnNumber() << fnNumber() << fnNumber();
	m_SignalId = id.str();
}

ImVec2 SignalSource::ToCanvasCoordinates(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset, const glm::vec2& coordinates) const
{
    static const float sZoom = 0.5f;
    const ImVec2 size(canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y);
    return ImVec2(canvasTopLeft.x + canvasOffset.x + size.x * coordinates.x * sZoom, canvasTopLeft.y + canvasOffset.y + size.y * coordinates.y * sZoom);
}

} // namespace Hyperscape