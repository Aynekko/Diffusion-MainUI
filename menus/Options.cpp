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

	CMenuCheckBox cl_showdamage;
	CMenuCheckBox cl_achievement_notify;
	CMenuCheckBox cl_tutor;
	CMenuCheckBox cl_showhealthbars;
	CMenuCheckBox cl_hitsound;
	CMenuCheckBox cl_useicon;
	CMenuSpinControl cl_crosshair;
	CMenuCheckBox allowConsole;
	CMenuSlider	cl_viewmodel_extras;

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
	CMenuPicButton *controls = AddButton( L( "Controls" ), L( "Change keyboard and mouse settings" ),
		PC_CONTROLS, UI_Controls_Menu, QMF_NOTIFY );
	CMenuPicButton *audio =  AddButton( L( "Audio" ), L( "Change sound volume and quality" ),
		PC_AUDIO, UI_Audio_Menu, QMF_NOTIFY );
	CMenuPicButton *video = AddButton( L( "Video" ), L( "Change screen size, video mode and gamma" ),
		PC_VIDEO, UI_Video_Menu, QMF_NOTIFY );
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
	done->pos.y += 50;

	cl_showdamage.SetNameAndStatus( L( "GameUI_ShowDamage" ), L( "-" ) );
	cl_showdamage.iFlags |= QMF_NOTIFY;
	cl_showdamage.SetCoord( 300, MenuYOffset + 130 );

	cl_achievement_notify.SetNameAndStatus( L( "GameUI_AchievementNotify" ), L( "-" ) );
	cl_achievement_notify.iFlags |= QMF_NOTIFY;
	cl_achievement_notify.SetCoord( 300, MenuYOffset + 180 );

	cl_tutor.SetNameAndStatus( L( "GameUI_Tutorials" ), L( "-" ) );
	cl_tutor.iFlags |= QMF_NOTIFY;
	cl_tutor.SetCoord( 300, MenuYOffset + 230 );

	cl_showhealthbars.SetNameAndStatus( L( "GameUI_Healthbars" ), L( "-" ) );
	cl_showhealthbars.iFlags |= QMF_NOTIFY;
	cl_showhealthbars.SetCoord( 300, MenuYOffset + 280 );

	cl_hitsound.SetNameAndStatus( L( "GameUI_HitmarkSound" ), L( "-" ) );
	cl_hitsound.iFlags |= QMF_NOTIFY;
	cl_hitsound.SetCoord( 300, MenuYOffset + 330 );

	cl_useicon.SetNameAndStatus( L( "GameUI_InteractionIcon" ), L( "-" ) );
	cl_useicon.iFlags |= QMF_NOTIFY;
	cl_useicon.SetCoord( 300, MenuYOffset + 380 );

	static const char *cl_crosshair_str[] =
	{
		L( "GameUI_Off" ), L( "GameUI_Diffusion" ), L( "GameUI_Simple" )
	};
	static CStringArrayModel crossy( cl_crosshair_str, V_ARRAYSIZE( cl_crosshair_str ) );
	cl_crosshair.SetNameAndStatus( L( "GameUI_CrosshairStyle" ), L( "Set crosshair style" ) );
	cl_crosshair.Setup( &crossy );
	cl_crosshair.onChanged = CMenuEditable::WriteCvarCb;
	cl_crosshair.font = QM_SMALLFONT;
	cl_crosshair.SetRect( 300, MenuYOffset + 470, 200, 32 );

	cl_viewmodel_extras.SetCoord( 300, MenuYOffset + 560 );
	cl_viewmodel_extras.SetNameAndStatus( L( "GameUI_WeaponAnimInfluence" ), L( "-" ) );
	cl_viewmodel_extras.Setup( 0.0f, 1.0f, 0.1f );
	cl_viewmodel_extras.SetSize( 400, 15 );

	allowConsole.SetNameAndStatus( L( "GameUI_EnableConsole" ), L( "-" ) );
	allowConsole.SetCoord( 300, MenuYOffset + 590 );
	allowConsole.onChanged.SetCommand( FALSE, "ui_allowconsole\n" );

	AddItem( cl_showdamage );
	AddItem( cl_achievement_notify );
	AddItem( cl_tutor );
	AddItem( cl_showhealthbars );
	AddItem( cl_hitsound );
	AddItem( cl_useicon );
	AddItem( cl_crosshair );
	AddItem( allowConsole );
	AddItem( cl_viewmodel_extras );
}

void CMenuOptions::Show(void)
{
	CMenuFramework::Show();
	EngFuncs::ClientCmd(FALSE, "menuactivate options\n");
}

void CMenuOptions::Hide(void)
{
	cl_showdamage.WriteCvar();
	cl_achievement_notify.WriteCvar();
	cl_tutor.WriteCvar();
	cl_showhealthbars.WriteCvar();
	cl_hitsound.WriteCvar();
	cl_useicon.WriteCvar();
	EngFuncs::CvarSetValue( "cl_viewmodel_extras", cl_viewmodel_extras.GetCurrentValue() );

	CMenuFramework::Hide();
	EngFuncs::ClientCmd(FALSE, "menuactivate mainmenu\n");
}

void CMenuOptions::GetConfig()
{
	cl_showdamage.LinkCvar( "cl_showdamage" );
	cl_achievement_notify.LinkCvar( "cl_achievement_notify" );
	cl_tutor.LinkCvar( "cl_tutor" );
	cl_showhealthbars.LinkCvar( "cl_showhealthbars" );
	cl_hitsound.LinkCvar( "cl_hitsound" );
	cl_useicon.LinkCvar( "cl_useicon" );
	cl_crosshair.LinkCvar( "cl_crosshair", CMenuEditable::CVAR_VALUE );
	cl_viewmodel_extras.SetCurrentValue( EngFuncs::GetCvarFloat( "cl_viewmodel_extras" ) );
}

void CMenuOptions::_VidInit()
{
	outlineWidth = 2;
	UI_ScaleCoords( NULL, NULL, &outlineWidth, NULL );
	GetConfig();
}

ADD_MENU( menu_options, CMenuOptions, UI_Options_Menu );
