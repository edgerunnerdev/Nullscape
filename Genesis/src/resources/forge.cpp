// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

#include <resources/forge.hpp>

#include <filesystem>

namespace Genesis
{

Forge::Forge() 
{
	std::filesystem::path forgePath("../../Genesis/bin/Forge.exe");
	if (std::filesystem::exists(forgePath))
    {
        // -m standalone -a C:\Users\pnunes\Documents\GitHub\Hyperscape\Game\bin\assets -d C:\Users\pnunes\Documents\GitHub\Hyperscape\Game\bin\data -c C:\Users\pnunes\Documents\GitHub\Hyperscape\Genesis\bin\Compilers -i C:\Users\pnunes\Documents\GitHub\Hyperscape\Game\bin\intermediates
        //m_pProcess = std::make_unique<Core::Process>(path, arguments.str());
        //process.Run();
    }
}

Forge::~Forge(){}

} // namespace Genesis
