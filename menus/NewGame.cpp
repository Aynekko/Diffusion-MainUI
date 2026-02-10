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
#include "PicButton.h"
#include "YesNoMessageBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"
#include <algorithm>
#include <vector>

#define ART_BANNER		"gfx/shell/head_newgame"

static const char *chapter_str[5];
static const char *chapter_str_sel[5];
static float lerp_value = 1.0f;
static int SELECTED_CHAPTER = 0;
const float PIC_WIDTH = 576.0f; // 1920x1080 x 0.3
const float PIC_HEIGHT = 324.0f;
const float PIC_Y_POS = 100.0f;
const float PIC_X_POS = 80.0f;
static float pic_width;
static float pic_height;
static float pic_x_pos;
static float pic_y_pos;

float lerp( float start, float end, float frac )
{
	frac = bound( 0.0f, frac, 1.0f );

	// Exact, monotonic, bounded, determinate, and (for a=b=0) consistent:
	if( start <= 0 && end >= 0 || start >= 0 && end <= 0 )
		return frac * end + (1 - frac) * start;

	if( frac == 1 )
		return end; // exact
	// Exact at t=0, monotonic except near t=1,
	// bounded, determinate, and consistent:
	const float x = start + frac * (end - start);
	return frac > 1 == end > start ? Q_max( end, x ) : Q_min( end, x ); // monotonic near t=1
}

class CMenuNewGame : public CMenuFramework
{
public:
	CMenuNewGame() : CMenuFramework( "CMenuNewGame" ) { }
	static void StartGameCb( float skill );
	void Show() override
	{
		if( gMenu.m_gameinfo.flags & GFL_NOSKILLS )
		{
			StartGameCb( 1.0f );
			return;
		}
		EngFuncs::ClientCmd(false, "menuactivate newgame");
		chapters.SetCurrentValue( 0.0f );
		lerp_value = 1.0f;

		CMenuFramework::Show();
	}
	void Hide();
	void GetConfig(void);
	void UpdateChapter( void );
	void Draw( void ) override;
private:
	void _Init() override;
	void _VidInit() override;
	void SetPicPos();

	static void ShowDialogCb( CMenuBaseItem *pSelf, void *pExtra  );

	CMenuYesNoMessageBox  msgBox;

	CEventCallback easyCallback;
	CEventCallback normCallback;
	CEventCallback hardCallback;

	CImage chapters_img[5];
	CImage chapters_img_bw[5]; // black'n'white
	CMenuSpinControl chapters;
};

void CMenuNewGame::SetPicPos()
{
	pic_width = PIC_WIDTH * uiStatic.scaleX;
	pic_height = PIC_HEIGHT * uiStatic.scaleY;
	pic_x_pos = chapters.pos.x;
	pic_y_pos = (chapters.pos.y - PIC_HEIGHT - 100) * uiStatic.scaleY;
}

void CMenuNewGame::_VidInit()
{
	GetConfig();
	SetPicPos();
}

void CMenuNewGame::GetConfig()
{
	
}

/*
=================
CMenuNewGame::StartGame
=================
*/
void CMenuNewGame::StartGameCb( float skill )
{
	if( EngFuncs::GetCvarFloat( "host_serverstate" ) && EngFuncs::GetCvarFloat( "maxplayers" ) > 1 )
		EngFuncs::HostEndGame( "end of the game" );

	EngFuncs::CvarSetValue( "skill", skill );
	EngFuncs::CvarSetValue( "deathmatch", 0.0f );
	EngFuncs::CvarSetValue( "teamplay", 0.0f );
	EngFuncs::CvarSetValue( "pausable", 1.0f ); // singleplayer is always allowing pause
	EngFuncs::CvarSetValue( "maxplayers", 1.0f );
	EngFuncs::CvarSetValue( "coop", 0.0f );

	EngFuncs::PlayBackgroundTrack( NULL, NULL );

	switch( SELECTED_CHAPTER )
	{
	default:
	case 0:
		EngFuncs::ClientCmd( false, "newgame\n" );
		break;
	case 1:
		EngFuncs::ClientCmd( false, "map ch2map1\n" );
		break;
	case 2:
		EngFuncs::ClientCmd( false, "map ch3map1\n" );
		break;
	case 3:
		EngFuncs::ClientCmd( false, "map ch4map2\n" );
		break;
	case 4:
		EngFuncs::ClientCmd( false, "map ch5map1\n" );
		break;
	}
}

