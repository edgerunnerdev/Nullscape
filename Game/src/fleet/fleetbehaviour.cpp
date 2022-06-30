// Copyright 2015 Pedro Nunes
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

#include "fleet/fleetbehaviour.h"

#include "fleet/fleet.h"
#include "globals.h"
#include "hexterminate.h"
#include "misc/randomshuffle.h"
#include "sector/sector.h"
#include "sector/sectorinfo.h"

#include <algorithm>
#include <genesis.h>
#include <list>
#include <log.hpp>
#include <math/misc.h>
#include <string>
#include <vector>

namespace Hyperscape
{

///////////////////////////////////////////////////////////////////////////////
// FleetBehaviour
///////////////////////////////////////////////////////////////////////////////

FleetBehaviour::FleetBehaviour(Fleet* pFleet)
    : m_pFleet(pFleet)
    , m_ClaimsSectors(false)
    , m_AssistsFriendlies(true)
    , m_JoinsTheFray(false)
{
}

bool FleetBehaviour::ProcessTurn()
{
    return true;
}

void FleetBehaviour::NotifyBattleWon()
{

}

void FleetBehaviour::NotifyAssist(SectorInfo* pSectorToAssist)
{

}

SectorInfo* FleetBehaviour::GetSectorToAssist() const
{
    return nullptr;
}

bool FleetBehaviour::CanAttackSector(SectorInfo* pSectorInfo) const
{
    return false;
}

// Find out how many fleets are already in the process of assisting a particular sector
int FleetBehaviour::GetAssistingFleetsCount(SectorInfo* pSectorToAssist) const
{
    int count = 0;
    const FleetList& fleets = m_pFleet->GetFaction()->GetFleets();
    for (auto& pFleet : fleets)
    {
        if (pFleet->GetDestinationSector() == pSectorToAssist)
        {
            count++;
        }
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////
// FleetBehaviourRoaming
///////////////////////////////////////////////////////////////////////////////

FleetBehaviourRoaming::FleetBehaviourRoaming(Fleet* pFleet, const SectorInfo* pBaseSector)
    : FleetBehaviour(pFleet)
    , m_pBaseSector(pBaseSector)
{
    m_ClaimsSectors = false;
    m_AssistsFriendlies = true;
    m_JoinsTheFray = true;

    SDL_assert(m_pBaseSector != nullptr);
}

bool FleetBehaviourRoaming::HasValidBaseSector() const
{
    return false;
}

void FleetBehaviourRoaming::RelocateBaseSector()
{
    // Attempt to relocate to a sector controlled by this fleet's faction.
    const SectorInfoVector& controlledSectors = m_pFleet->GetFaction()->GetControlledSectors();
    if (controlledSectors.empty() == false)
    {
        m_pBaseSector = controlledSectors[rand() % controlledSectors.size()];
    }
    else
    {
        // If that fails (because the faction has been conquered), go wreck some havok in Imperial space.
        const SectorInfoVector& empireControlledSectors = g_pGame->GetFaction(FactionId::Empire)->GetControlledSectors();
        if (empireControlledSectors.empty() == false)
        {
            m_pBaseSector = empireControlledSectors[rand() % empireControlledSectors.size()];
        }
    }
}

bool FleetBehaviourRoaming::TryAttackSector(SectorInfo* pSector)
{
    if (CanAttackSector(pSector))
    {
        int x, y;
        pSector->GetCoordinates(x, y);

        m_pFleet->SetDestination((static_cast<float>(x) + 0.5f) / NumSectorsX, (static_cast<float>(y) + 0.5f) / NumSectorsY);
        return true;
    }
    else
    {
        return false;
    }
}

bool FleetBehaviourRoaming::ProcessTurn()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// FleetBehaviourRaiding
///////////////////////////////////////////////////////////////////////////////

FleetBehaviourRaiding::FleetBehaviourRaiding(Fleet* pFleet)
    : FleetBehaviour(pFleet)
{
    m_ClaimsSectors = true;
    m_AssistsFriendlies = true;
    m_JoinsTheFray = true;
}

bool FleetBehaviourRaiding::ProcessTurn()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// FleetBehaviourExpansionist
///////////////////////////////////////////////////////////////////////////////

FleetBehaviourExpansionist::FleetBehaviourExpansionist(Fleet* pFleet)
    : FleetBehaviour(pFleet)
{
    m_ClaimsSectors = true;
    m_AssistsFriendlies = true;
    m_JoinsTheFray = false;
}

bool FleetBehaviourExpansionist::ProcessTurn()
{
    if (FleetBehaviour::ProcessTurn() == false)
    {
        return false;
    }

    const SectorInfoVector& controlledSectors = m_pFleet->GetFaction()->GetControlledSectors();
    SectorInfoVector hostileBorderingSectors;
    Faction* pFleetFaction = m_pFleet->GetFaction();
    for (auto& sector : controlledSectors)
    {
        sGetHostileBorderingSectors(pFleetFaction, sector, hostileBorderingSectors);
    }

    SectorInfoVector::iterator it = std::unique(hostileBorderingSectors.begin(), hostileBorderingSectors.end());
    hostileBorderingSectors.resize(std::distance(hostileBorderingSectors.begin(), it));
    if (hostileBorderingSectors.size() > 0)
    {
        RandomShuffle::Shuffle(hostileBorderingSectors.begin(), hostileBorderingSectors.end());
        SectorInfo* pDestinationSector = hostileBorderingSectors.front();

        if (CanAttackSector(pDestinationSector))
        {
            int sectorX, sectorY;
            pDestinationSector->GetCoordinates(sectorX, sectorY);
            m_pFleet->SetDestination((static_cast<float>(sectorX) + 0.5f) / NumSectorsX, (static_cast<float>(sectorY) + 0.5f) / NumSectorsY);

#if _DEBUG
            Genesis::Core::Log::Info() << "[" << m_pFleet->GetFaction() << "] Fleet expanding to sector [" << sectorX << "," << sectorY << "]";
#endif
        }
    }

    return true;
}

void FleetBehaviourExpansionist::sGetHostileBorderingSectors(Faction* pFaction, const SectorInfo* pAroundSector, SectorInfoVector& hostileSectors)
{

}

} // namespace Hyperscape