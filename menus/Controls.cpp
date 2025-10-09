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
#include "PicButton.h"
#include "Action.h"
#include "YesNoMessageBox.h"
#include "MessageBox.h"
#include "Table.h"
#include "CheckBox.h"
#include "Slider.h"
#include "kbutton.h"
#include "Field.h"

#define ART_BANNER		"gfx/shell/head_controls"
#define MAX_KEYS 256

class CMenuControls;

class CMenuKeysModel : public CMenuBaseModel
{
public:
	CMenuKeysModel( CMenuControls *parent ) : parent( parent ) { }

	void Update();
	void OnActivateEntry( int line );
	void OnDeleteEntry( int line );
	int GetRows() const
	{
		return m_iNumItems;
	}
	int GetColumns() const
	{
		return 3; // cmd, key1, key2
	}
	unsigned int GetAlignmentForColumn( int column ) const override
	{
		if( column == 1 || column == 2 )
			return QM_CENTER;
		return QM_LEFT;
	}
	const char *GetCellText( int line, int column )
	{
		switch( column )
		{
		case 0: return name[line];
		case 1: return firstKey[line];
		case 2: return secondKey[line];
		}

		return NULL;
	}

	bool IsCellTextWrapped( int line, int column )
	{
		return IsLineUsable( line );
	}

	bool IsLineUsable( int line )
	{
		return keysBind[line][0] != 0;
	}

	char name[MAX_KEYS][128];
	char keysBind[MAX_KEYS][128];
	char firstKey[MAX_KEYS][20];
	char secondKey[MAX_KEYS][20];
	int m_iNumItems;
private:
	CMenuControls *parent;
};

class CMenuControls : public CMenuFramework
{
public:
	CMenuControls() : CMenuFramework("CMenuControls"), keysListModel( this ) { }

	void _Init();
	void _VidInit();
	void EnterGrabMode( void );
	void UnbindEntry( void );
	static void GetKeyBindings( const char *command, int *twoKeys );
	void GetConfig( void );
	void PitchInvert( void );
	void SaveAndPopMenu( void );
	void SetSensitivity( void );

	// state toggle by
	CMenuTable keysList;
	CMenuKeysModel keysListModel;

private:
	void UnbindCommand( const char *command );
	void ResetKeysList( void );
	void Cancel( void )
	{
		EngFuncs::ClientCmd( true, "exec keyboard\n" );
		Hide();
	}

	// redefine key wait dialog
	class CGrabKeyMessageBox : public CMenuMessageBox
	{
	public:
		bool KeyUp( int key ) override;
		bool KeyDown( int key ) override;
	} msgBox1; // small msgbox

	CMenuYesNoMessageBox msgBox2; // large msgbox

	// diffusion - removed Advanced controls and moved this here.
	CMenuCheckBox	lookFilter;
	CMenuCheckBox	invertMouse;
	CMenuCheckBox	mouseLook;
	CMenuSlider	sensitivity;
	CMenuField sens2;
};

/*
=================
UI_Controls_GetKeyBindings
=================
*/
void CMenuControls::GetKeyBindings( const char *command, int *twoKeys )
{
	twoKeys[0] = twoKeys[1] = -1;

	for( int i = 0, count = 0; i < MAX_KEYS; i++ )
	{
		const char *b = EngFuncs::KEY_GetBinding( i );
		if( !b ) continue;

		if( !stricmp( command, b ))
		{
			twoKeys[count] = i;
			count++;

			if( count == 2 ) break;
		}
	}

	// swap keys if needed
	if( twoKeys[0] != -1 && twoKeys[1] != -1 )
	{
		int tempKey = twoKeys[1];
		twoKeys[1] = twoKeys[0];
		twoKeys[0] = tempKey;
	}
}

void CMenuControls::UnbindCommand( const char *command )
{
	int i, l;
	const char *b;

	l = strlen( command );

	for( i = 0; i < MAX_KEYS; i++ )
	{
		b = EngFuncs::KEY_GetBinding( i );
		if( !b ) continue;

		if( !strncmp( b, command, l ))
			EngFuncs::KEY_SetBinding( i, "" );
	}
}