void CMenuNewGame::ShowDialogCb( CMenuBaseItem *pSelf, void *pExtra )
{
	CMenuNewGame *ui = (CMenuNewGame*)pSelf->Parent();

	ui->msgBox.onPositive = *(CEventCallback*)pExtra;
	ui->msgBox.Show();
}

void CMenuNewGame::UpdateChapter()
{
	lerp_value = 0.0f;
	SELECTED_CHAPTER = (int)chapters.GetCurrentValue();
}

/*
=================
CMenuNewGame::Init
=================
*/
void CMenuNewGame::_Init( void )
{
	AddItem( banner );

	banner.SetPicture( ART_BANNER );

	SET_EVENT( easyCallback, CMenuNewGame::StartGameCb( 1.0f ) );
	SET_EVENT( normCallback, CMenuNewGame::StartGameCb( 2.0f ) );
	SET_EVENT( hardCallback, CMenuNewGame::StartGameCb( 3.0f ) );

	chapter_str_sel[0] = L( "GameUI_Chapter1Sel" );
	chapter_str_sel[1] = L( "GameUI_Chapter2Sel" );
	chapter_str_sel[2] = L( "GameUI_Chapter3Sel" );
	chapter_str_sel[3] = L( "GameUI_Chapter4Sel" );
	chapter_str_sel[4] = L( "GameUI_Chapter5Sel" );

	chapter_str[0] = L( "GameUI_Chapter1" );
	chapter_str[1] = L( "GameUI_Chapter2" );
	chapter_str[2] = L( "GameUI_Chapter3" );
	chapter_str[3] = L( "GameUI_Chapter4" );
	chapter_str[4] = L( "GameUI_Chapter5" );

	static const char *CHstr[] =
	{
		chapter_str_sel[0], chapter_str_sel[1], chapter_str_sel[2], chapter_str_sel[3], chapter_str_sel[4]
	};

	static CStringArrayModel aasing( CHstr, V_ARRAYSIZE( CHstr ) );
	chapters.SetNameAndStatus( L( "GameUI_SelectChapter" ), "" );
	chapters.Setup( &aasing );
	chapters.onChanged = VoidCb( &CMenuNewGame::UpdateChapter );
	chapters.font = QM_SMALLFONT;
	chapters.SetRect( 80, MenuYOffset + 150, 300, 30 );
	chapters.SetCurrentValue( 0.0f );
	chapters_img[0].Load( "gfx/shell/menu_chapter1.tga" );
	chapters_img[1].Load( "gfx/shell/menu_chapter2.tga" );
	chapters_img[2].Load( "gfx/shell/menu_chapter3.tga" );
	chapters_img[3].Load( "gfx/shell/menu_chapter4.tga" );
	chapters_img[4].Load( "gfx/shell/menu_chapter5.tga" );
	chapters_img_bw[0].Load( "gfx/shell/menu_chapter1bw.tga" );
	chapters_img_bw[1].Load( "gfx/shell/menu_chapter2bw.tga" );
	chapters_img_bw[2].Load( "gfx/shell/menu_chapter3bw.tga" );
	chapters_img_bw[3].Load( "gfx/shell/menu_chapter4bw.tga" );
	chapters_img_bw[4].Load( "gfx/shell/menu_chapter5bw.tga" );
	
	AddItem( chapters );

	SetPicPos();
	
	CMenuPicButton *easy = AddButton( L( "GameUI_Easy" ), L( "StringsList_200" ), PC_EASY, easyCallback, QMF_NOTIFY );
	easy->pos.y = chapters.pos.y + 50;
	CMenuPicButton *norm = AddButton( L( "GameUI_Medium" ), L( "StringsList_201" ), PC_MEDIUM, normCallback, QMF_NOTIFY );
	norm->pos.y = easy->pos.y + 50;
	CMenuPicButton *hard = AddButton( L( "GameUI_Hard" ), L( "StringsList_202" ), PC_DIFFICULT, hardCallback, QMF_NOTIFY );
	hard->pos.y = norm->pos.y + 50;

	easy->onReleasedClActive =
		norm->onReleasedClActive =
		hard->onReleasedClActive = ShowDialogCb;
	easy->onReleasedClActive.pExtra = &easyCallback;
	norm->onReleasedClActive.pExtra = &normCallback;
	hard->onReleasedClActive.pExtra = &hardCallback;

	CMenuPicButton *cancel = AddButton( L( "GameUI_Cancel" ), L( "Go back to the Main menu" ), PC_CANCEL, VoidCb( &CMenuNewGame::Hide ), QMF_NOTIFY );
	cancel->pos.y = hard->pos.y + 100;

	msgBox.SetMessage( L( "StringsList_240" ) );
	msgBox.HighlightChoice( CMenuYesNoMessageBox::HIGHLIGHT_NO );
	msgBox.Link( this );
}

