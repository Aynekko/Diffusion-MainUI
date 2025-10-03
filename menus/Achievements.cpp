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

#define TOTAL_ACHIEVEMENTS 35

// must be the same as in client
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
	ACH_DASH,				// 19 dash # times
	ACH_NOTES,				// 20 find # notes
	ACH_KILLENEMIESBALLS,	// 21 kill # enemies with balls (weapon_ar2 or func_tankball)
	ACH_REDDWELLER,			// 22 help the red dweller escape (chapter 1)
	ACH_ASSEMBLEBLASTLEVEL,	// 23 get the first blast level by assembling the module on ch2map2
	ACH_BROKENCAR,			// 24 break the car completely in chapter 1 intro
	ACH_CARDISTANCE,		// 25 travelled distance by car
	ACH_WATERJETDISTANCE,	// 26 travelled distance by water jet
	ACH_KILLBOTS,			// 27 kill # bots in multiplayer
	ACH_CH3_NOKILLDW,		// 28 don't kill any dwellers in chapter 3
	ACH_CH3_3MINS,			// 29 destroy the computer within 3 minutes
	ACH_5DRONES,			// 30 kill 5 enemy drones while piloting a drone from 1st person
	ACH_DRUNK,				// 31 kill 5 enemies while drunk
	ACH_DIDNTLISTEN,		// 32 you didn't listen to Alice in ch5map2
	ACH_CARMAGEDDON,		// 33 run over 5 enemies with a car
	ACH_ELECTROBLAST,		// 34 kill 30 enemies with electroblast
};

const char *AchievementTitles[TOTAL_ACHIEVEMENTS + 1] = // color used: ^2
{
	"GameUI_ACHTITLE_BULLETSFIRED",		// 0 fire # bullets
	"GameUI_ACHTITLE_JUMPS",			// 1 jump # times
	"GameUI_ACHTITLE_AMMOCRATES",		// 2 find # ammo crates
	"GameUI_ACHTITLE_DISARMEDMINES",	// 3 disarm # enemy mines (disarming your own doesn't count)
	"GameUI_ACHTITLE_KILLENEMIES",		// 4 kill # enemies
	"GameUI_ACHTITLE_INFLICTDAMAGE",	// 5 inflict a total of # damage
	"GameUI_ACHTITLE_KILLENEMIESSNIPER",// 6 kill # enemies with a stationary sniper rifle (func_tank)
	"GameUI_ACHTITLE_CH1",				// 7 complete chapter
	"GameUI_ACHTITLE_CH2",				// 8
	"GameUI_ACHTITLE_CH3",				// 9
	"GameUI_ACHTITLE_CH4",				// 10
	"GameUI_ACHTITLE_CH5",				// 11
	"GameUI_ACHTITLE_GENERAL30SEC",		// 12 kill security general under 30 sec
	"GameUI_ACHTITLE_HPREGENERATE",		// 13 regenerate a total of # health
	"GameUI_ACHTITLE_RECEIVEDAMAGE",	// 14 receive a total of # damage
	"GameUI_ACHTITLE_OVERCOOK",			// 15 overcook the grenade
	"GameUI_ACHTITLE_DRONESEC",			// 16 kill # security drones
	"GameUI_ACHTITLE_DRONEALIEN",		// 17 kill # alien drones
	"GameUI_ACHTITLE_CROSSBOW",			// 18 kill # enemies on a certain distance with a crossbow
	"GameUI_ACHTITLE_DASH",				// 19 dash # times
	"GameUI_ACHTITLE_NOTES",			// 20 find # notes
	"GameUI_ACHTITLE_KILLENEMIESBALLS",	// 21 kill # enemies with balls (weapon_ar2 or func_tankball)
	"GameUI_ACHTITLE_REDDWELLER",		// 22 help the red dweller escape (chapter 1)
	"GameUI_ACHTITLE_ASSEMBLEBLASTLEVEL",	// 23 get the first blast level by assembling the module on ch2map2
	"GameUI_ACHTITLE_BROKENCAR",		// 24 break the car completely in chapter 1 intro
	"GameUI_ACHTITLE_CARDISTANCE",		// 25 travelled distance by car
	"GameUI_ACHTITLE_WATERJETDISTANCE",	// 26 travelled distance by water jet
	"GameUI_ACHTITLE_KILLBOTS",			// 27 kill # bots in multiplayer
	"GameUI_ACHTITLE_CH3_NOKILLDW",		// 28 don't kill any dwellers in chapter 3
	"GameUI_ACHTITLE_CH3_3MINS",		// 29 destroy the computer within 3 minutes
	"GameUI_ACHTITLE_5DRONES",			// 30 kill 5 enemy drones while piloting a drone from 1st person
	"GameUI_ACHTITLE_DRUNK",			// 31 kill 5 enemies while drunk
	"GameUI_ACHTITLE_DIDNTLISTEN",		// 32 you didn't listen to Alice in ch5map2
	"GameUI_ACHTITLE_CARMAGEDDON",		// 33 run over 5 enemies with a car
	"GameUI_ACHTITLE_ELECTROBLAST",		// 34 kill 30 enemies with electroblast
	"GameUI_ACHTITLE_LOCKED",			// ALWAYS LAST LINE (= TOTAL_ACHIEVEMENTS)
};

