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

#include <coredefines.h>

#include <chrono>

namespace Hyperscape
{

GENESIS_DECLARE_SMART_PTR( Entity );

class BattleEvent
{
public:
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

    virtual ~BattleEvent() {}
    virtual void OnExpiry() = 0;

protected:
    BattleEvent( const EntitySharedPtr& pFromEntity, const EntitySharedPtr& pToEntity, std::chrono::milliseconds expiry );

private:
    EntitySharedPtr m_pFrom;
    EntitySharedPtr m_pTo;
    TimePoint m_Expiry;
};

class FireProjectileBattleEvent : public BattleEvent
{
public:
    FireProjectileBattleEvent( const EntitySharedPtr& pFromEntity, const EntitySharedPtr& pToEntity, std::chrono::milliseconds expiry );
    virtual void OnExpiry() override;
};

class IncomingProjectileBattleEvent : public BattleEvent
{
public:
    IncomingProjectileBattleEvent( const EntitySharedPtr& pFromEntity, const EntitySharedPtr& pToEntity, std::chrono::milliseconds expiry );
    virtual void OnExpiry() override;
};

} // namespace Hyperscape
