// Copyright 2016 Pedro Nunes
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

#include <beginexternalheaders.h>
#include <tinyxml2.h>
#include <endexternalheaders.h>

namespace Hyperscape
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
