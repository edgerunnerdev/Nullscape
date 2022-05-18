// Copyright 2021 Pedro Nunes
//
// This file is part of Hexterminate.
//
// Hexterminate is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hexterminate is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hexterminate. If not, see <http://www.gnu.org/licenses/>.

#include <filesystem>
#include <sstream>

#include <gui/gui.h>
#include <imgui/imgui.h>
#include <configuration.h>
#include <logger.h>
#include <genesis.h>

#include "menus/popup.h"
#include "ui/design.h"
#include "ui/element.h"
#include "ui/rootelement.h"
#include "hexterminate.h"

namespace Hexterminate::UI
{

static const char* sElementPropertyPosition = "position";
static const char* sElementPropertySize = "size";


Element::Element( const std::string& name ) :
m_Name( name ),
m_Highlighted( false ),
m_FullWidth( false ),
m_FullHeight( false ),
m_IsPopupElement( false ),
m_IsEditable( true ),
m_IsDynamic( false )
{
    m_pPanel = new Genesis::Gui::Panel();
    m_pPanel->SetSize( 128.0f, 128.0f );
    m_pPanel->SetPosition( 0.0f, 0.0f );
    m_pPanel->SetColour( 0.0f, 0.0f, 0.0f, 0.0f );
    m_pPanel->SetBorderColour( 0.0f, 0.0f, 0.0f, 0.0f );
}

Element::~Element()
{

}

void Element::Update()
{
    for (auto& pChild : m_Children)
    {
        pChild->Update();
    }
}

void Element::Add( ElementSharedPtr pElement )
{
#ifdef _DEBUG
    for ( auto& pChild : m_Children )
    {
        if ( pChild->GetName() == pElement->GetName() )
        {
            Genesis::FrameWork::GetLogger()->LogWarning( "UI Element '%s' already contains a child called '%s'.", pElement->GetName().c_str() );
            return;
        }
    }
#endif

    m_Children.push_back( pElement );
    m_pPanel->AddElement( pElement->GetPanel() );
    pElement->ResolvePath( this );
    pElement->LoadFromDesign();
}

bool Element::IsHovered() const
{
    return GetPanel()->IsMouseInside();
}

void Element::Show( bool state )
{
    GetPanel()->Show( state );
}

void Element::LoadFromDesign()
{
    if ( IsPathResolved() == false || IsDynamic() )
    {
        return;
    }

    Design* pDesign = g_pGame->GetUIRoot()->GetDesign();
    const json& data = pDesign->Get( GetPath() );
    if ( data.is_object() )
    {
        if ( data.contains( "properties" ) )
        {
            const json& properties = data[ "properties" ];
            if ( properties.is_object() )
            {
                LoadProperties( properties );
            }
        }

        for ( auto& pChildElement : m_Children )
        {
            pChildElement->LoadFromDesign();
        }
    }
}

void Element::ResolvePath( Element* pParentElement )
{
    std::stringstream path;

    if ( pParentElement == g_pGame->GetUIRoot() )
    {
        path << "/" << GetName();
    }
    else if ( pParentElement->IsPathResolved() )
    {
        path << pParentElement->GetPath() << "/" << GetName();
    }
    else
    {
        return;
    }

    bool pathChanged = ( m_Path != path.str() );
    if ( pathChanged )
    {
        m_Path = path.str();

        for ( auto& pChildElement : m_Children )
        {
            pChildElement->ResolvePath( this );
        }
    }
}

void Element::SaveInternal( json& data, bool saveProperties /* = true */ )
{
    if ( IsDynamic() )
    {
        return;
    }

    if ( saveProperties )
    {
        SaveProperties( data[ "properties" ] );
    }

    if ( GetChildren().size() > 0 )
    {
        for ( auto& pChild : GetChildren() )
        {
            json& childData = data[pChild->GetName()];
            pChild->SaveInternal( childData );
        }
    }
}

void Element::SaveProperties( json& properties )
{
    glm::vec2 pos = m_pPanel->GetPosition();
    properties[ sElementPropertyPosition ] = {
        { "x", pos.x },
        { "y", pos.y }
    };

    glm::vec2 size = m_pPanel->GetSize();
    properties[ sElementPropertySize ] = {
        { "w", m_FullWidth ? -1.0f : size.x },
        { "h", m_FullHeight ? -1.0f : size.y }
    };
}

void Element::LoadProperties( const json& properties )
{
    const json& position = properties[ sElementPropertyPosition ];
    SetPosition( position[ "x" ].get<int>(), position[ "y" ].get<int>() );

    const json& size = properties[ sElementPropertySize ];
    int w = size[ "w" ].get<int>();
    int h = size[ "h" ].get<int>();
    if ( w < 0.0f )
    {
        w = static_cast<int>( Genesis::Configuration::GetScreenWidth() );
    }
    if ( h < 0.0f )
    {
        h = static_cast<int>( Genesis::Configuration::GetScreenHeight() );
    }
    SetSize( w, h );
}

void Element::RenderProperties()
{
    if ( ImGui::Checkbox( "Highlighted", &m_Highlighted ) )
    {
        m_pPanel->SetHighlighted( m_Highlighted );
    }

    ImGui::SameLine();

    bool visible = m_pPanel->IsVisible();
    if ( ImGui::Checkbox( "Visible", &visible ) )
    {
        m_pPanel->Show( visible );
    }

    if ( IsResizeable() )
    {
        ImGui::SameLine();
        ImGui::Checkbox( "Full width", &m_FullWidth );
        ImGui::SameLine();
        ImGui::Checkbox( "Full height", &m_FullHeight );
    }

    glm::vec2 gpos = m_pPanel->GetPosition();
    int ipos[2] = { static_cast<int>( gpos.x ), static_cast<int>( gpos.y ) };
    ImGui::InputInt2( "Position", ipos );
    SetPosition( ipos[0], ipos[1] );

    if ( IsResizeable() )
    {
        glm::vec2 gsize = m_pPanel->GetSize();
        int isize[2] = { static_cast<int>( gsize.x ), static_cast<int>( gsize.y ) };

        if ( m_FullWidth == false && m_FullHeight == false )
        {
            ImGui::InputInt2( "Size", isize );
        }
        else
        {
            if ( m_FullWidth )
            {
                isize[0] = static_cast<int>( Genesis::Configuration::GetScreenWidth() );
                ImGui::InputInt( "Height", &isize[1] );
            }
            if ( m_FullHeight )
            {
                isize[1] = static_cast<int>( Genesis::Configuration::GetScreenHeight() );
                ImGui::InputInt( "Width", &isize[0] );
            }
        }

        SetSize( isize[0], isize[1] );
    }
}

void Element::SetSize( int width, int height )
{
    m_pPanel->SetSize( width, height );
}

void Element::SetPosition( int x, int y )
{
    m_pPanel->SetPosition( x, y );
}

int Element::GetWidth() const
{
    return m_pPanel->GetWidth();
}

int Element::GetHeight() const
{
    return m_pPanel->GetHeight();
}

void Element::GetPosition( int& x, int& y )
{
    const glm::vec2 pos = m_pPanel->GetPosition();
    x = static_cast<int>( pos.x );
    y = static_cast<int>( pos.y );
}

bool Element::IsAcceptingInput() const
{
    if ( IsPopupElement() )
    {
        return true;
    }
    else
    {
        return g_pGame->GetPopup()->IsActive() == false && GetPanel()->IsVisible();
    }
}

bool Element::IsVisible() const
{
    return m_pPanel->IsVisible();
}

} // namespace Hexterminate::UI
