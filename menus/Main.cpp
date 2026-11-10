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
#include "YesNoMessageBox.h"
#include "MenuStrings.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"
#define ART_GAMEICON	"gfx/shell/game_icon.tga"

const float ArtCoord[4] = { 50.0f, 50.0f, 300.0f, 300.0f }; // x y w h

static bool bTrainMap = false;
static bool bCustomGame = false;
static bool bReInit = false;
static int iCvarExists = -1;

class CMenuLinksMessageBox : public CMenuYesNoMessageBox
{
public:
	CMenuLinksMessageBox() : CMenuYesNoMessageBox( true ) {}

	void _Init() override
	{
		CMenuYesNoMessageBox::_Init();

		RemoveItem( yes ); // remove OK button - it's not centered, I'll use my own button

		int textWidth, textHeight;

		//--------------------------------------------------------------------------
		// set link 1 - itchio
		//--------------------------------------------------------------------------
		link1.SetNameAndStatus( "itch.io", NULL );
		link1.SetCoord( (640 - UI_BUTTONS_WIDTH) / 2, 140 );
		link1.iFlags |= QMF_NOTIFY;
		SET_EVENT( link1.onReleased, EngFuncs::ShellExecute( "https://aynekko.itch.io/diffusion", NULL, false ) );

		// Calculate text width and set button size to match
		textWidth = g_FontMgr->GetTextWideScaled( link1.font, link1.szName, link1.charSize );
		textHeight = g_FontMgr->GetTextHeightExt( link1.font, link1.szName, link1.charSize, textWidth );
		textHeight += 8;

		const int y_base = 80;
		const int y_add = (int)(textHeight * 0.7f);

		// Center using the real text-based width
		link1.SetRect( (640 - textWidth) / 2, y_base, textWidth, textHeight );
		AddItem( link1 );

		//--------------------------------------------------------------------------
		// set link 2 - moddb
		//--------------------------------------------------------------------------
		link2.SetNameAndStatus( "ModDB", NULL );
		link2.SetCoord( (640 - UI_BUTTONS_WIDTH) / 2, 140 );
		link2.iFlags |= QMF_NOTIFY;
		SET_EVENT( link2.onReleased, EngFuncs::ShellExecute( "https://www.moddb.com/mods/diffusion", NULL, false ) );

		textWidth = g_FontMgr->GetTextWideScaled( link2.font, link2.szName, link2.charSize );
		textHeight = g_FontMgr->GetTextHeightExt( link2.font, link2.szName, link2.charSize, textWidth );
		textHeight += 8;

		link2.SetRect( (640 - textWidth) / 2, y_base + y_add, textWidth, textHeight );
		AddItem( link2 );

		//--------------------------------------------------------------------------
		// set link 3 - OST/Spotify
		//--------------------------------------------------------------------------
		link3.SetNameAndStatus( "OST (Spotify)", NULL );
		link3.SetCoord( (640 - UI_BUTTONS_WIDTH) / 2, 140 );
		link3.iFlags |= QMF_NOTIFY;
		SET_EVENT( link3.onReleased, EngFuncs::ShellExecute( "https://open.spotify.com/artist/5WlZjaIwAqje4r2OGZ3nXG", NULL, false ) );

		textWidth = g_FontMgr->GetTextWideScaled( link3.font, link3.szName, link3.charSize );
		textHeight = g_FontMgr->GetTextHeightExt( link3.font, link3.szName, link3.charSize, textWidth );
		textHeight += 8;

		link3.SetRect( (640 - textWidth) / 2, y_base + y_add + y_add, textWidth, textHeight );
		AddItem( link3 );

		//--------------------------------------------------------------------------
		// close button
		//--------------------------------------------------------------------------
		Close.SetNameAndStatus( L( "GameUI_OK" ), NULL );
		Close.SetCoord( (640 - UI_BUTTONS_WIDTH) / 2, 140 );
		Close.iFlags |= QMF_NOTIFY;
		Close.onReleased = VoidCb( &CMenuLinksMessageBox::Hide );

		textWidth = g_FontMgr->GetTextWideScaled( Close.font, Close.szName, Close.charSize );
		textHeight = g_FontMgr->GetTextHeightExt( Close.font, Close.szName, Close.charSize, textWidth );
		textHeight += 8;

		Close.SetRect( (640 - textWidth) / 2, y_base + y_add + y_add + y_add + y_add, textWidth, textHeight );
		AddItem( Close );
	}

