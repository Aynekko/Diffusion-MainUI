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
#include "CheckBox.h"
#include "Slider.h"
#include "Field.h"
#include "SpinControl.h"
#include "YesNoMessageBox.h"
#include "PlayerModelView.h"
#include "StringArrayModel.h"
#include "StringVectorModel.h"

#define ART_BANNER		"gfx/shell/head_customize"

#define MAX_PLAYERMODELS	100

static struct
{
	const char *name;
	int r, g, b;
} g_LogoColors[] =
{
{ "FullColor", -1, -1, -1 },
{ "#Valve_Orange", 255, 120, 24  }, // L( "Valve_Orange" )
{ "#Valve_Yellow", 225, 180, 24  }, // L( "Valve_Yellow" )
{ "#Valve_Blue",   0,   60,  255 }, // L( "Valve_Blue" )
{ "#Valve_Ltblue", 0,   167, 255 }, // L( "Valve_Ltblue" )
{ "#Valve_Green",  0,   167, 0   }, // L( "Valve_Green" )
{ "#Valve_Red",    255, 43,  0   }, // L( "Valve_Red" )
{ "#Valve_Brown",  123, 73,  0   }, // L( "Valve_Brown" )
{ "#Valve_Ltgray", 100, 100, 100 }, // L( "Valve_Ltgray" )
{ "#Valve_Dkgray", 36,  36,  36  }, // L( "Valve_Dkgray" )
};

class CMenuPlayerSetup : public CMenuFramework
{
private:
	void _Init() override;
	void Reload() override;
public:
	CMenuPlayerSetup() : CMenuFramework( "CMenuPlayerSetup" ), msgBox( true ) { }

	void SetConfig();
	void UpdateModel();
	void UpdateLogo();
	void ApplyColorToImagePreview();
	void ApplyColorToLogoPreview();
	void WriteNewLogo();
	void SaveAndPopMenu() override;

	class CModelListModel : public CStringArrayModel
	{
	public:
		CModelListModel() : CStringArrayModel( (const char *)models, CS_SIZE, 0 ) {}
		void Update();

	private:
		char models[MAX_PLAYERMODELS][CS_SIZE];
	} modelsModel;

	class CLogosListModel : public CStringVectorModel
	{
	public:
		void Update() override;

		int GetFullPath( char *buf, size_t size, int pos )
		{
			const char *file, *ext;

			file = Element( pos ).String();
			ext = IsPng( pos ) ? "png" : "bmp";

			return snprintf( buf, size, "logos/%s.%s", file, ext );
		}

		bool IsPng( int pos )
		{
			return m_isPngs[pos];
		}

	private:
		CUtlVector<bool> m_isPngs;
	} logosModel;

	CMenuPlayerModelView	view;

	CMenuCheckBox	showModels;
	//	CMenuCheckBox	hiModels;
	CMenuCheckBox voiceEnable;
	CMenuSlider transmitVolume;
	CMenuSlider receiveVolume;
	CMenuAction noProprietaryCodecNotice;
	CMenuSlider	topColor;
	CMenuSlider	bottomColor;

	CMenuField	name;
	CMenuSpinControl	model;

	class CMenuLogoPreview : public CMenuBaseItem
	{
	public:
		virtual void Draw();
		int r, g, b;
		HIMAGE hImage;
	} logoImage;

	CMenuSpinControl	logo;
	CMenuSpinControl	logoColor;

	CMenuYesNoMessageBox msgBox;

	bool hideModels, hideLogos;
};

