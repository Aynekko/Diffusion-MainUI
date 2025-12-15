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
#include "keydefs.h"
#include "TabView.h"
#include "CheckBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"
#include "Slider.h"

#define ART_BANNER	     	"gfx/shell/head_config"

class CMenuOptions: public CMenuFramework
{
private:
	void _Init( void ) override;
	void _VidInit() override;

public:
	typedef CMenuFramework BaseClass;
	CMenuOptions() : CMenuFramework("CMenuOptions") { }
	void Show();
	void Hide();
	void Think() override;
	void GetConfig( void );
	int	outlineWidth;

	CMenuCheckBox cl_achievement_notify;
	CMenuCheckBox cl_tutor;
	CMenuCheckBox cl_showhealthbars;
	CMenuSpinControl cl_hitmarker;
	CMenuCheckBox cl_useicon;
	CMenuCheckBox cl_subtitles;
	CMenuSpinControl cl_crosshair;
	CMenuSpinControl cl_largehud;
	CMenuCheckBox cl_centerhud;
	CMenuCheckBox allowConsole;
	CMenuSlider	cl_viewmodel_extras;
	CMenuSpinControl default_fov;

	// update dialog
	CMenuYesNoMessageBox msgBox;
};

void CMenuOptions::Think()
{
	allowConsole.bChecked = gpGlobals->developer;
	CMenuFramework::Think();
}

/*
=================
CMenuOptions::Init
=================
*/
void CMenuOptions::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	msgBox.SetMessage( L( "Check the Internet for updates?" ) );
	SET_EVENT( msgBox.onPositive, UI_OpenUpdatePage( false, true ) );

	msgBox.Link( this );

	AddItem( banner );
	CMenuPicButton *controls = AddButton( L( "Controls" ), L( "Change keyboard and mouse settings" ), PC_CONTROLS, UI_Controls_Menu, QMF_NOTIFY );
	controls->pos.y -= 100;
	CMenuPicButton *audio =  AddButton( L( "Audio" ), L( "Change sound volume and quality" ), PC_AUDIO, UI_Audio_Menu, QMF_NOTIFY );
	audio->pos.y -= 100;
	CMenuPicButton *video = AddButton( L( "Video" ), L( "Change screen size, video mode and gamma" ), PC_VIDEO, UI_VidOptions_Menu, QMF_NOTIFY );
	video->pos.y -= 100;
	CMenuPicButton *resolution = AddButton( L( "Video modes" ), L( "Set video modes and configure 3D accelerators." ), PC_VID_MODES, UI_VidModes_Menu, QMF_NOTIFY );
	resolution->pos.y -= 100;
