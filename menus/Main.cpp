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
#include "Action.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "keydefs.h"
#include "MenuStrings.h"
#include "PlayerIntroduceDialog.h"
#include "Table.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"
#define ART_GAMEICON	"gfx/shell/game_icon.tga"

class CMenuMain: public CMenuFramework
{
public:
	CMenuMain() : CMenuFramework( "CMenuMain" ) { }

	bool KeyDown( int key ) override;
private:
	void _Init() override;
	void _VidInit( ) override;
	void Draw();
	void Think() override;
	
	void VidInit(bool connected);

	void QuitDialog( void *pExtra = NULL );
	void DisconnectCb();
	void DisconnectDialogCb();
	void HazardCourseDialogCb();
	void HazardCourseCb();

	CMenuPicButton	console;
	CMenuPicButton achievements;
	class CMenuMainBanner : public CMenuBannerBitmap
	{
	public:
		virtual void Draw();
	} banner;

	CMenuPicButton	resumeGame;
	CMenuPicButton	disconnect;
	CMenuPicButton	newGame;
	CMenuPicButton	hazardCourse;
	CMenuPicButton	configuration;
	CMenuPicButton	saveRestore;
	CMenuPicButton	multiPlayer;
	CMenuPicButton	customGame;
	CMenuPicButton	previews;
	CMenuPicButton	quit;

	// buttons on top right. Maybe should be drawn if fullscreen == 1?
	CMenuBitmap	minimizeBtn;
	CMenuBitmap	quitButton;

	// quit dialog
	CMenuYesNoMessageBox dialog;

	bool bTrainMap;
	bool bCustomGame;

	class CGameLogo : public CMenuBitmap
	{
		void Draw() override;
	} gameLogo;
	HIMAGE	hGameLogo;
};

void CMenuMain::CMenuMainBanner::Draw()
{
	if( !CMenuBackgroundBitmap::ShouldDrawLogoMovie() )
		return; // no logos for steam background

	if( EngFuncs::GetLogoLength() <= 0.05f || EngFuncs::GetLogoWidth() <= 32 )
		return;	// don't draw stub logo (GoldSrc rules)

	float	logoWidth, logoHeight, logoPosY;
	float	scaleX, scaleY;

	scaleX = ScreenWidth / 640.0f;
	scaleY = ScreenHeight / 480.0f;

	// a1ba: multiply by height scale to look better on widescreens
	logoWidth = EngFuncs::GetLogoWidth() * scaleX;
	logoHeight = EngFuncs::GetLogoHeight() * scaleY * uiStatic.scaleY;
	logoPosY = 70 * scaleY * uiStatic.scaleY;	// 70 it's empirically determined value (magic number)

	EngFuncs::DrawLogo( "logo.avi", 0, logoPosY, logoWidth, logoHeight );
}

void CMenuMain::QuitDialog(void *pExtra)
{
	dialog.SetMessage( L( "GameUI_QuitConfirmationText" ) );
	dialog.onPositive.SetCommand( FALSE, "quit\n" );
	dialog.Show();
}

void CMenuMain::DisconnectCb()
{
	EngFuncs::ClientCmd( false, "disconnect\n" );
	VidInit( false );
	CalcPosition();
	CalcSizes();
	VidInitItems();
}

void CMenuMain::DisconnectDialogCb()
{
	dialog.onPositive = VoidCb( &CMenuMain::DisconnectCb );
	dialog.SetMessage( L( "GameUI_GameMenu_DisconnectD" ) );
	dialog.Show();
}

void CMenuMain::HazardCourseDialogCb()
{
	dialog.onPositive = VoidCb( &CMenuMain::HazardCourseCb );;
	dialog.SetMessage( L( "StringsList_234" ) );
	dialog.Show();
}

