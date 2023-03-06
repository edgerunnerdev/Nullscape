// Copyright 2023 Pedro Nunes
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

#include <scene/sceneobject.h>

#include "sector/battlecontrollerevents.hpp"

#include <coredefines.h>
#include <imgui/imgui.h>

#include <list>
#include <string>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR( BattleEvent );
GENESIS_DECLARE_SMART_PTR( Fleet );

class BattleController : public Genesis::SceneObject
{
public:
    BattleController( const FleetSharedPtr& pPlayerFleet, const FleetSharedPtr& pOtherFleet );
    virtual ~BattleController();
    virtual void Update( float delta ) override;
    virtual void Render( const Genesis::SceneCameraSharedPtr& pCamera ) override;
    void AddBattleEvent( const BattleEventUniquePtr& pBattleEvent );

private:
    void UpdateDebugUI();
    void UpdateFleetDebugUI( FleetSharedPtr& pFleet, const std::string& title );
    void UpdateBattleEventsDebugUI();

    FleetSharedPtr m_pPlayerFleet;
    FleetSharedPtr m_pOtherFleet;
    std::list<BattleEventUniquePtr> m_Events;
    bool m_DebugUIOpen;
    ImVec4 m_DebugUITitleColor;
};
GENESIS_DECLARE_SMART_PTR( BattleController );

} // namespace Hyperscape
