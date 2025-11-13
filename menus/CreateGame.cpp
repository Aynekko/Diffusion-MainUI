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
#include "keydefs.h"
#include "Bitmap.h"
#include "Field.h"
#include "CheckBox.h"
#include "PicButton.h"
#include "Table.h"
#include "Action.h"
#include "YesNoMessageBox.h"
#include "SpinControl.h"

#define ART_BANNER		"gfx/shell/head_creategame"

struct map_t
{
	char name[CS_SIZE];
	char desc[CS_SIZE];
};

class CMenuCreateGame;
class CMenuMapListModel : public CMenuBaseModel, public CUtlVector<map_t>
{
public:
	CMenuMapListModel( CMenuCreateGame *parent ) : parent( parent ) { }

	void Update() override;
	int GetColumns() const override { return 2; }
	int GetRows() const override { return Count(); }
	const char *GetCellText( int line, int column ) override
	{
		switch( column )
		{
		case 0: return Element( line ).name;
		case 1: return Element( line ).desc;
		}

		return NULL;
	}

	CMenuCreateGame *parent;
};

class CMenuCreateGame : public CMenuFramework
{
public:
	CMenuCreateGame() : CMenuFramework("CMenuCreateGame"), mapsListModel( this ) { }
	static void Begin( CMenuBaseItem *pSelf, void *pExtra );
	void Reload( void ) override;
	void SaveCvars( void );

	CMenuField	maxClients;
	CMenuField	hostName;
	CMenuField	password;
	CMenuCheckBox   nat;
	CMenuCheckBox   teamplay;
	CMenuCheckBox   bhop;
	CMenuSpinControl   bots;

	// newgame prompt dialog
	CMenuYesNoMessageBox msgBox;

	CMenuTable        mapsList;
	CMenuMapListModel mapsListModel;

	CMenuPicButton *done;
private:
	void _Init() override;
	void _VidInit() override;
};

/*
=================
CMenuCreateGame::Begin
=================
*/
void CMenuCreateGame::Begin( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuCreateGame *menu = (CMenuCreateGame*)pSelf->Parent();
	int item = menu->mapsList.GetCurrentIndex();
	if( !menu->mapsListModel.IsValidIndex( item ) )
		return;

	if( item == 0 )
		item = EngFuncs::RandomLong( 1, menu->mapsListModel.GetRows() - 1 );

	const char *mapName = menu->mapsListModel[item].name;

	if( !EngFuncs::IsMapValid( mapName ))
		return;	// bad map

	if( EngFuncs::GetCvarFloat( "host_serverstate" ) )
	{
		if(	EngFuncs::GetCvarFloat( "maxplayers" ) == 1.0f )
			EngFuncs::HostEndGame( "end of the game" );
		else
			EngFuncs::HostEndGame( "starting new server" );
	}

	EngFuncs::CvarSetValue( "deathmatch", 1.0f );	// start deathmatch as default

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	// all done, start server
	const char *listenservercfg = EngFuncs::GetCvarString( "lservercfgfile" );
	EngFuncs::WriteServerConfig( listenservercfg );

	char cmd[1024];
	snprintf( cmd, sizeof( cmd ), "exec %s\n", listenservercfg );
	EngFuncs::ClientCmd( true, cmd );

	// dirty listenserver config form old xash may rewrite maxplayers
	menu->maxClients.WriteCvar();

	// diffusion - UI cvars must have priority over listenserver
	menu->SaveCvars();

	// notify server to not execute listenserver config again
	EngFuncs::CvarSetValue( "sv_startfromui", 1.0f );

	// hack: wait three frames allowing server to completely shutdown, reapply maxplayers and start new map
	char cmd2[256];
	Com_EscapeCommand( cmd2, mapName, sizeof( cmd2 ) );
	snprintf( cmd, sizeof( cmd ), "disconnect;menu_connectionprogress localserver;wait;wait;wait;maxplayers %i;latch;map %s\n", atoi( menu->maxClients.GetBuffer() ), cmd2 );
	EngFuncs::ClientCmd( false, cmd );
}

/*
=================
CMenuMapListModel::Update
=================
*/
void CMenuMapListModel::Update( void )
{
	char *afile;

	if( !uiStatic.needMapListUpdate )
		return;

	RemoveAll();

	if( !EngFuncs::CreateMapsList( true ) || (afile = (char *)EngFuncs::COM_LoadFile( "maps.lst", NULL )) == NULL )
	{
		parent->done->SetGrayed( true );
		Con_Printf( "Cmd_GetMapsList: can't open maps.lst\n" );
		return;
	}

	{
		map_t map;
		Q_strncpy( map.name, L( "GameUI_RandomMap" ), sizeof( map.name ) );
		Q_strncpy( map.desc, "", sizeof( map.desc ) );
		AddToTail( map );
	}

	char *pfile = afile;
	char token[1024];
	
	while( (pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) )) != NULL )
	{
		map_t map;

		Q_strncpy( map.name, token, sizeof( map.name ) );
		if( (pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) )) == NULL )
		{
			Q_strncpy( map.desc, map.name, sizeof( map.desc ) );
			AddToTail( map );
			break; // unexpected end of file
		}
		Q_strncpy( map.desc, token, sizeof( map.desc ) );
		AddToTail( map );
	}

	if( Count() <= 1 )
		parent->done->SetGrayed( true );

	EngFuncs::COM_FreeFile( afile );
	uiStatic.needMapListUpdate = false;
}