/*
=================
CMenuMain::Key
=================
*/
bool CMenuMain::KeyDown( int key )
{
	UI_SetCursorPos( 0, 0 );

	if( UI::Key::IsEscape( key ) )
	{
		if( CL_IsActive() )
		{
			if( !dialog.IsVisible() )
			{
				UI_CloseMenu();
			}
		}
		else
			QuitDialog();

		return true;
	}
	
	return CMenuFramework::KeyDown( key );
}

/*
=================
UI_Main_HazardCourse
=================
*/
void CMenuMain::HazardCourseCb()
{
	if( EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) > 1 )
		EngFuncs::HostEndGame( "end of the game" );

	EngFuncs::CvarSetValue( "skill", 1.0f );
	EngFuncs::CvarSetValue( "deathmatch", 0.0f );
	EngFuncs::CvarSetValue( "teamplay", 0.0f );
	EngFuncs::CvarSetValue( "pausable", 1.0f ); // singleplayer is always allowing pause
	EngFuncs::CvarSetValue( "coop", 0.0f );
	EngFuncs::CvarSetValue( "maxplayers", 1.0f ); // singleplayer

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	EngFuncs::ClientCmd( FALSE, "hazardcourse\n" );
}

void CMenuMain::_Init( void )
{
//	if( gMenu.m_gameinfo.trainmap[0] && stricmp( gMenu.m_gameinfo.trainmap, gMenu.m_gameinfo.startmap ) != 0 )
//		bTrainMap = true;
//	else
	bTrainMap = false; // diffusion - disabled for now

	if( EngFuncs::GetCvarFloat( "host_allow_changegame" ))
		bCustomGame = true;
	else bCustomGame = false;

	// game logo/icon
	hGameLogo = EngFuncs::PIC_Load( ART_GAMEICON, PIC_KEEP_SOURCE | PIC_EXPAND_SOURCE );
	gameLogo.iFlags = QMF_INACTIVE;
	gameLogo.SetRect( 390, 225, 480, 450 );
	gameLogo.SetPicture( ART_GAMEICON );

	// console
	console.SetNameAndStatus( L( "GameUI_Console" ), L( "Show console" ) );
	console.iFlags |= QMF_NOTIFY;
	console.SetPicture( PC_CONSOLE );
	console.SetVisibility( gpGlobals->developer );
	SET_EVENT_MULTI( console.onReleased,
	{
		UI_SetActiveMenu( FALSE );
		EngFuncs::KEY_SetDest( KEY_CONSOLE );
	});

	resumeGame.SetNameAndStatus( L( "GameUI_GameMenu_ResumeGame" ), L( "StringsList_188" ) );
	resumeGame.SetPicture( PC_RESUME_GAME );
	resumeGame.iFlags |= QMF_NOTIFY;
	resumeGame.onReleased = UI_CloseMenu;

	disconnect.SetNameAndStatus( L( "GameUI_GameMenu_Disconnect" ), L( "Disconnect from server" ) );
	disconnect.SetPicture( PC_DISCONNECT );
	disconnect.iFlags |= QMF_NOTIFY;
	disconnect.onReleased = VoidCb( &CMenuMain::DisconnectDialogCb );

	newGame.SetNameAndStatus( L( "GameUI_NewGame" ), L( "StringsList_189" ) );
	newGame.SetPicture( PC_NEW_GAME );
	newGame.iFlags |= QMF_NOTIFY;
	newGame.onReleased = UI_NewGame_Menu;

	hazardCourse.SetNameAndStatus( L( "GameUI_TrainingRoom" ), L( "StringsList_190" ) );
	hazardCourse.SetPicture( PC_HAZARD_COURSE );
	hazardCourse.iFlags |= QMF_NOTIFY;
	hazardCourse.onReleasedClActive = VoidCb( &CMenuMain::HazardCourseDialogCb );
	hazardCourse.onReleased = VoidCb( &CMenuMain::HazardCourseCb );

	multiPlayer.SetNameAndStatus( L( "GameUI_Multiplayer" ), L( "StringsList_198" ) );
	multiPlayer.SetPicture( PC_MULTIPLAYER );
	multiPlayer.iFlags |= QMF_NOTIFY;
	multiPlayer.onReleased = UI_MultiPlayer_Menu;

	configuration.SetNameAndStatus( L( "GameUI_Options" ), L( "StringsList_193" ) );
	configuration.SetPicture( PC_CONFIG );
	configuration.iFlags |= QMF_NOTIFY;
	configuration.onReleased = UI_Options_Menu;

	saveRestore.iFlags |= QMF_NOTIFY;

	customGame.SetNameAndStatus( L( "GameUI_ChangeGame" ), L( "StringsList_530" ) );
	customGame.SetPicture( PC_CUSTOM_GAME );
	customGame.iFlags |= QMF_NOTIFY;
	customGame.onReleased = UI_CustomGame_Menu;

	previews.SetNameAndStatus( L( "Previews" ), L( "StringsList_400" ) );
	previews.SetPicture( PC_PREVIEWS );
	previews.iFlags |= QMF_NOTIFY;
	SET_EVENT( previews.onReleased, EngFuncs::ShellExecute( MenuStrings[ IDS_MEDIA_PREVIEWURL ], NULL, false ) );

	quit.SetNameAndStatus( L( "GameUI_GameMenu_Quit" ), L( "GameUI_QuitConfirmationText" ) );
	quit.SetPicture( PC_QUIT );
	quit.iFlags |= QMF_NOTIFY;
	quit.onReleased = MenuCb( &CMenuMain::QuitDialog );

/*	quitButton.SetPicture( ART_CLOSEBTN_N, ART_CLOSEBTN_F, ART_CLOSEBTN_D );
	quitButton.iFlags = QMF_MOUSEONLY;
	quitButton.eFocusAnimation = QM_HIGHLIGHTIFFOCUS;
	quitButton.onReleased = MenuCb( &CMenuMain::QuitDialog );

	minimizeBtn.SetPicture( ART_MINIMIZE_N, ART_MINIMIZE_F, ART_MINIMIZE_D );
	minimizeBtn.iFlags = QMF_MOUSEONLY;
	minimizeBtn.eFocusAnimation = QM_HIGHLIGHTIFFOCUS;
	minimizeBtn.onReleased.SetCommand( FALSE, "minimize\n" );*/

	if ( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY || gMenu.m_gameinfo.startmap[0] == 0 )
		newGame.SetGrayed( true );

	if ( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		multiPlayer.SetGrayed( true );

	if ( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY )
	{
		saveRestore.SetGrayed( true );
		hazardCourse.SetGrayed( true );
	}

	// too short execute string - not a real command
	if( strlen( MenuStrings[IDS_MEDIA_PREVIEWURL] ) <= 3 )
		previews.SetGrayed( true );

	// server.dll needs for reading savefiles or startup newgame
	if( !EngFuncs::CheckGameDll( ))
	{
		saveRestore.SetGrayed( true );
		hazardCourse.SetGrayed( true );
		newGame.SetGrayed( true );
	}

	dialog.Link( this );

	achievements.SetNameAndStatus( L( "Achievements" ), L( "View" ) );
	achievements.SetPicture( PC_ACHIEVEMENTS );
	achievements.iFlags |= QMF_NOTIFY;
	achievements.onReleased = UI_Achievements_Menu;

	AddItem( banner );

	AddItem( console );

	AddItem( disconnect );
	AddItem( resumeGame );
	AddItem( newGame );

	if ( bTrainMap )
		AddItem( hazardCourse );

	AddItem( saveRestore );
	AddItem( configuration );
	AddItem( multiPlayer );

	// diffusion - disabled unnecessary menus
//	if ( bCustomGame )
//		AddItem( customGame );

	AddItem( previews );
	AddItem( quit );
//	AddItem( minimizeBtn );
//	AddItem( quitButton );

	AddItem( achievements );
	AddItem( gameLogo );
}

/*
=================
UI_Main_Init
=================
*/
void CMenuMain::VidInit( bool connected )
{
	// statically positioned items
	minimizeBtn.SetRect( uiStatic.width - 72, 13, 32, 32 );
	quitButton.SetRect( uiStatic.width - 36, 13, 32, 32 );
	disconnect.SetCoord( 72, MenuYOffset + 180 );
	resumeGame.SetCoord( 72, MenuYOffset + 230 );
	newGame.SetCoord( 72, MenuYOffset + 280 );
	hazardCourse.SetCoord( 72, MenuYOffset + 330 );

	bool isSingle = gpGlobals->maxClients < 2;

	if( CL_IsActive() && isSingle )
	{
		saveRestore.SetNameAndStatus( L( "Save\\Load Game" ), L( "StringsList_192" ) );
		saveRestore.SetPicture( PC_SAVE_LOAD_GAME );
		saveRestore.onReleased = UI_SaveLoad_Menu;
	}
	else
	{
		saveRestore.SetNameAndStatus( L( "GameUI_LoadGame" ), L( "StringsList_191" ) );
		saveRestore.SetPicture( PC_LOAD_GAME );
		saveRestore.onReleased = UI_LoadGame_Menu;
	}

	if( connected )
	{
		resumeGame.Show();
		if( CL_IsActive() )
		{
			disconnect.Show();
			if( gpGlobals->maxClients == 1 )
			{
				disconnect.SetNameAndStatus( L( "GameUI_GameMenu_BackToMenu" ), L( "Back to main menu" ) );
				disconnect.SetPicture( PC_BACK_TO_MENU );
				dialog.SetMessage( L( "GameUI_GameMenu_BackToMenuD" ) );
			}
			else
			{
				disconnect.SetNameAndStatus( L( "GameUI_GameMenu_Disconnect" ), L( "Disconnect from server" ) );
				disconnect.SetPicture( PC_DISCONNECT );
				dialog.SetMessage( L( "GameUI_GameMenu_DisconnectD" ) );
			}
			console.pos.y = MenuYOffset + 130;
		}
		else
		{
			disconnect.Hide();
			console.pos.y = MenuYOffset + 180;
		}
	}
	else
	{
		resumeGame.Hide();
		disconnect.Hide();
		console.pos.y = MenuYOffset + 230;
	}
	
	console.pos.x = 72;
	console.CalcPosition();
	saveRestore.SetCoord(72, MenuYOffset + (bTrainMap ? 380 : 330));
	configuration.SetCoord(72, MenuYOffset + (bTrainMap ? 430 : 380));
	multiPlayer.SetCoord(72, MenuYOffset + (bTrainMap ? 480 : 430));
//	customGame.SetCoord(72, MenuYOffset + (bTrainMap ? 530 : 480));
	achievements.SetCoord( 72, MenuYOffset + (bTrainMap ? 630 : 480) );
	previews.SetCoord(72, MenuYOffset + ((bCustomGame) ? (bTrainMap ? 580 : 530) : (bTrainMap ? 530 : 480)));
	quit.SetCoord(72, MenuYOffset + ((bCustomGame) ? (bTrainMap ? 630 : 580) : (bTrainMap ? 580 : 530)));
}

void CMenuMain::_VidInit()
{
	VidInit( CL_IsActive() );
}

void CMenuMain::Draw(void)
{
	if( !EngFuncs::ClientInGame() )
		UI_LoadBackgroundMap();

	BaseClass::Draw();
}

void CMenuMain::Think()
{
	if( gpGlobals->developer )
	{
		if( !console.IsVisible() )
			console.Show();
	}
	else
	{
		if( console.IsVisible() )
			console.Hide();
	}

	CMenuFramework::Think();
}

ADD_MENU( menu_main, CMenuMain, UI_Main_Menu );

void CMenuMain::CGameLogo::Draw()
{
	UI_DrawPic( 50, 50, m_scSize.w * 0.625, m_scSize.w * 0.625, uiColorWhite, szPic, QM_DRAWTRANS ); // using w twice to make sure it's a square...
}