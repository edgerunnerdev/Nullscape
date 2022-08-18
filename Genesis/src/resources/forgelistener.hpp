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

#include <memory>

// clang-format off
#include <externalheadersbegin.hpp>
#include <rpc/client.h>
#include <rpc/server.h>
#include <externalheadersend.hpp>
// clang-format on

#include <process.hpp>

namespace Genesis
{

class ForgeListener
{
public:
    ForgeListener();
    ~ForgeListener();

private:
    void SpawnForgeProcess();

    std::unique_ptr<Core::Process> m_pProcess;
    std::unique_ptr<rpc::client> m_pRPCClient;
    std::unique_ptr<rpc::server> m_pRPCServer;
};

} // namespace Genesis
