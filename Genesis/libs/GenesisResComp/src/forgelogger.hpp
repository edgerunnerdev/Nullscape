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

// clang-format off
#include <externalheadersbegin.hpp>
#include <rpc/client.h>
#include <externalheadersend.hpp>
// clang-format on

#include <filesystem>
#include <log.hpp>
#include <memory>

namespace Genesis
{

namespace ResComp
{

class ForgeLogger : public Core::ILogTarget
{
public:
    ForgeLogger(rpc::client* pRPCClient);
    virtual void Log(const std::string& text, Core::Log::Level type) override;

private:
    rpc::client* m_pRPCClient;
};

} // namespace ResComp
} // namespace Genesis