/*	if( UI_IsXashFWGS() )
	{
		AddButton( L( "Touch" ), L( "Change touch settings and buttons" ),
			PC_TOUCH, UI_Touch_Menu, QMF_NOTIFY );
		AddButton( L( "GameUI_Joystick" ), L( "Change gamepad axis and button settings" ),
			PC_GAMEPAD, UI_GamePad_Menu, QMF_NOTIFY );
	}
*/
//	AddButton( L( "Update" ), L( "Check for updates" ),
//		PC_UPDATE, msgBox.MakeOpenEvent(), QMF_NOTIFY );

	CMenuPicButton *done = AddButton( L( "Done" ), L( "Go back to the Main menu" ),
		PC_DONE, VoidCb( &CMenuOptions::Hide ), QMF_NOTIFY );
	done->pos.y -= 50;

	default_fov.szName = L( "GameUI_FieldOfView" );
	default_fov.Setup( 60, 120, 5 );
	default_fov.SetRect( 300, MenuYOffset + 30, 220, 32 );
	default_fov.onChanged = CMenuEditable::WriteCvarCb;

	cl_subtitles.SetNameAndStatus( L( "GameUI_Subtitles" ), L( "-" ) );
	cl_subtitles.iFlags |= QMF_NOTIFY;
	cl_subtitles.SetCoord( 300, MenuYOffset + 80 );

	cl_achievement_notify.SetNameAndStatus( L( "GameUI_AchievementNotify" ), L( "-" ) );
	cl_achievement_notify.iFlags |= QMF_NOTIFY;
	cl_achievement_notify.SetCoord( 300, MenuYOffset + 130 );

	cl_tutor.SetNameAndStatus( L( "GameUI_Tutorials" ), L( "-" ) );
	cl_tutor.iFlags |= QMF_NOTIFY;
	cl_tutor.SetCoord( 300, MenuYOffset + 180 );

	cl_showhealthbars.SetNameAndStatus( L( "GameUI_Healthbars" ), L( "-" ) );
	cl_showhealthbars.iFlags |= QMF_NOTIFY;
	cl_showhealthbars.SetCoord( 300, MenuYOffset + 230 );

	static const char *cl_hitmarker_str[] =
	{
		L( "GameUI_Off" ), L( "GameUI_On" ), L( "GameUI_OnWithSound" )
	};
	static CStringArrayModel hitmarker( cl_hitmarker_str, V_ARRAYSIZE( cl_hitmarker_str ) );
	cl_hitmarker.SetNameAndStatus( L( "GameUI_Hitmarker" ), L( "-" ) );
	cl_hitmarker.Setup( &hitmarker );
	cl_hitmarker.onChanged = CMenuEditable::WriteCvarCb;
	cl_hitmarker.font = QM_SMALLFONT;
	cl_hitmarker.SetRect( 300, MenuYOffset + 305, 300, 32 );

	cl_useicon.SetNameAndStatus( L( "GameUI_InteractionIcon" ), L( "-" ) );
	cl_useicon.iFlags |= QMF_NOTIFY;
	cl_useicon.SetCoord( 300, MenuYOffset + 345 );

	static const char *cl_crosshair_str[] =
	{
		L( "GameUI_Off" ), L( "GameUI_Diffusion" ), L( "GameUI_Simple" )
	};
	static CStringArrayModel crossy( cl_crosshair_str, V_ARRAYSIZE( cl_crosshair_str ) );
	cl_crosshair.SetNameAndStatus( L( "GameUI_CrosshairStyle" ), L( "Set crosshair style" ) );
	cl_crosshair.Setup( &crossy );
	cl_crosshair.onChanged = CMenuEditable::WriteCvarCb;
	cl_crosshair.font = QM_SMALLFONT;
	cl_crosshair.SetRect( 300, MenuYOffset + 420, 200, 32 );

	static const char *cl_largehud_str[] =
	{
		L( "GameUI_No" ), L( "GameUI_Yes" ), L( "GameUI_Auto" )
	};
	static CStringArrayModel largehud( cl_largehud_str, V_ARRAYSIZE( cl_largehud_str ) );
	cl_largehud.SetNameAndStatus( L( "GameUI_LargeHUD" ), L( "Set crosshair style" ) );
	cl_largehud.Setup( &largehud );
	cl_largehud.onChanged = CMenuEditable::WriteCvarCb;
	cl_largehud.font = QM_SMALLFONT;
	cl_largehud.SetRect( 520, MenuYOffset + 420, 200, 32 );

	cl_centerhud.SetNameAndStatus( L( "GameUI_CenterHUD" ), L( "-" ) );
	cl_centerhud.iFlags |= QMF_NOTIFY;
	cl_centerhud.SetCoord( 730, MenuYOffset + 420 );

	cl_viewmodel_extras.SetCoord( 300, MenuYOffset + 510 );
	cl_viewmodel_extras.SetNameAndStatus( L( "GameUI_WeaponAnimInfluence" ), L( "-" ) );
	cl_viewmodel_extras.Setup( 0.0f, 1.0f, 0.1f );
	cl_viewmodel_extras.SetSize( 400, 15 );

	allowConsole.SetNameAndStatus( L( "GameUI_EnableConsole" ), L( "-" ) );
	allowConsole.SetCoord( 300, MenuYOffset + 540 );
	allowConsole.onChanged.SetCommand( false, "ui_allowconsole\n" );

	AddItem( default_fov );
	AddItem( cl_subtitles );
	AddItem( cl_achievement_notify );
	AddItem( cl_tutor );
	AddItem( cl_showhealthbars );
	AddItem( cl_hitmarker );
	AddItem( cl_useicon );
	AddItem( cl_crosshair );
	AddItem( cl_largehud );
	AddItem( cl_centerhud );
	AddItem( allowConsole );
	AddItem( cl_viewmodel_extras );
}

void CMenuOptions::Show(void)
{
	CMenuFramework::Show();
	EngFuncs::ClientCmd(false, "menuactivate options\n");
}

void CMenuOptions::Hide(void)
{
	default_fov.WriteCvar();
	cl_subtitles.WriteCvar();
	cl_achievement_notify.WriteCvar();
	cl_tutor.WriteCvar();
	cl_showhealthbars.WriteCvar();
	cl_centerhud.WriteCvar();
	cl_hitmarker.WriteCvar();
	cl_useicon.WriteCvar();
	EngFuncs::CvarSetValue( "cl_viewmodel_extras", cl_viewmodel_extras.GetCurrentValue() );

	CMenuFramework::Hide();
	EngFuncs::ClientCmd(false, "menuactivate mainmenu\n");
}

void CMenuOptions::GetConfig()
{
	default_fov.LinkCvar( "default_fov", CMenuEditable::CVAR_VALUE );
	cl_subtitles.LinkCvar( "cl_subtitles" );
	cl_achievement_notify.LinkCvar( "cl_achievement_notify" );
	cl_tutor.LinkCvar( "cl_tutor" );
	cl_showhealthbars.LinkCvar( "cl_showhealthbars" );
	cl_centerhud.LinkCvar( "cl_centerhud" );
	cl_hitmarker.LinkCvar( "cl_hitmarker", CMenuEditable::CVAR_VALUE );
	cl_useicon.LinkCvar( "cl_useicon" );
	cl_crosshair.LinkCvar( "cl_crosshair", CMenuEditable::CVAR_VALUE );
	cl_largehud.LinkCvar( "cl_largehud", CMenuEditable::CVAR_VALUE );
	cl_viewmodel_extras.SetCurrentValue( EngFuncs::GetCvarFloat( "cl_viewmodel_extras" ) );
}

void CMenuOptions::_VidInit()
{
	outlineWidth = 2;
	UI_ScaleCoords( NULL, NULL, &outlineWidth, NULL );
	GetConfig();
}

ADD_MENU( menu_options, CMenuOptions, UI_Options_Menu );