	void _VidInit( void ) override
	{
		SetRect( DLG_X + 192, 256, 640, 300 );
		pos.x += uiStatic.xOffset;
		pos.y += uiStatic.yOffset;
		CalcPosition();
		CalcSizes();

		dlgMessage1.SetRect( 0, 24, 640, 256 - 24 );
		dlgMessage1.SetCharSize( QM_DEFAULTFONT );

	}

	CMenuPicButton link1;
	CMenuPicButton link2;
	CMenuPicButton link3;
	CMenuPicButton Close;
};

class CMenuMain : public CMenuFramework
{
public:
	CMenuMain() : CMenuFramework( "CMenuMain" ) {}

	bool KeyDown( int key ) override;
private:
	void _Init() override;
	void _VidInit() override;
	void Draw();
	void Think() override;

	void VidInit( bool connected );

	void QuitDialog( void *pExtra = NULL );
	void DisconnectCb();
	void DisconnectDialogCb();
	void HazardCourseDialogCb();
	void HazardCourseCb();
	void LinksDialog( void *pExtra = NULL );

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

	CMenuLinksMessageBox links;

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

void CMenuMain::QuitDialog( void *pExtra )
{
	dialog.SetMessage( L( "GameUI_QuitConfirmationText" ) );
	dialog.onPositive.SetCommand( false, "quit\n" );
	dialog.Show();
}

void CMenuMain::LinksDialog( void *pExtra )
{
	links.SetMessage( L( "GameUI_LinksDialog" ) );
	links.Show();
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

	EngFuncs::ClientCmd( false, "hazardcourse\n" );
}

void CMenuMain::_Init( void )
{
	//	if( gMenu.m_gameinfo.trainmap[0] && stricmp( gMenu.m_gameinfo.trainmap, gMenu.m_gameinfo.startmap ) != 0 )
	//		bTrainMap = true;
	//	else
	bTrainMap = false; // diffusion - disabled for now

	if( EngFuncs::GetCvarFloat( "host_allow_changegame" ) )
		bCustomGame = true;
	else bCustomGame = false;

	// game logo/icon
	hGameLogo = EngFuncs::PIC_Load( ART_GAMEICON, PIC_KEEP_SOURCE | PIC_EXPAND_SOURCE );
	gameLogo.iFlags = QMF_INACTIVE;
	gameLogo.SetRect( ArtCoord[0], ArtCoord[1], ArtCoord[2], ArtCoord[3] );
	gameLogo.SetPicture( ART_GAMEICON );

	// console
	console.SetNameAndStatus( L( "GameUI_Console" ), L( "Show console" ) );
	console.iFlags |= QMF_NOTIFY;
	console.SetPicture( PC_CONSOLE );
	console.SetVisibility( gpGlobals->developer );
	SET_EVENT_MULTI( console.onReleased,
		{
			UI_SetActiveMenu( false );
			EngFuncs::KEY_SetDest( KEY_CONSOLE );
		} );

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
	previews.onReleased = MenuCb( &CMenuMain::LinksDialog );

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
		minimizeBtn.onReleased.SetCommand( false, "minimize\n" );*/

	if( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY || gMenu.m_gameinfo.startmap[0] == 0 )
		newGame.SetGrayed( true );

	if( gMenu.m_gameinfo.gamemode == GAME_SINGLEPLAYER_ONLY )
		multiPlayer.SetGrayed( true );

	if( gMenu.m_gameinfo.gamemode == GAME_MULTIPLAYER_ONLY )
	{
		saveRestore.SetGrayed( true );
		hazardCourse.SetGrayed( true );
	}

	// too short execute string - not a real command
	if( strlen( MenuStrings[IDS_MEDIA_PREVIEWURL] ) <= 3 )
		previews.SetGrayed( true );

	// server.dll needs for reading savefiles or startup newgame
	if( !EngFuncs::CheckGameDll() )
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

	if( bTrainMap )
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
	resumeGame.SetCoord( 72, MenuYOffset + 180 );
	disconnect.SetCoord( 72, MenuYOffset + 230 );
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
	saveRestore.SetCoord( 72, MenuYOffset + (bTrainMap ? 380 : 330) );
	configuration.SetCoord( 72, MenuYOffset + (bTrainMap ? 430 : 380) );
	multiPlayer.SetCoord( 72, MenuYOffset + (bTrainMap ? 480 : 430) );
	//	customGame.SetCoord(72, MenuYOffset + (bTrainMap ? 530 : 480));
	achievements.SetCoord( 72, MenuYOffset + (bTrainMap ? 630 : 480) );
	previews.SetCoord( 72, MenuYOffset + ((bCustomGame) ? (bTrainMap ? 580 : 530) : (bTrainMap ? 530 : 480)) );
	quit.SetCoord( 72, MenuYOffset + ((bCustomGame) ? (bTrainMap ? 630 : 580) : (bTrainMap ? 580 : 530)) );

	console.SetVisibility( true );
	newGame.SetVisibility( true );
	saveRestore.SetVisibility( true );
	configuration.SetVisibility( true );
	multiPlayer.SetVisibility( true );
	achievements.SetVisibility( true );
	previews.SetVisibility( true );
	quit.SetVisibility( true );
	gameLogo.SetVisibility( true );

	iCvarExists = EngFuncs::textfuncs.pfnIsCvarReadOnly( "ui_shadersdone" );

	// hide all menus while compiling shaders, just for satisfying visuals
	if( iCvarExists != -1 && (int)EngFuncs::GetCvarFloat( "cl_background" ) != 0 )
	{
		if( (int)EngFuncs::GetCvarFloat( "ui_shadersdone" ) != 1 )
		{
			console.SetVisibility( false );
			newGame.SetVisibility( false );
			saveRestore.SetVisibility( false );
			configuration.SetVisibility( false );
			multiPlayer.SetVisibility( false );
			achievements.SetVisibility( false );
			previews.SetVisibility( false );
			quit.SetVisibility( false );
			gameLogo.SetVisibility( false );
		}
	}
}

void CMenuMain::_VidInit()
{
	VidInit( CL_IsActive() );
	uiStatic.bgmap_hack_framecount = 0;
}

void CMenuMain::Draw( void )
{
	if( !EngFuncs::ClientInGame() && (int)EngFuncs::GetCvarFloat( "host_serverstate" ) == 0 )
	{
		uiStatic.bgmap_hack_framecount++;
		if( uiStatic.bgmap_hack_framecount > 60 )
			UI_LoadBackgroundMap();
	}

	BaseClass::Draw();
}

void CMenuMain::Think()
{
	if( iCvarExists != -1 )
	{
		if( (int)EngFuncs::GetCvarFloat( "ui_shadersdone" ) == 1 )
		{
			if( !bReInit )
			{
				bReInit = true;
				VidInit( CL_IsActive() );
				CalcPosition();
				CalcSizes();
				VidInitItems();
			}

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
		}
	}
	else
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
	}

	CMenuFramework::Think();
}

ADD_MENU( menu_main, CMenuMain, UI_Main_Menu );

void CMenuMain::CGameLogo::Draw()
{
	UI_DrawPic( ArtCoord[0], ArtCoord[1], ArtCoord[2], ArtCoord[3], uiColorWhite, szPic, QM_DRAWTRANS );
}