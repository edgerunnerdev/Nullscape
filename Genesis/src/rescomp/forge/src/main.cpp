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

#include "forge.hpp"

#include <cmdparser.hpp>
#include <log.hpp>

int main(int argc, char** argv)
{
    using namespace Genesis::Core;
    using namespace Genesis::ResComp;

#ifdef _WIN32
    Log::AddLogTarget(std::make_shared<VisualStudioLogger>());
#endif
    Log::AddLogTarget(std::make_shared<TTYLogger>());

    Log::Info("Running Forge...");

    cli::Parser parser(argc, argv);
    parser.set_required<std::string>("m", "mode", "Mode for Forge to run in. Can be 'standalone' or 'service'.");
    parser.set_required<std::string>("a", "assets-dir", "Assets directory, containing all assets to be converted.");
    parser.set_required<std::string>("d", "data-dir", "Data directory, to which compiled resources will be written to.");
    parser.set_required<std::string>("i", "intermediates-dir", "Intermediates directory, containing temporary files used by the build process.");
    parser.run_and_exit_if_error();

    Forge::Mode mode(Forge::Mode::Standalone);
    std::string modeArg = parser.get<std::string>("m");
    if (modeArg == "standalone")
    {
        mode = Forge::Mode::Standalone;
    }
    else if (modeArg == "service")
    {
        mode = Forge::Mode::Service;
    }
    else
    {
        Log::Error("Mode '%s' not recognized, must be 'standalone' or 'service'.", modeArg.c_str());
        return -1;
    }

    std::filesystem::path assetsDir(parser.get<std::string>("a"));
    std::filesystem::path dataDir(parser.get<std::string>("d"));
    std::filesystem::path intermediatesDir(parser.get<std::string>("i"));

    Forge forge(mode, assetsDir, dataDir, intermediatesDir);
    return forge.Run() == true ? 0 : -1;
}
