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
#include "Table.h"

#define ART_BANNER	     	"gfx/shell/head_achievements"

enum Achievements_e
{
	ACH_BULLETSFIRED = 0,	// 0 fire # bullets
	ACH_JUMPS,				// 1 jump # times
	ACH_AMMOCRATES,			// 2 find # ammo crates
	ACH_DISARMEDMINES,		// 3 disarm # enemy mines (disarming your own doesn't count)
	ACH_KILLENEMIES,		// 4 kill # enemies
	ACH_INFLICTDAMAGE,		// 5 inflict a total of # damage
	ACH_KILLENEMIESSNIPER,	// 6 kill # enemies with a stationary sniper rifle (func_tank)
	ACH_CH1,				// 7 complete chapter
	ACH_CH2,				// 8
	ACH_CH3,				// 9
	ACH_CH4,				// 10
	ACH_CH5,				// 11
	ACH_GENERAL30SEC,		// 12 kill security general under 30 sec
	ACH_HPREGENERATE,		// 13 regenerate a total of # health
	ACH_RECEIVEDAMAGE,		// 14 receive a total of # damage
	ACH_OVERCOOK,			// 15 overcook the grenade
	ACH_DRONESEC,			// 16 kill # security drones
	ACH_DRONEALIEN,			// 17 kill # alien drones
	ACH_CROSSBOW,			// 18 kill # enemies on a certain distance with a crossbow
	ACH_TANKBALL,			// 19 kill the alien military ship with balls
	ACH_DASH,				// 20 dash # times
	ACH_NOTES,				// 21 find # notes
	ACH_SECRETS,			// 22 find # secrets
	ACH_KILLENEMIESBALLS,	// 23 kill # enemies with balls (weapon_ar2 or func_tankball)
	ACH_REDDWELLER,			// 24 help the red dweller escape (chapter 1)
	ACH_ASSEMBLEBLASTLEVEL,	// 25 get the first blast level by assembling the module on ch2map2
	ACH_BROKENCAR,			// 26 break the car completely in chapter 1 intro
	ACH_CARDISTANCE,		// 27 travelled distance by car
	ACH_WATERJETDISTANCE,	// 28 travelled distance by water jet
	ACH_KILLBOTS,			// 29 kill # bots in multiplayer
	ACH_CH3_NOKILLDW,		// 30 don't kill any dwellers in chapter 3
	ACH_CH3_3MINS,			// 31 destroy the computer within 3 minutes
};

#define TOTAL_ACHIEVEMENTS 32

class CMenuAchievements;

class CMenuAchievementModel : public CMenuBaseModel
{
public:
	CMenuAchievementModel( CMenuAchievements *parent ) : parent( parent ) { }

	void Update();
	int GetRows() const
	{
		return TOTAL_ACHIEVEMENTS;
	}
	int GetColumns() const
	{
		return 5; // title, description, current value, goal value, completed(yes/no, 1/0)
	}
	const char *GetCellText( int line, int column )
	{
		switch( column )
		{
		case 0: return title[line];
		case 1: return description[line];
		case 2: return current[line];
		case 3: return goal[line];
		case 4: return completed[line];
		}

		return NULL;
	}

	unsigned int GetAlignmentForColumn( int column ) const override
	{
		if( column > 1 )
			return QM_CENTER;
		return QM_LEFT;
	}

	bool IsCellTextWrapped( int line, int column )
	{
		return false;
	}

	bool IsLineUsable( int line )
	{
		return false;
	}

	char title[TOTAL_ACHIEVEMENTS][100];
	char description[TOTAL_ACHIEVEMENTS][150];
	char current[TOTAL_ACHIEVEMENTS][64];
	char goal[TOTAL_ACHIEVEMENTS][64];
	char completed[TOTAL_ACHIEVEMENTS][64];
	int m_iNumItems;
private:
	CMenuAchievements *parent;
};


class CMenuAchievements: public CMenuFramework
{
private:
	void _Init( void ) override;
	void _VidInit() override;

public:
	typedef CMenuFramework BaseClass;
	CMenuAchievements() : CMenuFramework( "CMenuAchievements" ), achListModel( this ) { }
	void Show();
	void Hide();
//	void SaveAndPopMenu() override;
	void GetConfig( void );
	int	outlineWidth;
	void ResetAchievements( );
	void ShowDialog();
	void Draw();