const char *AchievementDescriptions[TOTAL_ACHIEVEMENTS] = // color used: white
{
	"GameUI_ACH_BULLETSFIRED",		// 0 fire # bullets
	"GameUI_ACH_JUMPS",				// 1 jump # times
	"GameUI_ACH_AMMOCRATES",		// 2 find # ammo crates
	"GameUI_ACH_DISARMEDMINES",		// 3 disarm # enemy mines (disarming your own doesn't count)
	"GameUI_ACH_KILLENEMIES",		// 4 kill # enemies
	"GameUI_ACH_INFLICTDAMAGE",		// 5 inflict a total of # damage
	"GameUI_ACH_KILLENEMIESSNIPER",	// 6 kill # enemies with a stationary sniper rifle (func_tank)
	"GameUI_ACH_CH1",				// 7 complete chapter
	"GameUI_ACH_CH2",				// 8
	"GameUI_ACH_CH3",				// 9
	"GameUI_ACH_CH4",				// 10
	"GameUI_ACH_CH5",				// 11
	"GameUI_ACH_GENERAL30SEC",		// 12 kill security general under 30 sec
	"GameUI_ACH_HPREGENERATE",		// 13 regenerate a total of # health
	"GameUI_ACH_RECEIVEDAMAGE",		// 14 receive a total of # damage
	"GameUI_ACH_OVERCOOK",			// 15 overcook the grenade
	"GameUI_ACH_DRONESEC",			// 16 kill # security drones
	"GameUI_ACH_DRONEALIEN",		// 17 kill # alien drones
	"GameUI_ACH_CROSSBOW",			// 18 kill # enemies on a certain distance with a crossbow
	"GameUI_ACH_DASH",				// 19 dash # times
	"GameUI_ACH_NOTES",				// 20 find # notes
	"GameUI_ACH_KILLENEMIESBALLS",	// 21 kill # enemies with balls (weapon_ar2 or func_tankball)
	"GameUI_ACH_REDDWELLER",		// 22 help the red dweller escape (chapter 1)
	"GameUI_ACH_ASSEMBLEBLASTLEVEL",// 23 get the first blast level by assembling the module on ch2map2
	"GameUI_ACH_BROKENCAR",			// 24 break the car completely in chapter 1 intro
	"GameUI_ACH_CARDISTANCE",		// 25 travelled distance by car
	"GameUI_ACH_WATERJETDISTANCE",	// 26 travelled distance by water jet
	"GameUI_ACH_KILLBOTS",			// 27 kill # bots in multiplayer
	"GameUI_ACH_CH3_NOKILLDW",		// 28 don't kill any dwellers in chapter 3
	"GameUI_ACH_CH3_3MINS",			// 29 destroy the computer within 3 minutes
	"GameUI_ACH_5DRONES",			// 30 kill 5 enemy drones while piloting a drone from 1st person
	"GameUI_ACH_DRUNK",				// 31 kill 5 enemies while drunk
	"GameUI_ACH_DIDNTLISTEN",		// 32 you didn't listen to Alice in ch5map2
	"GameUI_ACH_CARMAGEDDON",		// 33 run over 5 enemies with a car
	"GameUI_ACH_ELECTROBLAST",		// 34 kill 30 enemies with electroblast
};

typedef struct
{
	int goal[TOTAL_ACHIEVEMENTS];
	int value[TOTAL_ACHIEVEMENTS];
	bool completion[TOTAL_ACHIEVEMENTS];
	char name[TOTAL_ACHIEVEMENTS][100];

} achievement_data_t;

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

	msgBox.SetMessage( L( "GameUI_AchResetPrompt" ) );
	msgBox.onPositive = VoidCb( &CMenuAchievements::ResetAchievements );

	msgBox.Link( this );

	AddItem( banner );

