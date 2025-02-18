// Copyright 2021 Pedro Nunes
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

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <list>
#include <memory>
#include <string>

#include "ui/types.fwd.h"

using json = nlohmann::json;

namespace Genesis::Gui 
{
class Panel;
}

namespace Hyperscape::UI
{

using ElementList = std::list<ElementSharedPtr>;

class Element
{
public:
    Element( const std::string& name );
    virtual ~Element();

    virtual void Update();
    virtual void RenderProperties();

    virtual bool IsResizeable() const;
    virtual void SetSize( int width, int height );
    virtual void SetPosition( int x, int y );
    int GetWidth() const;
    int GetHeight() const;
    void GetPosition( int& x, int& y );
    bool IsHovered() const;
    bool IsAcceptingInput() const;
    bool IsPopupElement() const;
    void SetPopupElement( bool isPopupElement );
    bool IsEditable() const;
    void SetEditable( bool isEditable );
    bool IsDynamic() const;
    void SetDynamic( bool isDynamic );
    bool IsVisible() const;

    void Add( ElementSharedPtr pElement );
    const ElementList& GetChildren() const;
    const std::string& GetName() const;
    void Show( bool state );

    // This shouldn't be exposed, but it is necessary until all the 
    // Genesis::Gui elements have been ported to the new system.
    Genesis::Gui::Panel* GetPanel() const;

protected:
    virtual void SaveProperties( json& properties );
    virtual void LoadProperties( const json& properties );

    const std::string& GetPath() const;
    bool IsPathResolved() const;
    void ResolvePath( Element* pParentElement );
    void LoadFromDesign();
    void SaveInternal( json& data, bool saveProperties = true );

private:
    std::string m_Name;
    std::string m_Path;
    Genesis::Gui::Panel* m_pPanel;
    ElementList m_Children;

    bool m_Highlighted;
    bool m_FullWidth;
    bool m_FullHeight;
    bool m_IsPopupElement;
    bool m_IsEditable;
    bool m_IsDynamic;
};

inline bool Element::IsResizeable() const 
{
    return true;
}

inline const std::string& Element::GetName() const
{
    return m_Name;
}

inline const std::string& Element::GetPath() const
{
    return m_Path;
}

inline bool Element::IsPathResolved() const
{
    return m_Path.empty() == false;
}

inline Genesis::Gui::Panel* Element::GetPanel() const
{
    return m_pPanel;
}

inline const ElementList& Element::GetChildren() const
{
    return m_Children;
}

inline bool Element::IsPopupElement() const
{
    return m_IsPopupElement;
}

inline void Element::SetPopupElement( bool isPopupElement )
{
    m_IsPopupElement = isPopupElement;
}

inline bool Element::IsEditable() const
{
    return m_IsEditable;
}

inline void Element::SetEditable( bool state )
{
    m_IsEditable = state;
}

inline bool Element::IsDynamic() const
{
    return m_IsDynamic;
}

inline void Element::SetDynamic( bool state )
{
    m_IsDynamic = state;
}

} // namespace Hyperscape::UI