	float UpdateTime; // delay the file update after changes
	bool DoUpdate;

	// update dialog
	CMenuYesNoMessageBox msgBox;

	CMenuTable achList;
	CMenuAchievementModel achListModel;
};

/*
=================
CMenuAchievements::Init
=================
*/
void CMenuAchievements::_Init( void )
{
	UpdateTime = uiStatic.realTime;
	DoUpdate = false;

	banner.SetPicture( ART_BANNER );

	msgBox.SetMessage( L( "Are you sure? This action can't be undone." ) );
	msgBox.onPositive = VoidCb( &CMenuAchievements::ResetAchievements );

	msgBox.Link( this );

	AddItem( background );
	AddItem( banner );

//	AddButton( L( "Update" ), L( "Check for updates" ),
//		PC_UPDATE, msgBox.MakeOpenEvent(), QMF_NOTIFY );

	CMenuPicButton *refresh = AddButton( L( "Refresh" ), L( "Reload stats" ),
		PC_REFRESH, VoidCb( &CMenuAchievements::GetConfig ), QMF_NOTIFY );
	refresh->pos.y = 650;

	CMenuPicButton *reset = AddButton( L( "Reset" ), L( "Reset stats" ),
		PC_RESET, VoidCb( &CMenuAchievements::ShowDialog ), QMF_NOTIFY );
	reset->pos.y = 700;


	CMenuPicButton *done = AddButton( L( "Done" ), L( "Go back to the Main menu" ),
		PC_DONE, VoidCb( &CMenuAchievements::Hide ), QMF_NOTIFY );
	done->pos.y = 750;

	achList.SetRect( 300, 200, 1500, 820 );
	achList.SetModel( &achListModel );
	achList.SetupColumn( 0, L( "Achievement" ), 0.3f );
	achList.SetupColumn( 1, L( "Description" ), 0.5f );
	achList.SetupColumn( 2, L( "Current" ), 0.0666f );
	achList.SetupColumn( 3, L( "Goal" ), 0.0666f );
	achList.SetupColumn( 4, L( "Completed?" ), 0.0666f );
	achList.charSize = 25;
	AddItem( achList );
}

void CMenuAchievements::Draw(void)
{
	if( DoUpdate && uiStatic.realTime > UpdateTime )
	{
		DoUpdate = false;
		achListModel.Update();
	}

	CMenuFramework::Draw();
}

void CMenuAchievements::Show(void)
{
	CMenuFramework::Show();
	EngFuncs::ClientCmd(FALSE, "menuactivate achievements\n");
}

void CMenuAchievements::Hide(void)
{
	CMenuFramework::Hide();
	EngFuncs::ClientCmd(FALSE, "menuactivate mainmenu\n");
}

void CMenuAchievements::GetConfig()
{
	EngFuncs::ClientCmd( FALSE, "ach_refresh\n" );

	UpdateTime = uiStatic.realTime + 0.5;
	DoUpdate = true;
}

void CMenuAchievements::_VidInit()
{
	outlineWidth = 20;
	UI_ScaleCoords( NULL, NULL, &outlineWidth, NULL );
	GetConfig();
}

void CMenuAchievements::ShowDialog()
{
	msgBox.Show();
}

void CMenuAchievements::ResetAchievements( )
{
	EngFuncs::ClientCmd( FALSE, "ach_reset\n" );

	UpdateTime = uiStatic.realTime + 0.5;
	DoUpdate = true;
}

ADD_MENU( menu_achievements, CMenuAchievements, UI_Achievements_Menu );