void CMenuKeysModel::Update( void )
{
	char *afile = (char *)EngFuncs::COM_LoadFile( "data/kb_act.lst", NULL );
	char *pfile = afile;
	char token[1024];
	int i = 0;

	if( !afile )
	{
		m_iNumItems = 0;

		Con_Printf( "UI_Parse_KeysList: kb_act.lst not found\n" );
		return;
	}

	memset( keysBind, 0, sizeof( keysBind ));
	memset( firstKey, 0, sizeof( firstKey ));
	memset( secondKey, 0, sizeof( secondKey ));

	while( (pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) )) != NULL )
	{
		if( !stricmp( token, "blank" ))
		{
			// separator
			pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );
			if( !pfile ) break;	// technically an error

			if( token[0] == '#' )
				snprintf( name[i], sizeof( name[i] ), "^6%s^7", L( token ));
			else
				snprintf( name[i], sizeof( name[i] ), "^6%s^7", token );

			keysBind[i][0] = firstKey[i][0] = secondKey[i][0] = 0;
			i++;
		}
		else
		{
			// key definition
			int	keys[2];

			CMenuControls::GetKeyBindings( token, keys );
			Q_strncpy( keysBind[i], token, sizeof( keysBind[i] ));

			pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );
			if( !pfile ) break; // technically an error

			if( token[0] == '#' )
				snprintf( name[i], sizeof( name[i] ), "^6%s^7", L( token ));
			else
				snprintf( name[i], sizeof( name[i] ), "^6%s^7", token );

			if( keys[0] != -1 )
			{
				const char *str = EngFuncs::KeynumToString( keys[0] );

				if( str )
				{
				//	if( !strnicmp( str, "MOUSE", 5 ) )
				//		snprintf( firstKey[i], 20, "^5%s^7", str );
				//	else
						snprintf( firstKey[i], 20, "^5%s^7", str );

					// diffusion - convert to uppercase
					firstKey[i][2] = toupper( firstKey[i][2] );
				}
				else firstKey[i][0] = 0;
			}

			if( keys[1] != -1 )
			{
				const char *str = EngFuncs::KeynumToString( keys[1] );

				if( str )
				{
				//	if( !strnicmp( str, "MOUSE", 5 ) )
				//		snprintf( secondKey[i], 20, "^5%s^7", str );
				//	else
						snprintf( secondKey[i], 20, "^5%s^7", str );

					// diffusion - convert to uppercase
					secondKey[i][2] = toupper( secondKey[i][2] );
				}
				else secondKey[i][0] = 0;
			}

			i++;
		}
	}

	m_iNumItems = i;

	EngFuncs::COM_FreeFile( afile );
}

void CMenuKeysModel::OnActivateEntry(int line)
{
	parent->EnterGrabMode();
}

void CMenuKeysModel::OnDeleteEntry(int line)
{
	parent->UnbindEntry();
}

void CMenuControls::ResetKeysList( void )
{
	char *afile = (char *)EngFuncs::COM_LoadFile( "data/kb_def.lst", NULL );
	char *pfile = afile;
	char token[1024];

	if( !afile )
	{
		Con_Printf( "UI_Parse_KeysList: kb_act.lst not found\n" );
		return;
	}

	EngFuncs::ClientCmd( true, "unbindall" );

	while( (pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) )) != NULL )
	{
		char	key[32];

		Q_strncpy( key, token, sizeof( key ));

		pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );
		if( !pfile ) break;	// technically an error

		char	cmd[4096];

		if( key[0] == '\\' && key[1] == '\\' )
		{
			key[0] = '\\';
			key[1] = '\0';
		}

		snprintf( cmd, sizeof( cmd ), "bind \"%s\" \"%s\"\n", key, token );
		EngFuncs::ClientCmd( true, cmd );
	}

	EngFuncs::COM_FreeFile( afile );
	keysListModel.Update();
}

