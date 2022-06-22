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

#include "filehash.hpp"

#include <fstream>
#include <vector>
#include <xxhash64.h>

namespace Genesis::ResComp
{

uint64_t CalculateFileHash(const std::filesystem::path& path)
{
    uint64_t hash = 0;
    std::ifstream file(path, std::ifstream::binary);
    if (file.good())
    {
        file.seekg(0, file.end);
        size_t length = static_cast<size_t>(file.tellg());
        file.seekg(0, file.beg);
        std::vector<char> buffer;
        buffer.reserve(length);
        file.read(buffer.data(), length);
        file.close();

        hash = XXHash64::hash(buffer.data(), length, 0);
    }
    return hash;
}

} // namespace Genesis::ResComp
