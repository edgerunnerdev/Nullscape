// Copyright 2014 Pedro Nunes
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

#include <string>
#include <vector>

#include <gui/gui.h>

namespace Genesis
{
	class ResourceShader;
    class VertexBuffer;
}

namespace Hyperscape
{

class TableRow;

typedef std::vector< TableRow* > TableRowVector;
typedef std::vector< float > TableDimensions;
typedef std::vector< Genesis::Gui::Text* > TextVector;


///////////////////////////////////////////////////////////////////////////////
// Table
///////////////////////////////////////////////////////////////////////////////

class Table: public Genesis::Gui::GuiElement
{
public:
						Table();
	virtual				~Table();

	virtual void		Update( float delta ) override;
	virtual void		Render() override;

	int					AddRow( TableRow* pTableRow );
	TableRow*			GetRow( unsigned int index ) const;
	unsigned int		GetRowCount() const;
	float				GetRowHeight() const;

	void				NotifyContentUpdated();

private:
	bool				UpdateDimensions( TableRow* pTableRow );
	void				UpdatePositions();
	void				UpdateContents();

	void				OnMousePressedCallback();

	TableRowVector		m_Rows;
	TableDimensions		m_Dimensions;
	TextVector			m_Text;

	bool				m_PositionsDirty;
	bool				m_ContentsDirty;

	float				m_RowHeight;

	Genesis::InputCallbackToken	m_MousePressedToken;

    Genesis::ResourceShader* m_pShader;
    Genesis::VertexBuffer* m_pVertexBuffer;

};

inline unsigned int Table::GetRowCount() const
{
	return static_cast<unsigned int>(m_Rows.size());
}

inline TableRow* Table::GetRow( unsigned int index ) const
{
	if ( index < m_Rows.size() )
		return m_Rows[ index ];
	else
		return nullptr;
}

inline void Table::NotifyContentUpdated()
{
	m_PositionsDirty = true;
	m_ContentsDirty = true;
}

inline float Table::GetRowHeight() const
{
	return m_RowHeight;
}

}
