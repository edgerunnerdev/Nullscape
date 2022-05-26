/*
MIT License

Copyright (c) 2022 Pedro Nunes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "rescomp.hpp"

#include <SDL.h>
#include <cmdparser.hpp>

namespace Genesis
{
namespace ResComp
{

ResComp::ResComp() {}

ResComp::~ResComp() {}

void ResComp::Initialise(int argc, char** argv)
{
    cli::Parser parser(argc, argv);
    parser.set_required<std::string>("a", "assets-dir", "Assets directory, containing all assets to be converted.");
    parser.set_required<std::string>("d", "data-dir", "Data directory, to which compiled resources will be written to.");
    parser.set_required<std::string>("i", "intermediates-dir", "Intermediates directory, containing temporary files used by the build process.");
    parser.set_required<std::string>("f", "file", "File to compile.");
    parser.run_and_exit_if_error();
}

int ResComp::Run()
{
    return 0;
}

const std::filesystem::path& ResComp::GetAssetsDir() const
{
    return m_AssetsDir;
}
const std::filesystem::path& ResComp::GetDataDir() const
{
    return m_DataDir;
}
const std::filesystem::path& ResComp::GetIntermediatesDir() const
{
    return m_IntermediatesDir;
}
const std::filesystem::path& ResComp::GetFile() const
{
    return m_File;
}

} // namespace ResComp
} // namespace Genesis
