// Copyright 2014 Pedro Nunes
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

#include <algorithm>
#include <filesystem>
#include <string>

#include <coredefines.h>
#include <genesis.h>

#include "hexterminate.h"

namespace Hyperscape
{

enum class SaveGameHeaderError
{
	Uninitialised,
	NoError,
	ContentError,		// malformed XML file or failed to load all the needed content
	InvalidExtension,
	FileDoesntExist,
	NoReadPermission,
	Unknown				// shouldn't really happen, in case a system call returns an unexpected value
};

class SaveGameHeader
{
public:
								SaveGameHeader( const std::filesystem::path& filename );

	bool						Read( tinyxml2::XMLDocument& xmlDoc );
	inline bool					IsValid() const				{ return m_Error == SaveGameHeaderError::NoError; }
	inline SaveGameHeaderError	GetError() const			{ return m_Error; }

	inline const std::filesystem::path& GetFilename() const { return m_Filename; }
	inline const std::string&	GetCaptainName() const		{ return m_CaptainName; }
	inline const std::string&	GetShipName() const			{ return m_ShipName; }
	inline float				GetPlayedTime() const		{ return m_PlayedTime; }
	inline bool					IsAlive() const				{ return m_Alive; }

private:
	std::filesystem::path		m_Filename;
	std::string					m_CaptainName;
	std::string					m_ShipName;
	float						m_PlayedTime;
	bool						m_Alive;

	SaveGameHeaderError			m_Error;
};

GENESIS_DECLARE_SMART_PTR( SaveGameHeader );

}
