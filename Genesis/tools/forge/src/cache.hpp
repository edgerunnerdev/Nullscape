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

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace Genesis::ResComp
{

class Asset;

class Cache
{
public:
	Cache(const std::filesystem::path& intermediatesDir);
	~Cache();

	void Add(Asset* pAsset);
	bool NeedsRebuild(Asset* pAsset);

private:
	std::filesystem::path m_CachePath;
	std::unordered_map<std::string, uint64_t> m_Cache;
};

} // namespace Genesis::ResComp