bool CMenuControls::CGrabKeyMessageBox::KeyUp( int key )
{
	EUISounds sound;
	CMenuControls *parent = ((CMenuControls*)m_pParent);

	// defining a key
	// escape is special, should allow rebind all keys on gamepad
	if( UI::Key::IsConsole( key ) || key == K_ESCAPE )
	{
		sound = SND_BUZZ;
	}
	else
	{
		char cmd[4096];

		const char *bindName = parent->keysListModel.keysBind[parent->keysList.GetCurrentIndex()];
		snprintf( cmd, sizeof( cmd ), "bind \"%s\" \"%s\"\n", EngFuncs::KeynumToString( key ), bindName );
		EngFuncs::ClientCmd( true, cmd );
		sound = SND_LAUNCH;
	}

	parent->keysListModel.Update();

	Hide();

	PlayLocalSound( uiStatic.sounds[sound] );

	return true;
}

bool CMenuControls::CGrabKeyMessageBox::KeyDown( int key )
{
	return true;
}

void CMenuControls::UnbindEntry()
{
	if( !keysListModel.IsLineUsable( keysList.GetCurrentIndex() ) )
	{
		PlayLocalSound( uiStatic.sounds[SND_BUZZ] );
		return; // not a key
	}

	const char *bindName = keysListModel.keysBind[keysList.GetCurrentIndex()];

	UnbindCommand( bindName );
	PlayLocalSound( uiStatic.sounds[SND_REMOVEKEY] );
	keysListModel.Update();

	// disabled: left command just unbinded
	// msgBox1.Show();
}

void CMenuControls::EnterGrabMode()
{
	if( !keysListModel.IsLineUsable( keysList.GetCurrentIndex() ) )
	{
		PlayLocalSound( uiStatic.sounds[SND_REMOVEKEY] );
		return;
	}

	// entering to grab-mode
	const char *bindName = keysListModel.keysBind[keysList.GetCurrentIndex()];

	int keys[2];

	GetKeyBindings( bindName, keys );
	if( keys[1] != -1 )
		UnbindCommand( bindName );

	msgBox1.Show();

	PlayLocalSound( uiStatic.sounds[SND_KEY] );
}

/*
=================
UI_Controls_Init
=================
*/
void CMenuControls::_Init( void )
{
	banner.SetPicture( ART_BANNER );

	keysList.SetRect( 460, 190, 1000, 720 ); // 460 = (1920(ui width) - 1000(table width)) / 2
	keysList.SetModel( &keysListModel );
	keysList.SetupColumn( 0, L( "GameUI_Action" ), 0.50f );
	keysList.SetupColumn( 1, L( "GameUI_KeyButton" ), 0.25f );
	keysList.SetupColumn( 2, L( "GameUI_Alternate" ), 0.25f );
	keysList.charSize += 5; // increase distance between lines

	msgBox1.SetMessage( L( "Press a key or button" ) );
	msgBox1.Link( this );

	msgBox2.SetMessage( L( "GameUI_KeyboardSettingsText" ) );
	msgBox2.onPositive = VoidCb( &CMenuControls::ResetKeysList );
	msgBox2.Link( this );

	invertMouse.SetNameAndStatus( L( "GameUI_ReverseMouse" ), L( "GameUI_ReverseMouseLabel" ) );
	invertMouse.iFlags |= QMF_NOTIFY;
	invertMouse.onChanged = VoidCb( &CMenuControls::PitchInvert );
	invertMouse.SetCoord( 72, 300 );

	mouseLook.SetNameAndStatus( L( "GameUI_MouseLook" ), L( "GameUI_MouseLookLabel" ) );
	mouseLook.iFlags |= QMF_NOTIFY;
	mouseLook.SetCoord( 72, 350 );

	lookFilter.SetNameAndStatus( L( "GameUI_MouseFilter" ), L( "GameUI_MouseFilterLabel" ) );
	lookFilter.iFlags |= QMF_NOTIFY;
	lookFilter.SetCoord( 72, 400 );

	sensitivity.SetNameAndStatus( L( "GameUI_MouseSensitivity" ), L( "Set in-game mouse sensitivity" ) );
	sensitivity.Setup( 0.1f, 20.0f, 0.1f );
	sensitivity.SetCoord( 72, 500 );
	sensitivity.SetSize( 250, 0 );
	sensitivity.onChanged = VoidCb( &CMenuControls::SetSensitivity );

	sens2.bNumbersOnly = true;
	sens2.SetRect( 72, sensitivity.pos.y + 30, 80, 32 );
	sens2.bDisplayOnly = true;

	AddItem( banner );

	CMenuPicButton *UseDefaults = AddButton( L( "GameUI_UseDefaults" ), L( "Are you sure to load default key values?" ), PC_USE_DEFAULTS, msgBox2.MakeOpenEvent() );
	UseDefaults->pos.y = 700;

//	AddButton( L( "Adv. Controls" ), L( "Change mouse sensitivity, enable autoaim, mouselook and crosshair" ), PC_ADV_CONTROLS, UI_AdvControls_Menu );

	CMenuPicButton *Done = AddButton( L( "GameUI_OK" ), L( "Save changed and return to configuration menu" ), PC_OK, VoidCb( &CMenuControls::SaveAndPopMenu ) );
	Done->pos.y = 800;

//	CMenuPicButton *Cancel = AddButton( L( "GameUI_Cancel" ), L( "Discard changes and return to configuration menu" ), PC_CANCEL, VoidCb( &CMenuControls::Cancel ) );
//	Cancel->pos.y = 850;

	AddItem( keysList );

	AddItem( invertMouse );
	AddItem( mouseLook );
	AddItem( lookFilter );
	AddItem( sensitivity );
	AddItem( sens2 );

	CMenuPicButton *Joystick = AddButton( L( "GameUI_Joystick" ), L( "Change gamepad axis and button settings" ), PC_GAMEPAD, UI_GamePad_Menu, QMF_NOTIFY );
	Joystick->pos.y = 650;
}

