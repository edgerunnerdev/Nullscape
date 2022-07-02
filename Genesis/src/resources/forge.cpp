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

#include <filesystem>
#include <platform.hpp>
#include <resources/forge.hpp>
#include <sstream>
#include <string>

namespace Genesis
{

Forge::Forge()
{
    using namespace std::filesystem;

#ifdef TARGET_PLATFORM_WINDOWS
    path forgePath("../../Genesis/bin/Forge.exe");
#else
    path forgePath("../../Genesis/bin/Forge");
#endif

    if (exists(forgePath))
    {
        m_pRPCClient = std::make_unique<rpc::client>("127.0.0.1", 47563);

        path assetsPath = current_path() / "assets";
        path dataPath = current_path() / "data";
        path intermediatesPath = current_path() / "intermediates";
        path compilersPath = canonical(current_path() / ".." / ".." / "Genesis" / "bin" / "compilers");

        std::stringstream arguments;
        arguments << "-m service -a " << assetsPath.generic_string() << " -d " << dataPath.generic_string() << " -c " << compilersPath.generic_string() << " -i " << intermediatesPath.generic_string();

        m_pProcess = std::make_unique<Core::Process>(forgePath, arguments.str());
        m_pProcess->Run();
    }
}

Forge::~Forge() 
{
    m_pRPCClient->send("quit");
}

} // namespace Genesis
