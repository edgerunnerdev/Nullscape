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

#include "viewers/modelviewer/modelviewerobject.hpp"

#include "resourcemanager.h"
#include "genesis.h"
#include "rendersystem.h"

namespace Genesis
{

ModelViewerObject::ModelViewerObject(const std::filesystem::path& path)
{
    m_pModel = FrameWork::GetResourceManager()->GetResource<ResourceModel*>(path.generic_string());
}

ModelViewerObject::~ModelViewerObject()
{
}

void ModelViewerObject::Update(float delta)
{
    SceneObject::Update(delta);
}

void ModelViewerObject::Render()
{
    if (m_pModel != nullptr)
    {
        m_pModel->Render(glm::mat4());
    }
}

void ModelViewerObject::DebugRender(Render::DebugRender* pDebugRender, ResourceModel::DebugRenderFlags flags) 
{
    if (m_pModel != nullptr)
    {
        m_pModel->DebugRender(pDebugRender, flags);
    }
}

size_t ModelViewerObject::GetVertexCount() const 
{ 
    return m_pModel ? m_pModel->GetVertexCount() : 0; 
}

size_t ModelViewerObject::GetTriangleCount() const 
{
    return m_pModel ? m_pModel->GetTriangleCount() : 0; 
}

} // namespace Genesis