void CMenuPlayerSetup::CMenuLogoPreview::Draw()
{
	if( !hImage )
	{
		// draw the background
		UI_FillRect( m_scPos, m_scSize, uiPromptBgColor );

		UI_DrawString( font, m_scPos, m_scSize, L( "No logo" ), colorBase, m_scChSize, QM_CENTER, ETF_SHADOW );
	}
	else
	{
		if( r != -1 && g != -1 && b != -1 )
			EngFuncs::PIC_Set( hImage, r, g, b );
		else
			EngFuncs::PIC_Set( hImage, 255, 255, 255 );
		EngFuncs::PIC_DrawTrans( m_scPos, m_scSize );
	}

	int textHeight = m_scPos.y - (m_scChSize * 1.5f);
	uint textflags = (iFlags & QMF_DROPSHADOW) ? ETF_SHADOW : 0;
	UI_DrawString( font, m_scPos.x, textHeight, m_scSize.w, m_scChSize, szName, uiColorHelp, m_scChSize, QM_LEFT, textflags | ETF_FORCECOL );

	// draw the rectangle
	if( eFocusAnimation == QM_HIGHLIGHTIFFOCUS && IsCurrentSelected() )
		UI_DrawRectangle( m_scPos, m_scSize, uiInputTextColor );
	else
		UI_DrawRectangle( m_scPos, m_scSize, uiInputFgColor );

}

/*
=================
UI_PlayerSetup_FindModels
=================
*/
void CMenuPlayerSetup::CModelListModel::Update( void )
{
	char	name[256];
	char **filenames;
	int numFiles, i;

	m_iCount = 0;

	// Get file list
	// search in basedir too, because that's how GoldSrc does this
	filenames = EngFuncs::GetFilesList( "models/player/*", &numFiles, FALSE );

	// build the model list
	for( i = 0; i < numFiles; i++ )
	{
		COM_FileBase( filenames[i], name, sizeof( name ) );
		Q_strncpy( models[m_iCount], name, sizeof( models[0] ) );

		// check if the path is a valid model
		snprintf( name, sizeof( name ), "models/player/%s/%s.mdl", models[m_iCount], models[m_iCount] );
		if( !EngFuncs::FileExists( name ) )
			continue;

		m_iCount++;
	}
}

/*
=================
CMenuPlayerSetup::FindLogos

=================
*/
void CMenuPlayerSetup::CLogosListModel::Update()
{
	char **filenames;
	int numFiles, i;

	// Get file list
	filenames = EngFuncs::GetFilesList( "logos/*.*", &numFiles, FALSE );

	if( !filenames || !numFiles )
	{
		m_isPngs.RemoveAll();
		RemoveAll();
		return;
	}

	// build the model list
	for( i = 0; i < numFiles; i++ )
	{
		CUtlString logoFileName = filenames[i];
		char temp[256];
		bool png;

		if( (png = logoFileName.BEndsWithCaseless( ".png" )) ||
			logoFileName.BEndsWithCaseless( ".bmp" ) )
		{
			COM_FileBase( logoFileName.String(), temp, sizeof( temp ) );

			if( !stricmp( temp, "remapped" ) )
				continue;

			AddToTail( temp );
			m_isPngs.AddToTail( png );
		}
	}
}

/*
=================
UI_PlayerSetup_SetConfig
=================
*/
void CMenuPlayerSetup::SetConfig( void )
{
	name.WriteCvar();
	model.WriteCvar();
	topColor.WriteCvar();
	bottomColor.WriteCvar();
	//	hiModels.WriteCvar();
	showModels.WriteCvar();
	WriteNewLogo();
}

void CMenuPlayerSetup::SaveAndPopMenu()
{
	// diffusion - disable
//	if( !UI::Names::CheckIsNameValid( name.GetBuffer() ) )
//	{
//		msgBox.Show();
//		return;
//	}

	SetConfig();
	CMenuFramework::SaveAndPopMenu();
}

void CMenuPlayerSetup::UpdateModel()
{
	char image[256];
	const char *mdl = model.GetCurrentString();

	// seems we DON'T have this model locally
	// just force display string and do nothing
	if( !mdl )
	{
		model.ForceDisplayString( EngFuncs::GetCvarString( "model" ) );
		return;
	}

	snprintf( image, sizeof( image ), "models/player/%s/%s.bmp", mdl, mdl );
	view.hPlayerImage = EngFuncs::PIC_Load( image, PIC_KEEP_SOURCE );

	ApplyColorToImagePreview();
	EngFuncs::CvarSetString( "model", mdl );
	if( !strcmp( mdl, "player" ) )
		strcpy( image, "models/player.mdl" );
	else
		snprintf( image, sizeof( image ), "models/player/%s/%s.mdl", mdl, mdl );
	if( view.ent )
		EngFuncs::SetModel( view.ent, image );
}