void CMenuAchievementModel::Update( void )
{
	char *afile = (char *)EngFuncs::COM_LoadFile( "data/achievements.txt", NULL );
	char *pfile = afile;
	char token[1024];
	int i = 0;

	if( !afile )
	{
		m_iNumItems = 0;

		Con_Printf( "UI: achievements.txt not found\n" );
		return;
	}

	memset( current, 0, sizeof( current ) );
	memset( goal, 0, sizeof( goal ) );
	memset( completed, 0, sizeof( completed ) );

	pfile = afile;
	for( i = 0; i < TOTAL_ACHIEVEMENTS; i++ )
	{
		pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );
		if( !pfile )
		{
			// line is incomplete
			Con_Printf( "^3Error:^7 achievement file has an incomplete line 1. Achievements are disabled.\n" );
			return;
		}
		if( i == ACH_CARDISTANCE || i == ACH_WATERJETDISTANCE )
			sprintf( current[i], "%s m", token );
		else
			sprintf( current[i], token );
		//	gEngfuncs.Con_Printf( "s %i %i\n", i, AchievementStats[i] );
	}

	// second line is the goals
	pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );

	for( i = 0; i < TOTAL_ACHIEVEMENTS; i++ )
	{
		if( i > 0 ) pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );
		if( !pfile )
		{
			// line is incomplete
			Con_Printf( "^3Error:^7 achievement file has an incomplete line 2. Achievements are disabled.\n" );
			return;
		}
		if( i == ACH_CARDISTANCE || i == ACH_WATERJETDISTANCE )
			sprintf( goal[i], "%s m", token );
		else
			sprintf( goal[i], token );
		//	gEngfuncs.Con_Printf( "g %i %i\n", i, AchievementGoal[i] );
	}

	// third line is the numbers, 0 or 1, checking if the achievement is completed
	pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );

	for( i = 0; i < TOTAL_ACHIEVEMENTS; i++ )
	{
		if( i > 0 ) pfile = EngFuncs::COM_ParseFile( pfile, token, sizeof( token ) );
		if( !pfile )
		{
			// line is incomplete
			Con_Printf( "^3Error:^7 achievement file has an incomplete line 3. Achievements are disabled.\n" );
			return;
		}
		if( atoi( token ) )
			sprintf( completed[i], "^2YES^7" );
		else
			sprintf( completed[i], "^1NO^7" );
		//	gEngfuncs.Con_Printf( "c %i %i\n", i, AchievementComplete[i] );
	}

	m_iNumItems = TOTAL_ACHIEVEMENTS;

	EngFuncs::COM_FreeFile( afile );

	memset( title, 0, sizeof( title ) );
	char yes[64];
	sprintf( yes, "^2YES^7" );

	for( int i=0; i<TOTAL_ACHIEVEMENTS; i++)
	{
		switch( i )
		{
		case ACH_BULLETSFIRED: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_BULLETSFIRED], "^2A MILLION DOLLAR WASTE^7" ); else sprintf( title[ACH_BULLETSFIRED], "^1< LOCKED >^7" ); break;
		case ACH_JUMPS: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_JUMPS], "^2BUNNY-HOPPER^7" ); else sprintf( title[ACH_JUMPS], "^1< LOCKED >^7" ); break;
		case ACH_AMMOCRATES: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_AMMOCRATES], "^2A MYSTERY BOX^7" ); else sprintf( title[ACH_AMMOCRATES], "^1< LOCKED >^7" ); break;
		case ACH_DISARMEDMINES: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_DISARMEDMINES], "^2SAPPER^7" ); else sprintf( title[ACH_DISARMEDMINES], "^1< LOCKED >^7" ); break;
		case ACH_KILLENEMIES: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_KILLENEMIES], "^2ONE MAN ARMY^7" ); else sprintf( title[ACH_KILLENEMIES], "^1< LOCKED >^7" ); break;
		case ACH_INFLICTDAMAGE: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_INFLICTDAMAGE], "^2THAT'S A LOTTA DAMAGE^7" ); else sprintf( title[ACH_INFLICTDAMAGE], "^1< LOCKED >^7" ); break;
		case ACH_KILLENEMIESSNIPER: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_KILLENEMIESSNIPER], "^2PRECISION MATTERS^7" ); else sprintf( title[ACH_KILLENEMIESSNIPER], "^1< LOCKED >^7" ); break;
		case ACH_CH1: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CH1], "^2BACK INTO ACTION^7" ); else sprintf( title[ACH_CH1], "^1< LOCKED >^7" ); break;
		case ACH_CH2: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CH2], "^2INTERDIMENSIONAL TRAVELLER^7" ); else sprintf( title[ACH_CH2], "^1< LOCKED >^7" ); break;
		case ACH_CH3: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CH3], "^2I'M FEELING BLUE^7" ); else sprintf( title[ACH_CH3], "^1< LOCKED >^7" ); break;
		case ACH_CH4: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CH4], "missing" ); else sprintf( title[ACH_CH4], "^1< LOCKED >^7" ); break;
		case ACH_CH5: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CH5], "missing" ); else sprintf( title[ACH_CH5], "^1< LOCKED >^7" ); break;
		case ACH_GENERAL30SEC: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_GENERAL30SEC], "missing" ); else sprintf( title[ACH_GENERAL30SEC], "^1< LOCKED >^7" ); break;
		case ACH_HPREGENERATE: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_HPREGENERATE], "^2NINE LIVES^7" ); else sprintf( title[ACH_HPREGENERATE], "^1< LOCKED >^7" ); break;
		case ACH_RECEIVEDAMAGE: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_RECEIVEDAMAGE], "^2STILL STANDING^7" ); else sprintf( title[ACH_RECEIVEDAMAGE], "^1< LOCKED >^7" ); break;
		case ACH_OVERCOOK: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_OVERCOOK], "^2DING! DINNER'S READY^7" ); else sprintf( title[ACH_OVERCOOK], "^1< LOCKED >^7" ); break;
		case ACH_DRONESEC: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_DRONESEC], "missing" ); else sprintf( title[ACH_DRONESEC], "^1< LOCKED >^7" ); break;
		case ACH_DRONEALIEN: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_DRONEALIEN], "missing" ); else sprintf( title[ACH_DRONEALIEN], "^1< LOCKED >^7" ); break;
		case ACH_CROSSBOW: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CROSSBOW], "^2CALCULATED TRAJECTORY^7" ); else sprintf( title[ACH_CROSSBOW], "^1< LOCKED >^7" ); break;
		case ACH_TANKBALL: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_TANKBALL], "missing" ); else sprintf( title[ACH_TANKBALL], "^1< LOCKED >^7" ); break;
		case ACH_DASH: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_DASH], "^2GOTTA GO FAST^7" ); else sprintf( title[ACH_DASH], "^1< LOCKED >^7" ); break;
		case ACH_NOTES: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_NOTES], "^2ARCHIVIST^7" ); else sprintf( title[ACH_NOTES], "^1< LOCKED >^7" ); break;
		case ACH_SECRETS: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_SECRETS], "missing" ); else sprintf( title[ACH_SECRETS], "^1< LOCKED >^7" ); break;
		case ACH_KILLENEMIESBALLS: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_KILLENEMIESBALLS], "missing" ); else sprintf( title[ACH_KILLENEMIESBALLS], "^1< LOCKED >^7" ); break;
		case ACH_REDDWELLER:if( !strcmp(completed[i], yes) ) sprintf( title[ACH_REDDWELLER], "^2BON VOYAGE!^7" ); else sprintf( title[ACH_REDDWELLER], "^1< LOCKED >^7" ); break;
		case ACH_ASSEMBLEBLASTLEVEL: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_ASSEMBLEBLASTLEVEL], "^2YOU KNOW, I'M SOMETHING OF A SCIENTIST MYSELF^7" ); else sprintf( title[ACH_ASSEMBLEBLASTLEVEL], "^1< LOCKED >^7" ); break;
		case ACH_BROKENCAR: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_BROKENCAR], "^2END OF THE ROAD^7" ); else sprintf( title[ACH_BROKENCAR], "^1< LOCKED >^7" ); break;
		case ACH_CARDISTANCE: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_CARDISTANCE], "^2LICENSE AND REGISTRATION PLEASE^7" ); else sprintf( title[ACH_CARDISTANCE], "^1< LOCKED >^7" ); break;
		case ACH_WATERJETDISTANCE: if( !strcmp(completed[i], yes) ) sprintf( title[ACH_WATERJETDISTANCE], "^2SAILOR^7" ); else sprintf( title[ACH_WATERJETDISTANCE], "^1< LOCKED >^7" ); break;
		case ACH_KILLBOTS: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_KILLBOTS], "^2RAGE AGAINST THE MACHINE^7" ); else sprintf( title[ACH_KILLBOTS], "^1< LOCKED >^7" ); break;
		case ACH_CH3_NOKILLDW: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_CH3_NOKILLDW], "^2A TRUE SAVIOUR^7" ); else sprintf( title[ACH_CH3_NOKILLDW], "^1< LOCKED >^7" ); break;
		case ACH_CH3_3MINS: if( !strcmp( completed[i], yes ) ) sprintf( title[ACH_CH3_3MINS], "^2RAPID DISASSEMBLY^7" ); else sprintf( title[ACH_CH3_3MINS], "^1< LOCKED >^7" ); break;
		default: break;
		}
	}

	memset( description, 0, sizeof( description ) );

	for( int i = 0; i < TOTAL_ACHIEVEMENTS; i++ )
	{
		switch( i )
		{
		case ACH_BULLETSFIRED: sprintf( description[ACH_BULLETSFIRED], "Fire 100000 bullets" ); break;
		case ACH_JUMPS: sprintf( description[ACH_JUMPS], "Jump 1000 times" ); break;
		case ACH_AMMOCRATES: sprintf( description[ACH_AMMOCRATES], "Loot 100 ammo crates" ); break;
		case ACH_DISARMEDMINES: sprintf( description[ACH_DISARMEDMINES], "Disarm 30 mines (SP)" ); break;
		case ACH_KILLENEMIES: sprintf( description[ACH_KILLENEMIES], "Kill 500 enemies (SP)" ); break;
		case ACH_INFLICTDAMAGE: sprintf( description[ACH_INFLICTDAMAGE], "Inflict 10000 damage" ); break;
		case ACH_KILLENEMIESSNIPER: sprintf( description[ACH_KILLENEMIESSNIPER], "Kill 100 enemies with a Barret sniper rifle" ); break;
		case ACH_CH1: sprintf( description[ACH_CH1], "Complete Chapter 1" ); break;
		case ACH_CH2: sprintf( description[ACH_CH2], "Complete Chapter 2" ); break;
		case ACH_CH3: sprintf( description[ACH_CH3], "Complete Chapter 3" ); break;
		case ACH_CH4: sprintf( description[ACH_CH4], "Complete Chapter 4" ); break;
		case ACH_CH5: sprintf( description[ACH_CH5], "Complete Chapter 5" ); break;
		case ACH_GENERAL30SEC: sprintf( description[ACH_GENERAL30SEC], "Kill the security general within 30 seconds (SP)" ); break;
		case ACH_HPREGENERATE: sprintf( description[ACH_HPREGENERATE], "Regenerate 10000 hp" ); break;
		case ACH_RECEIVEDAMAGE: sprintf( description[ACH_RECEIVEDAMAGE], "Receive 10000 damage" ); break;
		case ACH_OVERCOOK: sprintf( description[ACH_OVERCOOK], "Overcook a grenade" ); break;
		case ACH_DRONESEC: sprintf( description[ACH_DRONESEC], "< # >" ); break;
		case ACH_DRONEALIEN: sprintf( description[ACH_DRONEALIEN], "< # >" ); break;
		case ACH_CROSSBOW: sprintf( description[ACH_CROSSBOW], "Kill 10 enemies with a crossbow over a distance of 50 meters" ); break;
		case ACH_TANKBALL: sprintf( description[ACH_TANKBALL], "< # >" ); break;
		case ACH_DASH: sprintf( description[ACH_DASH], "Dash 100 times" ); break;
		case ACH_NOTES: sprintf( description[ACH_NOTES], "Collect 50 notes (SP)" ); break;
		case ACH_SECRETS: sprintf( description[ACH_SECRETS], "< # >" ); break;
		case ACH_KILLENEMIESBALLS: sprintf( description[ACH_KILLENEMIESBALLS], "< # >" ); break;
		case ACH_REDDWELLER: sprintf( description[ACH_REDDWELLER], "Help the Red Dweller to go back home (SP)" ); break;
		case ACH_ASSEMBLEBLASTLEVEL: sprintf( description[ACH_ASSEMBLEBLASTLEVEL], "Acquire first electroblast update using the recipe (SP)" ); break;
		case ACH_BROKENCAR: sprintf( description[ACH_BROKENCAR], "Break the car completely in the beginning of Chapter 1 (SP)" ); break;
		case ACH_CARDISTANCE: sprintf( description[ACH_CARDISTANCE], "Travel 10 km by land vehicle" ); break;
		case ACH_WATERJETDISTANCE: sprintf( description[ACH_WATERJETDISTANCE], "Travel 5 km by water vehicle" ); break;
		case ACH_KILLBOTS: sprintf( description[ACH_KILLBOTS], "Kill 1000 bots (MP)" ); break;
		case ACH_CH3_NOKILLDW: sprintf( description[ACH_CH3_NOKILLDW], "Don't kill any dwellers in Chapter 3" ); break;
		case ACH_CH3_3MINS: sprintf( description[ACH_CH3_3MINS], "Destroy the computer within 3 minutes in Chapter 3" ); break;
		default: break;
		}
	}
}
