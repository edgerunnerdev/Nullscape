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

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Genesis
{

class FileViewer
{
public:
    FileViewer(int width, int height, const std::string& extension);
    void Render();
    bool HasSelected() const;
    const std::filesystem::path& GetSelected() const;

private:
    struct Node
    {
        Node(const std::filesystem::path& p) : path(p) {}
        std::filesystem::path path;
        std::vector<Node> children;
    };

    void ProcessNode(Node& node);
    void RenderNode(const Node& node);

    std::unique_ptr<Node> m_pRootNode;
    int m_Width;
    int m_Height;
    std::string m_Extension;
    std::filesystem::path m_Selected;
};

} // namespace Genesis