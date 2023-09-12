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
//	void SaveAndPopMenu() override;
	void GetConfig( void );
	int	outlineWidth;

	CMenuCheckBox cl_achievement_notify;
	CMenuCheckBox cl_tutor;
	CMenuCheckBox cl_showhealthbars;
	CMenuCheckBox cl_hitsound;

	// update dialog
	CMenuYesNoMessageBox msgBox;
};

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

	AddItem( background );
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

	cl_achievement_notify.SetNameAndStatus( L( "Enable achievement notifications" ), L( "-" ) );
	cl_achievement_notify.iFlags |= QMF_NOTIFY;
	cl_achievement_notify.SetCoord( 300, MenuYOffset + 230 );

	cl_tutor.SetNameAndStatus( L( "Enable tutorials" ), L( "-" ) );
	cl_tutor.iFlags |= QMF_NOTIFY;
	cl_tutor.SetCoord( 300, MenuYOffset + 280 );

	cl_showhealthbars.SetNameAndStatus( L( "Enable healthbars" ), L( "-" ) );
	cl_showhealthbars.iFlags |= QMF_NOTIFY;
	cl_showhealthbars.SetCoord( 300, MenuYOffset + 330 );

	cl_hitsound.SetNameAndStatus( L( "Hitmark sound" ), L( "-" ) );
	cl_hitsound.iFlags |= QMF_NOTIFY;
	cl_hitsound.SetCoord( 300, MenuYOffset + 380 );

	AddItem( cl_achievement_notify );
	AddItem( cl_tutor );
	AddItem( cl_showhealthbars );
	AddItem( cl_hitsound );
}

void CMenuOptions::Show(void)
{
	CMenuFramework::Show();
	EngFuncs::ClientCmd(FALSE, "menuactivate options\n");
}

void CMenuOptions::Hide(void)
{
	cl_achievement_notify.WriteCvar();
	cl_tutor.WriteCvar();
	cl_showhealthbars.WriteCvar();
	cl_hitsound.WriteCvar();

	CMenuFramework::Hide();
	EngFuncs::ClientCmd(FALSE, "menuactivate mainmenu\n");
}

void CMenuOptions::GetConfig()
{
	cl_achievement_notify.LinkCvar( "cl_achievement_notify" );
	cl_tutor.LinkCvar( "cl_tutor" );
	cl_showhealthbars.LinkCvar( "cl_showhealthbars" );
	cl_hitsound.LinkCvar( "cl_hitsound" );
}

void CMenuOptions::_VidInit()
{
	outlineWidth = 2;
	UI_ScaleCoords( NULL, NULL, &outlineWidth, NULL );
	GetConfig();
}

ADD_MENU( menu_options, CMenuOptions, UI_Options_Menu );