void CMenuControls::SetSensitivity( void )
{
	sensitivity.WriteCvar();

	float flSens = EngFuncs::GetCvarFloat( "sensitivity" );
	sensitivity.SetCurrentValue( flSens );

	char m_szValue[32];
	snprintf( m_szValue, sizeof( m_szValue ), "%.*f", 1, flSens );
	sens2.SetBuffer( m_szValue );
}

void CMenuControls::_VidInit()
{
	msgBox1.SetRect( DLG_X + 192, 256, 640, 128 );
	msgBox1.pos.x += uiStatic.xOffset;
	msgBox1.pos.y += uiStatic.yOffset;

	keysListModel.Update();

	GetConfig();
}

void CMenuControls::GetConfig()
{
	kbutton_t *mlook;

	if( EngFuncs::GetCvarFloat( "m_pitch" ) < 0 )
		invertMouse.bChecked = true;

	mlook = (kbutton_t *)EngFuncs::KEY_GetState( "in_mlook" );
	if( mlook )
	{
		if( mlook && mlook->state & 1 )
			mouseLook.bChecked = true;
		else
			mouseLook.bChecked = false;
	}
	else
	{
		mouseLook.SetGrayed( true );
		mouseLook.bChecked = true;
	}

	lookFilter.LinkCvar( "look_filter" );

	sensitivity.LinkCvar( "sensitivity" );

	SetSensitivity();
}

void CMenuControls::PitchInvert()
{
	bool invert = invertMouse.bChecked;
	float m_pitch = EngFuncs::GetCvarFloat( "m_pitch" );
	if( (invert && (m_pitch > 0)) ||
		(!invert && (m_pitch < 0)) )
	{
		EngFuncs::CvarSetValue( "m_pitch", -m_pitch );
	}
}

void CMenuControls::SaveAndPopMenu()
{
	lookFilter.WriteCvar();
	if( EngFuncs::GetCvarString( "m_filter" )[0] )
		EngFuncs::CvarSetValue( "m_filter", lookFilter.bChecked );

	sensitivity.WriteCvar();

	if( mouseLook.bChecked )
		EngFuncs::ClientCmd( false, "+mlook\nbind _force_write\n" );
	else
		EngFuncs::ClientCmd( false, "-mlook\nbind _force_write\n" );

	CMenuFramework::SaveAndPopMenu();
}

ADD_MENU( menu_controls, CMenuControls, UI_Controls_Menu );
