// Copyright 2016 Pedro Nunes
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

// clang-format off
#include <externalheadersbegin.hpp>
#include <tinyxml2.h>
#include <externalheadersend.hpp>
// clang-format on

namespace Nullscape
{

class Serialisable
{
public:
	Serialisable() {};
	virtual ~Serialisable() {};

	virtual bool Write( tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pRootElement ) = 0;
	virtual bool Read( tinyxml2::XMLElement* pRootElement ) = 0;
	virtual int GetVersion() const = 0;
	virtual void UpgradeFromVersion( int version ) = 0;
};

}