void CMenuPlayerSetup::UpdateLogo()
{
	const int pos = logo.GetCurrentValue();

	logoImage.r = g_LogoColors[0].r;
	logoImage.g = g_LogoColors[0].g;
	logoImage.b = g_LogoColors[0].b;
	logoColor.SetCurrentValue( L( g_LogoColors[0].name ) );
	logoColor.SetGrayed( true );

	if( pos < 0 )
	{
		logoImage.hImage = 0;
		return;
	}

	char filename[1024];
	const int temp = logosModel.GetFullPath( filename, sizeof( filename ), pos );
	if( (temp < 0) || (temp > sizeof( filename )) )
	{
		logoImage.hImage = 0;
		return;
	}

	logoImage.hImage = EngFuncs::PIC_Load( filename, 0 );

	if( !logosModel.IsPng( pos ) )
	{
		CBMP *bmpFile = CBMP::LoadFile( filename );
		if( bmpFile->GetBitmapHdr()->bitsPerPixel == 8 )
		{
			ApplyColorToLogoPreview();
			logoColor.SetGrayed( false );
		}

		delete bmpFile;
	}

	EngFuncs::CvarSetString( "cl_logofile", logo.GetCurrentString() );
	logoColor.WriteCvar();
}

void CMenuPlayerSetup::ApplyColorToImagePreview()
{
	EngFuncs::ProcessImage( view.hPlayerImage, -1,
		topColor.GetCurrentValue(), bottomColor.GetCurrentValue() );
}

void CMenuPlayerSetup::ApplyColorToLogoPreview()
{
	const char *logoColorStr = logoColor.GetCurrentString();

	for( size_t i = 0; i < V_ARRAYSIZE( g_LogoColors ) && logoColorStr; i++ )
	{
		if( !stricmp( logoColorStr, L( g_LogoColors[i].name ) ) )
		{
			logoImage.r = g_LogoColors[i].r;
			logoImage.g = g_LogoColors[i].g;
			logoImage.b = g_LogoColors[i].b;
			return;
		}
	}

	logoColor.SetCurrentValue( L( g_LogoColors[0].name ) );
	logoImage.r = g_LogoColors[0].r;
	logoImage.g = g_LogoColors[0].g;
	logoImage.b = g_LogoColors[0].b;
}

void CMenuPlayerSetup::WriteNewLogo( void )
{
	char filename[1024];
	int pos = logo.GetCurrentValue();

	if( pos < 0 || hideLogos )
		return;

	EngFuncs::DeleteFile( "logos/remapped.png" );
	EngFuncs::DeleteFile( "logos/remapped.bmp" );

	logosModel.GetFullPath( filename, sizeof( filename ), pos );

	// TODO: check file size and throw a messagebox if it's too big?
	if( logosModel.IsPng( pos ) )
	{
		int len;
		void *afile = EngFuncs::COM_LoadFile( filename, &len );

		// just copy file, nothing special
		EngFuncs::COM_SaveFile( "logos/remapped.png", afile, len );

		EngFuncs::COM_FreeFile( afile );

		EngFuncs::CvarSetString( "cl_logoext", "png" );
	}
	else
	{
		CBMP *bmpFile = CBMP::LoadFile( filename );

		// not valid logo BMP file
		if( !bmpFile )
			return;

		// remap logo if needed
		if( logoImage.r != -1 && logoImage.g != -1 && logoImage.b != -1 )
			bmpFile->RemapLogo( logoImage.r, logoImage.g, logoImage.b );

		EngFuncs::COM_SaveFile( "logos/remapped.bmp", bmpFile->GetBitmap(), bmpFile->GetBitmapHdr()->fileSize );
		EngFuncs::CvarSetString( "cl_logoext", "bmp" );

		delete bmpFile;
	}

	logo.WriteCvar();
	logoColor.WriteCvar();
}

