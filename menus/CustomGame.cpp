/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Framework.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "Action.h"
#include "Table.h"
#include "YesNoMessageBox.h"
#include "keydefs.h"

#define ART_BANNER		"gfx/shell/head_custom"

#define MAX_MODS		512	// engine limit

enum
{
	COLUMN_TYPE = 0,
	COLUMN_NAME,
	COLUMN_VER,
	COLUMN_SIZE,
};

struct mod_t
{
	char dir[64];
	char webSite[256];
	char type[32];
	char name[32];
	char ver[32];
	char size[32];
	uint64_t bytes;

	int TypeCmp( const mod_t &other ) const
	{
		return stricmp( type, other.type );
	}

	int NameCmp( const mod_t &other ) const
	{
		return stricmp( name, other.name );
	}

	int SizeCmp( const mod_t &other ) const
	{
		if( bytes > other.bytes ) return 1;
		else if( bytes < other.bytes ) return -1;
		return 0;
	}

#define GENERATE_COMPAR_FN( method ) \
	static int method ## Ascend( const void *a, const void *b ) \
	{\
		return (( const mod_t *)a)->method( *(( const mod_t *)b) );\
	}\
	static int method ## Descend( const void *a, const void *b ) \
	{\
		return (( const mod_t *)b)->method( *(( const mod_t *)a) );\
	}\

	GENERATE_COMPAR_FN( TypeCmp )
		GENERATE_COMPAR_FN( NameCmp )
		GENERATE_COMPAR_FN( SizeCmp )
#undef GENERATE_COMPAR_FN
};

class CMenuModListModel : public CMenuBaseModel
{
public:
	CMenuModListModel() : m_iSortingColumn( COLUMN_NAME ) {}

	void Update() override;
	int GetColumns() const override { return 4; }
	int GetRows() const override { return m_iNumItems; }
	const char *GetCellText( int line, int column ) override
	{
		switch( column )
		{
		case COLUMN_TYPE: return mods[line].type;
		case COLUMN_NAME: return mods[line].name;
		case COLUMN_VER: return mods[line].ver;
		case COLUMN_SIZE: return mods[line].size;
		default: return NULL;
		}
	}
	bool Sort( int column, bool ascend ) override;

	mod_t  mods[MAX_MODS];

	int m_iNumItems;
private:
	int m_iSortingColumn;
	bool m_bAscend;
};

class CMenuCustomGame : public CMenuFramework
{
public:
	CMenuCustomGame() : CMenuFramework( "CMenuCustomGame" ) { }

private:
	void ShowDialog( void )
	{
		msgBox.ToggleVisibility();
	}
	void ChangeGame( void *pExtra );
	void Go2Site( void *pExtra );
	void UpdateExtras();
	virtual void _Init() override;

	CMenuPicButton *load;
	CMenuPicButton *go2url;

	// prompt dialog
	CMenuYesNoMessageBox msgBox;

	CMenuTable	modList;
	CMenuModListModel modListModel;
};

void CMenuCustomGame::ChangeGame( void *pExtra )
{
	char cmd[128];
	snprintf( cmd, sizeof( cmd ), "game %s\n", (const char *)pExtra );
	EngFuncs::ClientCmd( FALSE, cmd );
}

void CMenuCustomGame::Go2Site( void *pExtra )
{
	const char *url = (const char *)pExtra;
	if( url[0] )
		EngFuncs::ShellExecute( url, NULL, false );
}

void CMenuCustomGame::UpdateExtras()
{
	int i = modList.GetCurrentIndex();

	load->onReleased.pExtra = modListModel.mods[i].dir;
	load->SetGrayed( !stricmp( modListModel.mods[i].dir, gMenu.m_gameinfo.gamefolder ) );

	go2url->onReleased.pExtra = modListModel.mods[i].webSite;
	go2url->SetGrayed( modListModel.mods[i].webSite[0] == 0 );

	msgBox.onPositive.pExtra = modListModel.mods[i].dir;
}

