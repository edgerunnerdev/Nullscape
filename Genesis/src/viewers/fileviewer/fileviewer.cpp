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

#include "viewers/fileviewer/fileviewer.hpp"

#include <imgui/imgui.h>

namespace Genesis
{

FileViewer::FileViewer(int width, int height, const std::filesystem::path& root, const std::string& extension)
    : m_Extension(extension)
    , m_Width(width)
    , m_Height(height)
{
    m_pRootNode = std::make_unique<Node>(root);
    ProcessNode(*m_pRootNode);
}

void FileViewer::ProcessNode(Node& node)
{
    if (std::filesystem::is_directory(node.path))
    {
        for (const auto& dirEntry : std::filesystem::directory_iterator(node.path))
        {
            Node childNode(dirEntry.path());
            if (dirEntry.is_directory())
            {
                ProcessNode(childNode);
                node.children.push_back(std::move(childNode));
            }
            else if (dirEntry.is_regular_file() && dirEntry.path().extension() == m_Extension)
            {
                node.children.push_back(std::move(childNode));
            }
        }
    }
}

void FileViewer::Render()
{
    m_Selected.clear();

    if (ImGui::CollapsingHeader("Files", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (const Node& node : m_pRootNode->children)
        {
            RenderNode(node);
        }
    }
}

void FileViewer::RenderNode(const Node& node)
{
    std::string name = node.path.filename().generic_string();
    if (node.children.empty())
    {
        if (ImGui::Selectable(name.c_str()))
        {
            m_Selected = node.path;
        }
    }
    else if (ImGui::TreeNode(name.c_str()))
    {
        for (const Node& childNode : node.children)
        {
            RenderNode(childNode);
        }
        ImGui::TreePop();
    }
}

bool FileViewer::HasSelected() const 
{
    return m_Selected.empty() == false;
}

const std::filesystem::path& FileViewer::GetSelected() const
{
    return m_Selected;
}

} // namespace Genesis