//	AddButton( L( "Update" ), L( "Check for updates" ),
//		PC_UPDATE, msgBox.MakeOpenEvent(), QMF_NOTIFY );

	CMenuPicButton *refresh = AddButton( L( "GameUI_AchRefresh" ), L( "Reload stats" ),
		PC_REFRESH, VoidCb( &CMenuAchievements::GetConfig ), QMF_NOTIFY );
	refresh->pos.y = 650;

	CMenuPicButton *reset = AddButton( L( "GameUI_AchReset" ), L( "Reset stats" ),
		PC_RESET, VoidCb( &CMenuAchievements::ShowDialog ), QMF_NOTIFY );
	reset->pos.y = 700;


	CMenuPicButton *done = AddButton( L( "GameUI_GameMenu_BackToMenu" ), L( "Go back to the Main menu" ),
		PC_DONE, VoidCb( &CMenuAchievements::Hide ), QMF_NOTIFY );
	done->pos.y = 750;

	achList.SetRect( 300, 200, 1500, 820 );
	achList.SetModel( &achListModel );
	achList.SetupColumn( 0, L( "GameUI_AchColAchievement" ), 0.3f );
	achList.SetupColumn( 1, L( "GameUI_AchColDescription" ), 0.5f );
	achList.SetupColumn( 2, L( "GameUI_AchColCurrent" ), 0.0666f );
	achList.SetupColumn( 3, L( "GameUI_AchColGoal" ), 0.0666f );
	achList.SetupColumn( 4, L( "GameUI_AchColCompleted" ), 0.0666f );
	achList.charSize += 5; // increase distance between lines
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
	EngFuncs::ClientCmd(false, "menuactivate achievements\n");
}

void CMenuAchievements::Hide(void)
{
	CMenuFramework::Hide();
	EngFuncs::ClientCmd(false, "menuactivate mainmenu\n");
}

void CMenuAchievements::GetConfig()
{
	EngFuncs::ClientCmd( false, "ach_refresh\n" );

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
	EngFuncs::ClientCmd( false, "ach_reset\n" );

	UpdateTime = uiStatic.realTime + 0.5;
	DoUpdate = true;
}

ADD_MENU( menu_achievements, CMenuAchievements, UI_Achievements_Menu );

void CMenuAchievementModel::Update( void )
{
	const char *szFilename = "data/achievements.bin";
	int i;

	byte *aMemFile = EngFuncs::COM_LoadFile( szFilename, NULL );

	if( !aMemFile )
	{
		m_iNumItems = 0;

		Con_Printf( "^2GameUI:^7 data/achievements.bin not found\n" );
		return;
	}

	achievement_data_t *pData = (achievement_data_t *)aMemFile;

	memset( current, 0, sizeof( current ) );
	memset( goal, 0, sizeof( goal ) );
	memset( completed, 0, sizeof( completed ) );
	memset( title, 0, sizeof( title ) );
	memset( description, 0, sizeof( description ) );

	for( i = 0; i < TOTAL_ACHIEVEMENTS; i++ )
	{
		bool novalue = (pData->goal[i] == 1); // it's a simple yes/no achievement, no need to show values

		// current values
		if( i == ACH_CARDISTANCE || i == ACH_WATERJETDISTANCE )
			sprintf_s( current[i], "%i m", pData->value[i] );
		else if( novalue )
			current[i][0] = '\0';
		else
			sprintf_s( current[i], "%i", pData->value[i] );
		
		// completed or not
		if( pData->completion[i] == true )
		{
			sprintf_s( completed[i], "^2YES^7" );
			sprintf_s( title[i], "^2%s^7", L( AchievementTitles[i] ) );
		}
		else
		{
			sprintf_s( completed[i], "^1NO^7" );
			sprintf_s( title[i], "^1%s^7", L( AchievementTitles[TOTAL_ACHIEVEMENTS] ) ); // locked
		}

		// goals
		if( i == ACH_CARDISTANCE || i == ACH_WATERJETDISTANCE )
			sprintf_s( goal[i], "%i m", pData->goal[i] );
		else if( novalue )
			goal[i][0] = '\0';
		else
			sprintf_s( goal[i], "%i", pData->goal[i] );

		sprintf_s( description[i], L( AchievementDescriptions[i] ) );
	}

	m_iNumItems = TOTAL_ACHIEVEMENTS;

	EngFuncs::COM_FreeFile( aMemFile );
}