/*
=================
CMenuModListModel::Update
=================
*/
void CMenuModListModel::Update( void )
{
	int i;

	for( i = 0; ; i++ )
	{
		gameinfo2_t *gi = EngFuncs::GetModInfo( i );

		if( !gi )
			break;

		Q_strncpy( mods[i].dir, gi->gamefolder, sizeof( mods[i].dir ) );
		Q_strncpy( mods[i].webSite, gi->game_url, sizeof( mods[i].webSite ) );
		Q_strncpy( mods[i].type, gi->type, sizeof( mods[i].type ) );
		Q_strncpy( mods[i].ver, gi->version, sizeof( mods[i].ver ) );

		if( ColorStrlen( gi->title ) > sizeof( mods[i].name ) - 1 ) // NAME_LENGTH
		{
			size_t s = sizeof( mods[i].name ) - 4;

			Q_strncpy( mods[i].name, gi->title, s );

			mods[i].name[s] = mods[i].name[s + 1] = mods[i].name[s + 2] = '.';
			mods[i].name[s + 3] = 0;
		}
		else Q_strncpy( mods[i].name, gi->title, sizeof( mods[i].name ) );

		mods[i].bytes = gi->size;
		if( gi->size > 0 )
			Q_strncpy( mods[i].size, Q_memprint( gi->size ), sizeof( mods[i].size ) );
		else Q_strncpy( mods[i].size, "0.0 Mb", sizeof( mods[i].size ) );
	}

	m_iNumItems = i;

	if( i != 0 )
	{
		if( m_iSortingColumn != -1 )
			Sort( m_iSortingColumn, m_bAscend );
	}
}

bool CMenuModListModel::Sort( int column, bool ascend )
{
	m_iSortingColumn = column;
	if( column == -1 )
		return false; // disabled

	m_bAscend = ascend;

	switch( column )
	{
	case COLUMN_TYPE:
		qsort( mods, m_iNumItems, sizeof( mod_t ),
			ascend ? mod_t::TypeCmpAscend : mod_t::TypeCmpDescend );
		return true;
	case COLUMN_NAME:
		qsort( mods, m_iNumItems, sizeof( mod_t ),
			ascend ? mod_t::NameCmpAscend : mod_t::NameCmpDescend );
		return true;
	case COLUMN_SIZE:
		qsort( mods, m_iNumItems, sizeof( mod_t ),
			ascend ? mod_t::SizeCmpAscend : mod_t::SizeCmpDescend );
		return true;
	}

	return false;
}

/*
=================
UI_CustomGame_Init
=================
*/
void CMenuCustomGame::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	msgBox.SetMessage( L( "GameUI_ForceGameRestart" ) );
	msgBox.onPositive = MenuCb( &CMenuCustomGame::ChangeGame );
	msgBox.Link( this );

	AddItem( banner );
	load = AddButton( L( "Activate" ), L( "Activate selected custom game" ), PC_ACTIVATE,
		VoidCb( &CMenuCustomGame::ShowDialog ) );

	go2url = AddButton( L( "Visit web site" ), L( "Visit the web site of game developers" ), PC_VISIT_WEB_SITE,
		MenuCb( &CMenuCustomGame::Go2Site ) );
	AddButton( L( "Done" ), L( "Return to main menu" ), PC_DONE,	// Done - уже где-то было, поэтому в отдельный файл повторно не выношу
		VoidCb( &CMenuCustomGame::Hide ) );

	modList.onChanged = VoidCb( &CMenuCustomGame::UpdateExtras );
	modList.SetupColumn( 0, L( "GameUI_Type" ), 0.20f );
	modList.SetupColumn( 1, L( "Name" ), 0.50f );
	modList.SetupColumn( 2, L( "Ver" ), 0.15f );
	modList.SetupColumn( 3, L( "Size" ), 0.15f );
	modList.SetModel( &modListModel );
	modList.bAllowSorting = true;
	modList.SetRect( 360, 230, -20, 465 );

	AddItem( modList );

	for( int i = 0; i < modListModel.GetRows(); i++ )
	{
		if( !stricmp( modListModel.mods[i].dir, gMenu.m_gameinfo.gamefolder ) )
		{
			modList.SetCurrentIndex( i );
			if( modList.onChanged )
				modList.onChanged( &modList );
			break;
		}
	}
}

ADD_MENU( menu_customgame, CMenuCustomGame, UI_CustomGame_Menu )