/*
=================
UI_PlayerSetup_Init
=================
*/
void CMenuPlayerSetup::_Init( void )
{
	int addFlags = 0;

	int MenuXOffset = 250;

	hideModels = hideLogos = false;

	// disable playermodel preview for HLRally to prevent crash
	if( !stricmp( gMenu.m_gameinfo.gamefolder, "hlrally" ) )
		hideModels = true;

	if( gMenu.m_gameinfo.flags & GFL_NOMODELS )
		addFlags |= QMF_INACTIVE;

	banner.SetPicture( ART_BANNER );

	name.szStatusText = L( "Enter your multiplayer nickname" );
	name.iMaxLength = 32;
	name.LinkCvar( "name" );
	name.SetRect( 320, 260, 300, 36 );

	modelsModel.Update();
	if( !modelsModel.GetRows() )
	{
		model.SetVisibility( false );
		hideModels = true;
	}
	else
	{
		model.Setup( &modelsModel );
		model.LinkCvar( "model", CMenuEditable::CVAR_STRING );
		model.onChanged = VoidCb( &CMenuPlayerSetup::UpdateModel );
		model.SetRect( MenuXOffset + 660, 200 + UI_OUTLINE_WIDTH, 260, 32 );
	}

	topColor.iFlags |= addFlags;
	topColor.szName = L( "Colors" );
	topColor.Setup( 0, 255, 1 );
	topColor.LinkCvar( "topcolor" );
	topColor.onCvarChange = CMenuEditable::WriteCvarCb;
	topColor.onChanged = VoidCb( &CMenuPlayerSetup::ApplyColorToImagePreview );
	topColor.SetCoord( MenuXOffset + 340, 520 );
	topColor.size.w = 260;

	bottomColor.iFlags |= addFlags;
	bottomColor.Setup( 0, 255, 1 );
	bottomColor.LinkCvar( "bottomcolor" );
	bottomColor.onCvarChange = CMenuEditable::WriteCvarCb;
	bottomColor.onChanged = VoidCb( &CMenuPlayerSetup::ApplyColorToImagePreview );;
	bottomColor.SetCoord( MenuXOffset + 340, 560 );
	bottomColor.size.w = 260;

	showModels.iFlags |= addFlags;
	showModels.SetNameAndStatus( L( "Show 3D preview" ), L( "Show 3D player models instead of preview thumbnails" ) );
	showModels.LinkCvar( "ui_showmodels" );
	showModels.onCvarChange = CMenuEditable::WriteCvarCb;
	showModels.SetCoord( MenuXOffset + 340, 380 );

	//	hiModels.iFlags |= addFlags;
	//	hiModels.SetNameAndStatus( L( "HD models" ), L( "Show HD models in multiplayer" ) );
	//	hiModels.LinkCvar( "cl_himodels" );
	//	hiModels.onCvarChange = CMenuEditable::WriteCvarCb;
	//	hiModels.SetCoord(MenuXOffset+340, 430 );

	view.iFlags |= addFlags;
	view.SetRect( MenuXOffset + 660, 260, 520, 640 );

	msgBox.SetMessage( L( "Please, choose another player name" ) );
	msgBox.Link( this );

	AddItem( banner );

	AddButton( L( "Done" ), L( "Go back to the Multiplayer Menu" ), PC_DONE, VoidCb( &CMenuPlayerSetup::SaveAndPopMenu ) );
	CMenuPicButton *gameOpt = AddButton( L( "Game options" ), L( "Configure handness, fov and other advanced options" ), PC_GAME_OPTIONS );
	SET_EVENT_MULTI( gameOpt->onReleased,
		{
			((CMenuPlayerSetup *)pSelf->Parent())->SetConfig();
			UI_AdvUserOptions_Menu();
		} );

	AddButton( L( "Adv. Options" ), "", PC_ADV_OPT, UI_GameOptions_Menu );
	gameOpt->SetGrayed( !UI_AdvUserOptions_IsAvailable() );


	if( !hideLogos )
	{
		logosModel.Update();
		if( !logosModel.GetRows() )
		{
			// don't add to framework
			hideLogos = true;
		}
		else
		{
			static const char *itemlist[V_ARRAYSIZE( g_LogoColors )];
			static CStringArrayModel colors( itemlist, V_ARRAYSIZE( g_LogoColors ) );
			for( size_t i = 0; i < V_ARRAYSIZE( g_LogoColors ); i++ )
				itemlist[i] = L( g_LogoColors[i].name );

			logoImage.SetRect( MenuXOffset + 72, 230 + m_iBtnsNum * 50 + 10, 200, 200 );

			logo.Setup( &logosModel );
			logo.LinkCvar( "cl_logofile", CMenuEditable::CVAR_STRING );
			logo.onChanged = VoidCb( &CMenuPlayerSetup::UpdateLogo );
			logo.SetRect( MenuXOffset + 72, logoImage.pos.y + logoImage.size.h + UI_OUTLINE_WIDTH, 200, 32 );

			logoColor.Setup( &colors );
			logoColor.LinkCvar( "cl_logocolor", CMenuEditable::CVAR_STRING );
			logoColor.onChanged = VoidCb( &CMenuPlayerSetup::ApplyColorToLogoPreview );;
			logoColor.SetRect( MenuXOffset + 72, logo.pos.y + logo.size.h + UI_OUTLINE_WIDTH, 200, 32 );
		}
	}

	voiceEnable.szName = L( "GameUI_EnableVoice" );
	voiceEnable.onCvarChange = CMenuEditable::WriteCvarCb;
	voiceEnable.LinkCvar( "voice_modenable" ); // unlike engine's voice_enable, this is synchronized with server
	voiceEnable.SetCoord( logo.pos.x, logo.pos.y + 90 );
	transmitVolume.szName = L( "GameUI_VoiceTransmitVolume" );
	transmitVolume.onCvarChange = CMenuEditable::WriteCvarCb;
	transmitVolume.Setup( 0, 1, 0.05f );
	transmitVolume.LinkCvar( "voice_transmit_scale" );
	transmitVolume.SetCoord( logo.pos.x, voiceEnable.pos.y + 100 );
	transmitVolume.size.w = 300;
	receiveVolume.szName = L( "GameUI_VoiceReceiveVolume" );
	receiveVolume.onCvarChange = CMenuEditable::WriteCvarCb;
	receiveVolume.Setup( 0, 1, 0.05f );
	receiveVolume.LinkCvar( "voice_scale" );
	receiveVolume.SetCoord( logo.pos.x, transmitVolume.pos.y + 50 );
	receiveVolume.size.w = 300;
	noProprietaryCodecNotice.szName = L( "* Uses Opus Codec.\nOpen, royalty-free, highly versatile audio codec." );
	noProprietaryCodecNotice.colorBase = uiColorHelp;
	noProprietaryCodecNotice.SetCharSize( QM_SMALLFONT );
	noProprietaryCodecNotice.SetRect( logo.pos.x, receiveVolume.pos.y + 30, 400, 100 );

	AddItem( name );
	AddItem( voiceEnable );
	AddItem( transmitVolume );
	AddItem( receiveVolume );
	AddItem( noProprietaryCodecNotice );

	if( !hideLogos )
	{
		UpdateLogo();
		AddItem( logo );
		AddItem( logoColor );
		AddItem( logoImage );
	}

	if( !(gMenu.m_gameinfo.flags & GFL_NOMODELS) )
	{
		AddItem( topColor );
		AddItem( bottomColor );
		AddItem( showModels );
		//		AddItem( hiModels );
		AddItem( model );
		// disable playermodel preview for HLRally to prevent crash
		if( !hideModels )
		{
			AddItem( view );
		}
	}
}

void CMenuPlayerSetup::Reload()
{
	if( !hideLogos ) UpdateLogo();
	if( !hideModels ) UpdateModel();
}

ADD_MENU( menu_playersetup, CMenuPlayerSetup, UI_PlayerSetup_Menu );