void CMenuNewGame::Draw( void )
{
	static float nexttime = 0, lasttime = 0;
	float newtime = (float)EngFuncs::DoubleTime();
	if( !lasttime )
		lasttime = newtime - 0.001f;
	float frametime = (newtime - lasttime);

	const int iSelectedChapter = (int)chapters.GetCurrentValue();

	static float fSelectedChapter = 0;

	if( lerp_value < 1.0f )
		lerp_value += frametime;
	lerp_value = bound( 0.0f, lerp_value, 1.0f );
	fSelectedChapter = lerp( fSelectedChapter, (float)iSelectedChapter, lerp_value );

	int w[5];
	int h[5];
	float size_factor[5];

	for( int i = 0; i < 5; i++ )
	{
		float distance = (float)i - fSelectedChapter;
		size_factor[i] = 1.0f - 0.025f * distance * distance;
		size_factor[i] = Q_max( 0.5f, size_factor[i] );

		w[i] = pic_width * size_factor[i];
		h[i] = pic_height * size_factor[i];
	}

	// sort here...
	std::vector<int> widths = { w[0], w[1], w[2], w[3], w[4] };
	std::vector<size_t> render_order( widths.size() );
	for( size_t i = 0; i < widths.size(); ++i )
		render_order[i] = i;

	std::stable_sort( render_order.begin(), render_order.end(), [&]( size_t a, size_t b )
	{
		return widths[a] < widths[b];
	} );

	// draw the pics
	float Xpos[5];
	Xpos[0] = pic_x_pos;
	for( int i = 1; i < 5; ++i )
	{
		float overlap = 300.0f * uiStatic.scaleX * size_factor[i];
		Xpos[i] = Xpos[i - 1] + w[i - 1] - overlap;
	}

	// render the unselected chapters
	for( size_t i : render_order )
	{
		if( i == iSelectedChapter )
			continue;

		unsigned int color = PackRGBA( 100, 100, 100, 255 );
		const int outline_w = 5;
		Point pt;
		Size sz;
		pt.x = Xpos[i];
		pt.y = pic_y_pos + (pic_height - h[i]) * 0.5f;
		sz.w = w[i];
		sz.h = h[i];
		UI_DrawPic( pt, sz, uiColorWhite, chapters_img_bw[i] );
		UI_DrawRectangleExt( pt, sz, color, outline_w );
		
		// test
	//	UI_DrawString( font, Xpos[i], pt.y + h[i], m_scSize.w, m_scChSize, chapter_str[i], uiPromptFocusColor, m_scChSize, QM_LEFT, ETF_SHADOW | ETF_FORCECOL );
	}

	// render the selected last
	unsigned int color = PackRGBA( 70, 169, 255, 255 );
	const int outline_w = 5;
	Point pt;
	Size sz;
	pt.x = Xpos[iSelectedChapter];
	pt.y = pic_y_pos + (pic_height - h[iSelectedChapter]) * 0.5f;
	sz.w = w[iSelectedChapter];
	sz.h = h[iSelectedChapter];
	UI_DrawPic( pt, sz, uiColorWhite, chapters_img[iSelectedChapter] );
	UI_DrawRectangleExt( pt, sz, color, outline_w );
	UI_DrawString( font, Xpos[iSelectedChapter], pt.y + h[iSelectedChapter] + 10, m_scSize.w, m_scChSize, chapter_str[iSelectedChapter], uiPromptTextColor, m_scChSize, QM_LEFT, ETF_SHADOW | ETF_FORCECOL );
	
	lasttime = newtime;

	BaseClass::Draw();
}

void CMenuNewGame::Hide(void)
{
	CMenuFramework::Hide();
	EngFuncs::ClientCmd(false, "menuactivate mainmenu\n");
}

ADD_MENU( menu_newgame, CMenuNewGame, UI_NewGame_Menu );