/*
=================
CMenuCreateGame::Init
=================
*/
void CMenuCreateGame::_Init( void )
{
	uiStatic.needMapListUpdate = true;
	banner.SetPicture( ART_BANNER );

	nat.SetNameAndStatus( "NAT", L( "Use NAT Bypass instead of direct mode" ) );
	nat.bChecked = true;
	nat.LinkCvar( "sv_nat" );

	// add them here, so "done" button can be used by mapsListModel::Update
	AddItem( banner );
//	CMenuPicButton *advOpt = AddButton( L( "Adv. Options" ), L( "Open the game advanced options menu" ), PC_ADV_OPT, UI_AdvServerOptions_Menu );
//	advOpt->SetGrayed( !UI_AdvServerOptions_IsAvailable() );

	done = AddButton( L( "Start game" ), L( "Start the multiplayer game" ), PC_START_GAME, Begin );
	done->onReleasedClActive = msgBox.MakeOpenEvent();

	mapsList.SetCharSize( QM_SMALLFONT );
	mapsList.SetupColumn( 0, L( "Map" ), 0.5f );
	mapsList.SetupColumn( 1, L( "Title" ), 0.5f );
	mapsList.SetModel( &mapsListModel );

	hostName.szName = L( "Server name" );
	hostName.iMaxLength = 28;
	hostName.LinkCvar( "hostname" );
	
	maxClients.iMaxLength = 3;
	maxClients.bNumbersOnly = true;
	maxClients.szName = L( "Max. players" );
	SET_EVENT_MULTI( maxClients.onChanged,
	{
		CMenuField *self = (CMenuField*)pSelf;
		const char *buf = self->GetBuffer();
		if( buf[0] == 0 ) return;
		
		int players = atoi( buf );
		if( players <= 1 )
			self->SetBuffer( "2" );
		else if( players > 32 )
			self->SetBuffer( "32" );
	});
	SET_EVENT_MULTI( maxClients.onCvarGet,
	{
		CMenuField *self = (CMenuField*)pSelf;
		const char *buf = self->GetBuffer();
		
		int players = atoi( buf );
		if( players <= 1 )
			self->SetBuffer( "16" );
		else if( players > 32 )
			self->SetBuffer( "32" );
	});
	maxClients.LinkCvar( "maxplayers" );

	password.szName = L( "Password" );
	password.iMaxLength = 16;
	password.eTextAlignment = QM_CENTER;
	password.bHideInput = true;
	password.LinkCvar( "sv_password" );

	msgBox.onPositive = Begin;
	msgBox.SetMessage( L( "Starting a new game will exit any current game, OK to exit?" ) );
	msgBox.Link( this );

	bots.szName = L( "GameUI_Bots" );
	bots.Setup( 0, 16, 1 );
	bots.SetRect( 72, MenuYOffset, 220, 32 );
	bots.LinkCvar( "bot_max", CMenuEditable::CVAR_VALUE );
	bots.onChanged = CMenuEditable::WriteCvarCb;

	teamplay.szName = L( "GameUI_Teamplay" );
	teamplay.bChecked = false;
	teamplay.LinkCvar( "mp_teamplay" );
	teamplay.SetCoord( 72, MenuYOffset + 50 );

	bhop.szName = L( "GameUI_Bhop" );
	bhop.bChecked = false;
	bhop.LinkCvar( "sv_enablebunnyhopping" );
	bhop.SetCoord( 72, MenuYOffset + 90 );

	AddButton( L( "GameUI_Cancel" ), L( "Return to the previous menu" ), PC_CANCEL, VoidCb( &CMenuCreateGame::Hide ) );
	AddItem( hostName );
	AddItem( maxClients );
	AddItem( password );
	AddItem( bots );
	AddItem( teamplay );
	AddItem( bhop );

	AddItem( nat );
	AddItem( mapsList );
}

void CMenuCreateGame::_VidInit()
{
	nat.SetCoord( 350, 685 );
	if( !EngFuncs::GetCvarFloat("public") )
		nat.Hide();
	else nat.Show();

	mapsList.SetRect( 610, 230, 800, 666 );

	hostName.SetRect( 350, 260, 205, 32 );
	maxClients.SetRect( 350, 360, 205, 32 );
	password.SetRect( 350, 460, 205, 32 );
}

void CMenuCreateGame::SaveCvars()
{
	hostName.WriteCvar();
	maxClients.WriteCvar();
	password.WriteCvar();
	bots.WriteCvar();
	teamplay.WriteCvar();
	bhop.WriteCvar();
	EngFuncs::CvarSetValue( "sv_nat", EngFuncs::GetCvarFloat( "public" ) ? nat.bChecked : 0 );
}

void CMenuCreateGame::Reload( void )
{
	mapsListModel.Update();
}

ADD_MENU( menu_creategame, CMenuCreateGame, UI_CreateGame_Menu );
